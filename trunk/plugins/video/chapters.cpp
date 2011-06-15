// **************************************************************************
//   Copyright (C) 2004-2006 by Petri Damsten
//   petri.damsten@iki.fi
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the
//   Free Software Foundation, Inc.,
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
// **************************************************************************

#include "chapters.h"

#include <QtCore/QDir>
#include <QtGui/QImage>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMenu>
#include <QtGui/QSlider>

#include <KApplication>
#include <KDebug>
#include <KFileDialog>
#include <KIcon>
#include <KInputDialog>
#include <KLocale>
#include <KMainWindow>
#include <KMessageBox>
#include <KStandardAction>
#include <Phonon/MediaObject>

#include <kmfimageview.h>
#include <kmfmediafile.h>
#include <kmftime.h>
#include <kmftools.h>
#include <qdvdinfo.h>
#include <ui_addchapter.h>
#include <ui_autochapters.h>
#include <kmediafactory/plugin.h>
#include "videoplugin.h"

enum Columns {
    COL_NAME,
    COL_START,
    COL_LENGTH,
    COL_HIDDEN,
    COL_THUMBNAIL,
    COL_COUNT
};

static QString getFile(const QString &f)
{
    QString d(f);

    int slashPos=d.lastIndexOf('/');

    if(slashPos!=-1)
        d.remove(0, slashPos+1);

    return d;
}

class CellListModel : public QAbstractListModel
{
    public:
        CellListModel(QDVD::CellList *data, QTime total) : m_data(data), m_total(total){};

        virtual int rowCount(const QModelIndex &) const
        {
            return m_data->count();
        };

        virtual int columnCount(const QModelIndex &) const
        {
            return COL_COUNT;
        };

        void update()
        {
            reset();
        };

        virtual bool setData(const QModelIndex &index, const QVariant &value, int role)
        {
            if (!index.isValid()) {
                return false;
            }

            if (index.row() >= rowCount(index)) {
                return false;
            }

            if ((role == Qt::EditRole) && (index.column() == COL_NAME)) {
                (*m_data)[index.row()].setName(value.toString());
                emit dataChanged(index, index);
                return true;
            } else if (Qt::CheckStateRole == role && COL_HIDDEN == index.column())       {
                (*m_data)[index.row()].setHidden(!value.toBool());
                emit dataChanged(index, index);
                return true;
            }

            return false;
        };

        virtual Qt::ItemFlags flags(const QModelIndex &index) const
        {
            Qt::ItemFlags result = QAbstractItemModel::flags(index);

            if (COL_NAME == index.column()) {
                result |= Qt::ItemIsEditable;
            } else if (COL_HIDDEN == index.column())      {
                result |= Qt::ItemIsUserCheckable;
            }

            return result;
        };

        virtual QVariant data(const QModelIndex &index, int role) const
        {
            if (!index.isValid()) {
                return QVariant();
            }

            if (index.row() >= rowCount(index)) {
                return QVariant();
            }

            switch (role) {
                case Qt::DisplayRole:
                    switch (index.column()) {
                        case COL_NAME:
                            return m_data->at(index.row()).name();

                        case COL_START:
                            return KMF::Time(m_data->at(index.row()).start()).toString();

                        case COL_LENGTH:
                            if (index.row() == m_data->count() - 1) {
                                KMF::Time t(m_total);
                                t -= m_data->at(index.row()).start();
                                return t.toString();
                            } else   {
                                return KMF::Time(m_data->at(index.row()).length()).toString();
                            }

                        case COL_THUMBNAIL:
                            return m_data->at(index.row()).isHidden()
                                    ? i18n("Not Applicable")
                                    : m_data->at(index.row()).previewFile().isEmpty() 
                                        ? i18n("Generated") : getFile(m_data->at(index.row()).previewFile());
                        default:
                            break;
                    }
                    break;

                case Qt::ToolTipRole:
                    if (COL_THUMBNAIL == index.column() && 
                        !m_data->at(index.row()).previewFile().isEmpty() &&  !m_data->at(index.row()).isHidden()) {
                            return m_data->at(index.row()).previewFile();
                    }
                    break;

                case Qt::FontRole:
                    if (COL_THUMBNAIL == index.column() && 
                        (m_data->at(index.row()).previewFile().isEmpty() ||  m_data->at(index.row()).isHidden())) {
                        QFont font;
                        font.setItalic(true);
                        return font;
                    }
                    break;

                case Qt::CheckStateRole:
                    if (COL_HIDDEN == index.column()) {
                        return m_data->at(index.row()).isHidden() ? Qt::Unchecked : Qt::Checked;
                    }
                    break;

                case Qt::EditRole:
                    if (COL_NAME == index.column()) {
                        return m_data->at(index.row()).name();
                    }
                    break;

                default:
                    break;
            }

            return QVariant();
        };

        virtual QVariant headerData(int column, Qt::Orientation, int role) const
        {
            if (role == Qt::DisplayRole) {
                switch (column) {
                    case COL_NAME:
                        return i18n("Chapter Name");

                    case COL_START:
                        return i18n("Start");

                    case COL_LENGTH:
                        return i18n("Length");
                        
                    case COL_THUMBNAIL:
                        return i18n("Thumbnail");
                }
            } else if (role == Qt::DecorationRole) {
                switch (column) {
                    case COL_HIDDEN:
                        return KIcon("layer-visible-on");
                }
            } else if (role == Qt::ToolTipRole) {
                switch (column) {
                    case COL_HIDDEN:
                        return i18nc("Column header for chapter visibility", "Visible");
                }
            }
            return QVariant();
        };

    private:
        QDVD::CellList *m_data;
        QTime m_total;
};

class AddChapter : public KDialog, public Ui::AddChapter
{
    public:
        AddChapter(QWidget *parent) : KDialog(parent)
        {
            setupUi(mainWidget());
            setButtons(KDialog::Ok | KDialog::Cancel);
            setCaption(i18n("Add Chapter"));
        };
};

class AutoChapters : public KDialog, public Ui::AutoChapters
{
    public:
        AutoChapters(QWidget *parent) : KDialog(parent)
        {
            setupUi(mainWidget());
            setButtons(KDialog::Ok | KDialog::Cancel);
        };
};

Chapters::Chapters(QWidget *parent)
    : QWidget(parent)
    , m_obj(0)
    , m_model(0)
{
    setupUi(this);
    chaptersView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(startButton, SIGNAL(clicked()), this, SLOT(slotStart()));
    connect(endButton, SIGNAL(clicked()), this, SLOT(slotEnd()));
    connect(addButton, SIGNAL(clicked()), this, SLOT(slotAdd()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(slotRemove()));
    connect(timeSlider, SIGNAL(sliderMoved(int)),
            this, SLOT(slotSliderMoved(int)));
    connect(fwdButton, SIGNAL(clicked()), this, SLOT(slotForward()));
    connect(rewButton, SIGNAL(clicked()), this, SLOT(slotRewind()));
    connect(nextButton, SIGNAL(clicked()), this, SLOT(slotForwardShort()));
    connect(prevButton, SIGNAL(clicked()), this, SLOT(slotRewindShort()));
    connect(playButton, SIGNAL(clicked()), this, SLOT(slotPlay()));
    connect(chaptersView, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(slotContextMenu(const QPoint &)));
    connect(chaptersView, SIGNAL(doubleClicked(const QModelIndex &)),
            this, SLOT(chapterDoubleClicked(const QModelIndex &)));
    connect(customPreviewButton, SIGNAL(clicked()),
            this, SLOT(saveCustomPreview()));
    startButton->setIcon(KIcon("media-skip-backward"));
    rewButton->setIcon(KIcon("media-seek-backward"));
    prevButton->setIcon(KIcon("arrow-left"));
    playButton->setIcon(KIcon("media-playback-start"));
    nextButton->setIcon(KIcon("arrow-right"));
    fwdButton->setIcon(KIcon("media-seek-forward"));
    endButton->setIcon(KIcon("media-skip-forward"));
    addButton->setIcon(KIcon("list-add"));
    removeButton->setIcon(KIcon("list-remove"));
}

Chapters::~Chapters()
{
    KConfigGroup cg = KGlobal::config()->group("ChaptersDlg");
    cg.writeEntry("splitter", splitter->sizes());
}

void Chapters::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    chaptersView->header()->setResizeMode(COL_NAME, QHeaderView::ResizeToContents);
    chaptersView->header()->setResizeMode(COL_START, QHeaderView::ResizeToContents);
    chaptersView->header()->setResizeMode(COL_LENGTH, QHeaderView::ResizeToContents);
    chaptersView->header()->setResizeMode(COL_HIDDEN, QHeaderView::ResizeToContents);
    chaptersView->header()->setResizeMode(COL_THUMBNAIL, QHeaderView::ResizeToContents);
    chaptersView->header()->setStretchLastSection(true);
    KConfigGroup cg = KGlobal::config()->group("ChaptersDlg");
    splitter->setSizes(cg.readEntry("splitter", QList<int>() << 330 << 330));
}

void Chapters::setData(const QDVD::CellList &cells, const VideoObject *obj, VideoOptions *vidOpt)
{
    m_cells = cells;
    m_obj = obj;
    m_vidOpt = vidOpt;

    if (m_model) {
        delete m_model;
    }

    m_model = new CellListModel(&m_cells, m_obj->duration());
    chaptersView->setModel(m_model);
    timeSlider->setMaximum((int)KMF::Time(m_obj->duration()));
    m_duration = KMF::Time(m_obj->duration()).toString();
    m_pos = 0.0;
    chaptersView->setCurrentIndex(m_model->index(0));
    m_lastFile.clear();
    updateVideo();
    connect(chaptersView->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(slotSelectionChanged()));
}

void Chapters::updateVideo()
{
    KMF::Time t(m_pos);
    QString file = m_obj->videoFileName(&t);

    if (video->isPlaying()) {
        slotPlay();
    }

    if (file != m_lastFile) {
        // video->load(file);
        video->play(file);
        video->pause();
        video->mediaObject()->setTickInterval(25);
        connect(video->mediaObject(), SIGNAL(tick(qint64)),
                this, SLOT(slotTick(qint64)));
        connect(video->mediaObject(), SIGNAL(totalTimeChanged(qint64)),
                this, SLOT(slotTotalTime(qint64)));
        m_lastFile = file;
        m_difference = m_pos - t;
        m_msecPhononRatio = 1.0;
    }

    seekVideo(t.toMSec());
    setSliderAndTime(t.toMSec());
}

void Chapters::seekVideo(int msec)
{
    // kDebug() << (qint64)((qreal)msec / m_msecPhononRatio);
    video->seek((qint64)((qreal)msec / m_msecPhononRatio));
}

void Chapters::setSliderAndTime(int msec)
{
    m_pos = KMF::Time(msec) + m_difference;
    QString s = QString("%1: %2 / %3").arg(m_obj->text()).arg(m_pos.toString()).arg(m_duration);
    timeLabel->setText(s);
    timeSlider->setValue((int)m_pos);
}

void Chapters::slotTotalTime(qint64 totalTime)
{
    const KMFMediaFile &media = KMFMediaFile::mediaFile(m_lastFile);
    int duration = KMF::Time(media.duration()).toMSec();

    m_msecPhononRatio = (qreal)duration / (qreal)totalTime;
    // kDebug() << m_msecPhononRatio << duration << totalTime;
    slotTick(video->currentTime());
}

void Chapters::slotTick(qint64 time)
{
    // kDebug() << time;
    setSliderAndTime((int)(m_msecPhononRatio * time));
}

void Chapters::slotSliderMoved(int value)
{
    m_pos = value;
    updateVideo();
}

void Chapters::moveMSecs(int direction)
{
    m_pos += direction;

    if (m_pos < KMF::Time(0.0)) {
        m_pos = 0.0;
    } else if (m_pos > m_obj->duration()) {
        m_pos = m_obj->duration();
    }

    updateVideo();
}

void Chapters::slotForwardShort()
{
    moveMSecs(500);
}

void Chapters::slotRewindShort()
{
    moveMSecs(-500);
}

void Chapters::slotForward()
{
    moveMSecs(30000);
}

void Chapters::slotRewind()
{
    moveMSecs(-30000);
}

void Chapters::slotStart()
{
    m_pos = 0.0;
    updateVideo();
}

void Chapters::slotEnd()
{
    m_pos = m_obj->duration();
    updateVideo();
}

void Chapters::slotSelectionChanged()
{
    int i = chaptersView->currentIndex().row();

    if (i >= 0) {
        m_pos = m_cells.at(i).start();
        updateVideo();
    }
}

void Chapters::slotRemove()
{
    if (m_cells.count() > 0) {
        int i = chaptersView->currentIndex().row();
        m_cells.removeAt(i);

        if (i > 0) {
            --i;
        }
        chaptersView->setCurrentIndex(m_model->index(i));
        checkLengths();
    }
}

void Chapters::slotAdd()
{
    QPointer<AddChapter> dlg = new AddChapter(this);

    dlg->chapterTime->setMaximumTime(m_obj->duration());
    dlg->chapterTime->setTime(m_pos);

    if (dlg->exec() == QDialog::Accepted) {
        QTime pos = dlg->chapterTime->time();
        QString text = dlg->nameEdit->text();
        int i;

        // Don't lose milliseconds
        if ((pos.hour() == m_pos.hour()) && (pos.minute() == m_pos.minute()) &&
            (pos.second() == m_pos.second()))
        {
            pos = m_pos;
        }

        for (i = 0; i < m_cells.count(); ++i) {
            if (pos < m_cells[i].start()) {
                break;
            }
        }

        m_cells.insert(i, QDVD::Cell(pos, QTime(), text));
        checkLengths();
    }
    delete dlg;
}

void Chapters::slotContextMenu(const QPoint &p)
{
    QMenu *popup = new QMenu(this);

    popup->addAction(i18n("Delete All"), this, SLOT(deleteAll()));
    popup->addAction(i18n("Rename All"), this, SLOT(renameAll()));
    popup->addAction(i18n("Auto Chapters"), this, SLOT(autoChapters()));
    popup->addAction(i18nc("Import chapter file", "Import..."), this, SLOT(import()));
    
    popup->addSeparator();
    popupIndex=chaptersView->indexAt(p);
    QAction *act=popup->addAction(i18n("Set Custom Thumbnail..."), this, SLOT(setThumbnail()));
    if(!popupIndex.isValid()) {
        act->setEnabled(false);
    }
    act=popup->addAction(i18n("Unset Custom Thumbnail"), this, SLOT(unsetThumbnail()));
    if(!popupIndex.isValid() || m_cells[popupIndex.row()].previewFile().isEmpty()) {
        act->setEnabled(false);
    }
    popup->exec(chaptersView->viewport()->mapToGlobal(p));
}

void Chapters::renameAll()
{
    QPointer<AutoChapters> dlg = new AutoChapters(kapp->activeWindow());

    dlg->intervalLabel->hide();
    dlg->intervalTime->hide();
    dlg->resize(dlg->minimumSize());
    dlg->setCaption(i18n("Rename All"));

    if (dlg->exec()) {
        QString text = dlg->nameEdit->text().replace('#', "%1");

        for (int i = 0; i < m_cells.size(); ++i) {
            m_cells[i].setName(QString(text).arg(i));
        }

        chaptersView->viewport()->update();
    }
    delete dlg;
}

void Chapters::deleteAll()
{
    m_cells.clear();
    chaptersView->viewport()->update();
}

void Chapters::autoChapters()
{
    QPointer<AutoChapters> dlg = new AutoChapters(kapp->activeWindow());

    dlg->setCaption(i18n("Auto Chapters"));

    if (dlg->exec()) {
        QString text = dlg->nameEdit->text().replace('#', "%1");
        KMF::Time interval = dlg->intervalTime->time();
        KMF::Time time;
        int i = 1;

        if (interval.toMSec() < 1000) {
            return;
        }

        m_cells.clear();

        while (time < m_obj->duration()) {
            QString s;

            if (!text.isEmpty()) {
                s = QString(text).arg(i);
            } else {
                s = time.toString("h:mm:ss");
            }

            m_cells << QDVD::Cell(time, interval, s);
            kDebug() << m_cells.count();
            time += interval;
            ++i;
        }
        checkLengths();
    }
    delete dlg;
}

void Chapters::import()
{
    QString chapterFile = KFileDialog::getOpenFileName(
            KUrl("kfiledialog:///<Chapters>"), QString(), this);

    if (!chapterFile.isEmpty()) {
        QMap<QString, QString> chapters = KMF::Tools::readIniFile(chapterFile);
        m_cells.clear();

        for (int i = 1;; ++i) {
            QString number = QString::number(i).rightJustified(2, '0');
            QString entry = chapters.value(QString("CHAPTER%1").arg(number), "");
            kDebug() << chapters << entry;

            if (entry.isEmpty()) {
                break;
            }

            KMF::Time time(entry);
            QString s = chapters.value(QString("CHAPTER%1NAME").arg(number),
                    QString("%1").arg(i));
            m_cells.insert(i, QDVD::Cell(time, QTime(), s));
        }

        checkLengths();
    }
}

void Chapters::saveCustomPreview()
{
    int serial = m_obj->interface()->serial();
    QDir dir(m_obj->interface()->projectDir("media"));
    QString preview;

    preview.sprintf("%3.3d_preview.png", serial);
    preview = dir.filePath(preview);
    m_obj->getFrame(m_pos).save(preview);
    m_vidOpt->setPreviewUrl(preview);
}

void Chapters::checkLengths()
{
    if (m_cells.count() == 0) {
        return;
    }

    for (int i = 0; i < m_cells.count() - 1; ++i) {
        KMF::Time next = m_cells[i + 1].start();
        m_cells[i].setLength(next - m_cells[i].start());
    }

    m_cells.last().setLength(QTime(0, 0));
    m_model->update();
}

bool Chapters::ok()
{
    if (m_cells.count() > 0) {
        int nonHidden(0);

        for (int i = 0; i < m_cells.count(); ++i) {
            if (!m_cells[i].isHidden()) {
                nonHidden++;
            }
        }

        if (0 == nonHidden) {
            KMessageBox::sorry(this,
                    i18n("You should have at least one non-hidden chapter."),
                    i18n("No Chapters"));
        } else if (m_cells.count() > 99) {
            KMessageBox::sorry(this,
                    i18n("Each title in a DVD can have a maximum of 99 chapters.\n"
                         "This title currently has %1.", m_cells.count()),
                    i18n("Too Many Chapters"));
        } else {
            return true;
        }
    } else {
        KMessageBox::sorry(this,
                i18n("You should have at least one chapter."),
                i18n("No Chapters"));
    }

    return false;
}

void Chapters::slotPlay()
{
    if (video->isPlaying()) {
        video->pause();
        playButton->setIcon(KIcon("media-playback-start"));
    } else {
        video->play();
        playButton->setIcon(KIcon("media-playback-pause"));
    }
}

void Chapters::setThumbnail()
{
    if(popupIndex.isValid()) {
        setThumbnail(popupIndex.row());
    }
}

void Chapters::unsetThumbnail()
{
    if(popupIndex.isValid()) {
        m_cells[popupIndex.row()].setPreviewFile(QString());
    }
}

void Chapters::chapterDoubleClicked(const QModelIndex &index)
{
    if(index.isValid() && COL_THUMBNAIL==index.column()) {
        setThumbnail(index.row());
    }
}

void Chapters::setThumbnail(int index)
{
    QString existing = m_cells[index].previewFile(),
            fileName = KFileDialog::getOpenFileName(KUrl(existing.isEmpty() ? "kfiledialog:///<Thumbnails>" : existing), 
                                                    "image/jpeg image/png", this);

    if (!fileName.isEmpty()) {
        QImage img(fileName);
        
        if(img.isNull()) {
            KMessageBox::error(this, i18n("%1 is not a vlid image file", fileName));
        }
        else {
            m_cells[index].setPreviewFile(fileName);
        }
    }
}

#include "chapters.moc"
