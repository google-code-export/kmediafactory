//**************************************************************************
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
//**************************************************************************

#include "chapters.h"
#include "videoplugin.h"
#include "kmftime.h"
#include "kmftools.h"
#include "ui_autochapters.h"
#include "ui_addchapter.h"
#include <qdvdinfo.h>
#include <kmfimageview.h>
#include <kmediafactory/plugin.h>
#include <Phonon/MediaObject>
#include <KApplication>
#include <KFileDialog>
#include <KMainWindow>
#include <KStandardAction>
#include <kinputdialog.h>
#include <KLocale>
#include <KMessageBox>
#include <KIcon>
#include <KDebug>
#include <QMenu>
#include <QSlider>
#include <QImage>
#include <QLabel>
#include <QDir>
#include <QLineEdit>

class CellListModel : public QAbstractListModel
{
  public:
    CellListModel(QDVD::CellList *data, QTime total) :
      m_data(data), m_total(total) {};

    virtual int rowCount(const QModelIndex&) const
    {
      return m_data->count();
    };

    virtual int columnCount(const QModelIndex&) const
    {
      return 3;
    };

    void update()
    {
      reset();
    };

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role)
    {
      if (!index.isValid())
        return false;

      if (index.row() >= rowCount(index))
        return false;
      
      if (role == Qt::EditRole && index.column() == 0)
      {
         (*m_data)[index.row()].setName(value.toString());
         emit dataChanged(index, index);
         return true;
      }
      return false;
    };
    
    virtual Qt::ItemFlags flags(const QModelIndex &index) const
    {
        Qt::ItemFlags result = QAbstractItemModel::flags(index);
        if (index.column() == 0) {
            result |= Qt::ItemIsEditable;
        }
        return result;
    };
    
    virtual QVariant data(const QModelIndex &index, int role) const
    {
      if (!index.isValid())
        return QVariant();

      if (index.row() >= rowCount(index))
        return QVariant();

      if (role == Qt::DisplayRole)
      {
        switch(index.column())
        {
          case 0:
            return m_data->at(index.row()).name();
          case 1:
            return KMF::Time(m_data->at(index.row()).start()).toString();
          case 2:
            if(index.row() == m_data->count() - 1)
            {
              KMF::Time t(m_total);
              t -= m_data->at(index.row()).start();
              return t.toString();
            }
            else
              return KMF::Time(m_data->at(index.row()).length()).toString();
        }
      }
      return QVariant();
    };

    virtual QVariant headerData(int column, Qt::Orientation, int role) const
    {
      if (role != Qt::DisplayRole)
        return QVariant();

      switch(column)
      {
        case 0:
          return i18n("Name");
        case 1:
          return i18n("Start");
        case 2:
          return i18n("Length");
      }
      return "";
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
      setCaption(i18n("Add chapters"));
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
  : KDialog(parent), m_obj(0), m_model(0)
{
  setupUi(mainWidget());
  setButtons(KDialog::Ok | KDialog::Cancel);
  setCaption(i18n("Chapters"));
  chaptersView->setContextMenuPolicy(Qt::CustomContextMenu);

  connect(startButton, SIGNAL(clicked()), this, SLOT(slotStart()));
  connect(endButton, SIGNAL(clicked()), this, SLOT(slotEnd()));
  connect(addButton, SIGNAL(clicked()), this, SLOT(slotAdd()));
  connect(removeButton, SIGNAL(clicked()), this, SLOT(slotRemove()));
  connect(timeSlider, SIGNAL(sliderMoved(int)),
          this, SLOT(slotSliderMoved(int)));
  connect(fwdButton, SIGNAL(clicked()), this, SLOT(slotForward()));
  connect(rewButton, SIGNAL(clicked()), this, SLOT(slotRewind()));
  connect(nextButton, SIGNAL(clicked()), this, SLOT(slotNextFrame()));
  connect(prevButton, SIGNAL(clicked()), this, SLOT(slotPrevFrame()));
  connect(playButton, SIGNAL(clicked()), this, SLOT(slotPlay()));
  connect(chaptersView, SIGNAL(customContextMenuRequested(const QPoint&)),
          this, SLOT(slotContextMenu(const QPoint&)));
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
}

void Chapters::getData(QDVD::CellList& cells, QString* preview) const
{
  cells = m_cells;
  *preview = m_preview;
  delete m_model;
}

void Chapters::setData(const QDVD::CellList& cells,
                       const VideoObject* obj)
{
  m_cells = cells;
  m_obj = obj;
  m_model = new CellListModel(&m_cells, m_obj->duration());
  chaptersView->setModel(m_model);
  timeSlider->setMaximum((int)KMF::Time(m_obj->duration()));
  m_duration = KMF::Time(m_obj->duration()).toString();
  m_pos = 0.0;
  chaptersView->setCurrentIndex(m_model->index(0));
  m_lastFile.clear();
  updateVideo();
  connect(chaptersView->selectionModel(),
      SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
      this, SLOT(slotSelectionChanged()));
}

void Chapters::updateVideo()
{
  KMF::Time t(m_pos);
  QString file = m_obj->videoFileName(&t);
  
  if (file != m_lastFile) {
      video->play(file);
      video->mediaObject()->setTickInterval(10);
      connect(video->mediaObject(), SIGNAL(tick(qint64)),
              this, SLOT(slotTick(qint64)));
      m_lastFile = file;
      m_difference = m_pos - t;
  }
  video->pause();
  video->seek(t.toMSec());
  slotTick(t.toMSec());
}

void Chapters::slotSliderMoved(int value)
{
  m_pos = value;
  updateVideo();
}

void Chapters::moveFrames(int direction)
{
  double move = (double)((double)direction * (1.0 / m_obj->frameRate()));
  m_pos += move;
  if(m_pos < KMF::Time(0.0))
    m_pos = 0.0;
  else if(m_pos > m_obj->duration())
    m_pos = m_obj->duration();
  updateVideo();
}

void Chapters::slotNextFrame()
{
  moveFrames(1);
}

void Chapters::slotPrevFrame()
{
  moveFrames(-1);
}

void Chapters::slotForward()
{
  moveFrames(120);
}

void Chapters::slotRewind()
{
  moveFrames(-120);
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
  if(i >= 0)
  {
    m_pos = m_cells.at(i).start();
    updateVideo();
  }
}

void Chapters::slotRemove()
{
  if(m_cells.count() > 0)
  {
    int i = chaptersView->currentIndex().row();
    m_cells.removeAt(i);
    if(i > 0)
      --i;
    chaptersView->setCurrentIndex(m_model->index(i));
    checkLengths();
  }
}

void Chapters::slotAdd()
{
  AddChapter dlg(this);

  dlg.chapterTime->setMaximumTime(m_obj->duration());
  dlg.chapterTime->setTime(m_pos);
  if(dlg.exec() == QDialog::Accepted)
  {
    QTime pos = dlg.chapterTime->time();
    QString text = dlg.nameEdit->text();
    int i;

    // Don't lose milliseconds
    if(pos.hour() == m_pos.hour() && pos.minute() == m_pos.minute() &&
       pos.second() == m_pos.second())
    {
      pos = m_pos;
    }
    for(i = 0; i < m_cells.count(); ++i)
    {
      if(pos < m_cells[i].start())
        break;
    }
    m_cells.insert(i, QDVD::Cell(pos, QTime(), text));
    checkLengths();
  }
}

void Chapters::slotContextMenu(const QPoint& p)
{
  QMenu *popup = new QMenu( this );
  popup->addAction(i18n("&Delete all"), this, SLOT(deleteAll()));
  popup->addAction(i18n("&Rename all"), this, SLOT(renameAll()));
  popup->addAction(i18n("&Auto chapters"), this, SLOT(autoChapters()));
  popup->addAction(i18nc("Import chapter file", "&Import"),
                   this, SLOT(import()));
  popup->exec(chaptersView->viewport()->mapToGlobal(p));
}

void Chapters::renameAll()
{
  AutoChapters dlg(kapp->activeWindow());
  dlg.intervalLabel->hide();
  dlg.intervalTime->hide();
  dlg.resize(dlg.minimumSize());
  dlg.setCaption(i18n("Rename All"));
  if (dlg.exec())
  {
    QString text = dlg.nameEdit->text().replace("#", "%1");
    for(int i = 0; i < m_cells.size(); ++i)
    {
      m_cells[i].setName(QString(text).arg(i));
    }
    chaptersView->viewport()->update();
  }
}

void Chapters::deleteAll()
{
  m_cells.clear();
  chaptersView->viewport()->update();
}

void Chapters::autoChapters()
{
  AutoChapters dlg(kapp->activeWindow());
  dlg.setCaption(i18n("Auto chapters"));
  if (dlg.exec())
  {
    QString text = dlg.nameEdit->text().replace("#", "%1");
    KMF::Time interval = dlg.intervalTime->time();
    KMF::Time time;
    int i = 1;

    if(interval.toMSec() < 1000)
      return;
    m_cells.clear();
    while(time < m_obj->duration())
    {
      QString s;

      if(!text.isEmpty())
        s = QString(text).arg(i);
      else
        s = time.toString("h:mm:ss");
      m_cells << QDVD::Cell(time, interval, s);
      kDebug() << m_cells.count();
      time += interval;
      ++i;
    }
    checkLengths();
  }
}

void Chapters::import()
{
  QString chapterFile = KFileDialog::getOpenFileName(
      KUrl("kfiledialog:///<Chapters"), "*.*|All files");

  if(!chapterFile.isEmpty())
  {
    QMap<QString, QString> chapters = KMF::Tools::readIniFile(chapterFile);
    m_cells.clear();
    for(int i = 1;;++i)
    {
      QString number = QString::number(i).rightJustified(2, '0');
      QString entry = chapters.value(QString("CHAPTER%1").arg(number), "");
      kDebug() << chapters << entry;
      if(entry.isEmpty())
        break;
      KMF::Time time(entry);
      QString s = chapters.value(QString("CHAPTER%1NAME").arg(number),
                                 QString("%1").arg(i));
      m_cells.insert(i, QDVD::Cell(time, QTime(), s));
    }
    checkLengths();
  }
}

void Chapters::saveCustomPreview( )
{
  int serial = m_obj->interface()->serial();
  QDir dir(m_obj->interface()->projectDir("media"));
  m_preview.sprintf("%3.3d_preview.png", serial);
  m_preview = dir.filePath(m_preview);
  m_obj->getFrame(m_pos, m_preview);
}

void Chapters::checkLengths()
{
  if(m_cells.count() == 0)
    return;
  for(int i=0; i < m_cells.count() -1; ++i)
  {
    KMF::Time next = m_cells[i+1].start();
    m_cells[i].setLength(next - m_cells[i].start());
  }
  m_cells.last().setLength(QTime(0, 0));
  m_model->update();;
}

void Chapters::accept()
{
  if(m_cells.count() > 0)
    KDialog::accept();
  else
  {
    KMessageBox::sorry(this,
                       i18n("You should have atleast one chapter."));
  }
}

void Chapters::slotPlay()
{
    if (video->isPlaying()) {
        video->pause();
        playButton->setIcon(KIcon("media-playback-start"));
    }
    else
    {
        video->play();
        playButton->setIcon(KIcon("media-playback-pause"));
    }
}

void Chapters::slotTick(qint64 time)
{
  m_pos = KMF::Time((int)time) + m_difference;
  QString s = QString("%1: %2 / %3").
      arg(m_obj->text()).
      arg(m_pos.toString()).
      arg(m_duration);
  timeLabel->setText(s);
  timeSlider->setValue((int)m_pos);
}

#include "chapters.moc"
