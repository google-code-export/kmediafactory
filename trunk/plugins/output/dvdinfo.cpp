/***************************************************************************
*   Copyright (C) 2010 by Petri Damstén                                   *
*   petri.damsten@iki.fi                                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
***************************************************************************/

#include "dvdinfo.h"

#ifdef HAVE_LIBDVDREAD

#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QPainter>
#include <QtGui/QSlider>

#include <KApplication>
#include <KConfig>
#include <KDebug>
#include <KFileDialog>
#include <KGlobal>
#include <KIconLoader>
#include <KMessageBox>
#include <KProgressDialog>
#include <KRun>
#include <KUrlRequester>

DVDInfo::DVDInfo(QWidget *parent, QString device)
    : KDialog(parent)
    , m_info()
    , m_model(&m_info)
{
    setupUi(mainWidget());
    setButtons(KDialog::Close);
    setCaption(i18n("DVD Info"));

    dvdListView->setModel(&m_model);
    connect(dvdListView->selectionModel(),
            SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(currentChanged(const QModelIndex &, const QModelIndex &)));
    connect(url, SIGNAL(openFileDialog(KUrlRequester *)),
            this, SLOT(configureFileDialog(KUrlRequester *)));
    connect(url, SIGNAL(urlSelected(const KUrl &)), this, SLOT(open()));
    url->setUrl(device);
    open();
}

DVDInfo::~DVDInfo()
{
}

void DVDInfo::analyze()
{
    KProgressDialog dlg(this);

    dlg.setMinimumDuration(0);
    connect(&m_info, SIGNAL(titles(int)),
            dlg.progressBar(), SLOT(setMaximum(int)));
    connect(&m_info, SIGNAL(title(int)),
            dlg.progressBar(), SLOT(setValue(int)));
    dlg.setLabelText(i18n("Analyzing DVD..."));
    dlg.show();
    kapp->processEvents();
    m_info.parseDVD(url->url().path());
    dlg.hide();
}

bool DVDInfo::isDVD()
{
    QFileInfo fi(url->url().path());
    bool dvd = false;

    if (fi.isDir()) {
        QDir dir(fi.filePath() + "/VIDEO_TS");

        if (dir.exists()) {
            dvd = true;
        }
    } else if (fi.filePath().startsWith("/dev/"))    {
        dvd = true;
    } else if (fi.suffix().toLower() == "iso")    {
        dvd = true;
    }

    return dvd;
}

void DVDInfo::currentChanged(const QModelIndex &current, const QModelIndex &)
{
    if (!current.isValid()) {
        return;
    }

    QStandardItem *i = m_model.itemFromIndex(current);
    const QDVD::Base *base = i->data().value<const QDVD::Base *>();
    QString icon;
    QString text = "<table cellspacing=\"1\">\n";
    QString line = "<tr><td><b>%1:  </b></td><td>%2</td></tr>\n";

    if (base->rtti() == QDVD::Base::INFO) {
        const QDVD::Info *info = static_cast<const QDVD::Info *>(base);
        text += line.arg(i18n("DVD")).arg(info->device());
        text += line.arg(i18n("Titles")).arg(info->titles().count());
        text += line.arg(i18n("Longest title")).arg(QString("%1. %2")
                .arg(info->longestTitle() + 1)
                .arg(info->longestTitleLength().toString()));
        text += line.arg(i18n("Name")).arg(info->title());
        text += line.arg(i18n("VMG Identifier")).arg(info->vmgIdentifier());
        text += line.arg(i18n("Provider indentifier"))
                .arg(info->providerIdentifier());
    } else if (base->rtti() == QDVD::Base::TITLE)    {
        const QDVD::Title *title = static_cast<const QDVD::Title *>(base);
        text += line.arg(i18n("Index")).arg(title->titleNbr());
        text += line.arg(i18n("Length")).arg(title->length().toString());
        text += line.arg(i18n("VTS ID")).arg(title->vtsId());
        text += line.arg(i18n("VTS")).arg(title->vts());
        text += line.arg(i18n("TTN")).arg(title->ttn());
        text += line.arg(i18n("PGC")).arg(title->pgcNbr());
        text += line.arg(i18n("Start sector")).arg(title->startSector());
        text += line.arg(i18n("End sector")).arg(title->endSector());
    } else if (base->rtti() == QDVD::Base::VIDEO)    {
        const QDVD::VideoTrack *video = static_cast<const QDVD::VideoTrack *>(base);
        text += line.arg(i18n("Width")).arg(video->width());
        text += line.arg(i18n("Height")).arg(video->height());
        text += line.arg(i18n("Format")).arg(video->formatString());
        text += line.arg(i18n("Aspect ratio")).arg(video->aspectRatioString());
        text += line.arg(i18n("Frames per second")).arg(video->fps());
        text += line.arg(i18n("Permitted DF")).arg(video->permittedDfString());
    } else if (base->rtti() == QDVD::Base::CELL)    {
        const QDVD::Cell *cell = static_cast<const QDVD::Cell *>(base);
        text += line.arg(i18n("Index")).arg(cell->cellNbr());
        text += line.arg(i18n("Start")).arg(cell->start().toString());
        text += line.arg(i18n("Length")).arg(cell->length().toString());
        text += line.arg(i18n("Start sector")).arg(cell->startSector());
        text += line.arg(i18n("End sector")).arg(cell->endSector());
        text += line.arg(i18n("Is chapter"))
                .arg(cell->isChapter() ? i18n("yes") : i18n("no"));
    } else if (base->rtti() == QDVD::Base::AUDIO)    {
        const QDVD::AudioTrack *audio = static_cast<const QDVD::AudioTrack *>(base);
        text += line.arg(i18n("Language")).arg(audio->languageString());
        text += line.arg(i18n("Format")).arg(audio->formatString());
        text += line.arg(i18n("Channels")).arg(audio->channels());
        text += line.arg(i18n("Sample frequency")).arg(audio->sampleFreq());
        text += line.arg(i18n("Bitrate")).arg(audio->bitrate());
        text += line.arg(i18n("Quantization")).arg(audio->quantizationString());
        text += line.arg(i18n("AP Mode")).arg(audio->apMode());
        text += line.arg(i18n("Type")).arg(audio->typeString());
    } else if (base->rtti() == QDVD::Base::SUBTITLE)    {
        const QDVD::Subtitle *sub = static_cast<const QDVD::Subtitle *>(base);
        text += line.arg(i18n("Language")).arg(sub->languageString());
        text += line.arg(i18n("Type")).arg(sub->typeString());
    }

    text += "</table>";
    textBrowser->setText(text);
}

QList<QStandardItem *> DVDInfo::list(const QDVD::Base *item)
{
    QList<QStandardItem *> list;

    QStandardItem *i1 = new QStandardItem(item->toString());
    QStandardItem *i2 = new QStandardItem(QString("%1 MB").arg(item->size() /
                    (1024 * 1024)));
    i1->setEditable(false);
    i2->setEditable(false);

    QString icon;

    if (item->rtti() == QDVD::Base::INFO) {
        icon = "dvd-unmount";
    } else if (item->rtti() == QDVD::Base::TITLE) {
        icon = "video-television";
    } else if (item->rtti() == QDVD::Base::VIDEO) {
        icon = "video";
    } else if (item->rtti() == QDVD::Base::CELL) {
        icon = "man";
    } else if (item->rtti() == QDVD::Base::AUDIO) {
        icon = "sound";
    } else if (item->rtti() == QDVD::Base::SUBTITLE) {
        icon = "font";
    }

    i1->setIcon(KIcon(icon));
    i2->setIcon(KIcon());
    i1->setData(QVariant::fromValue(item));
    list << i1 << i2;
    return list;
}

void DVDInfo::open()
{
    if (!isDVD()) {
        KMessageBox::error(this, i18n("Not a DVD."));
        return;
    }

    analyze();

    m_model.clear();
    m_model.setColumnCount(2);
    m_model.setHeaderData(0, Qt::Horizontal, i18n("Name"));
    m_model.setHeaderData(1, Qt::Horizontal, i18n("Size"));

    QList<QStandardItem *> dvd = list(&m_info);
    m_model.invisibleRootItem()->appendRow(dvd);
    dvdListView->setExpanded(dvd[0]->index(), true);

    for (int i = 0; i < m_info.titles().count(); ++i) {
        const QDVD::Title *title = &(m_info.titles().at(i));
        QList<QStandardItem *> titleItem = list(title);
        dvd[0]->appendRow(titleItem);
        dvdListView->setExpanded(titleItem[0]->index(), true);

        QList<QStandardItem *> video = list(&(title->videoTrack()));
        titleItem[0]->appendRow(video);

        for (int j = 0; j < title->cells().count(); ++j) {
            video[0]->appendRow(list(&(title->cells().at(j))));
        }

        for (int j = 0; j < title->audioTracks().count(); ++j) {
            titleItem[0]->appendRow(list(&(title->audioTracks().at(j))));
        }

        for (int j = 0; j < title->subtitles().count(); ++j) {
            titleItem[0]->appendRow(list(&(title->subtitles().at(j))));
        }
    }

    dvdListView->resizeColumnToContents(0);
    // dvdListView->setSelected(dvd, true);
}

void DVDInfo::configureFileDialog(KUrlRequester *Url)
{
    Url->fileDialog()->setMode(KFile::File |
            KFile::Directory | KFile::ExistingOnly |
            KFile::LocalOnly);
    Url->fileDialog()->setFilter("*.mpg *.iso|" +
            i18n("DVD files and directories"));
}

#include "dvdinfo.moc"

#endif // HAVE_LIBDVDREAD
