/***************************************************************************
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "dvdinfo.h"

#ifdef HAVE_LIBDVDREAD

#include <klistview.h>
#include <krun.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kurlrequester.h>
#include <kiconloader.h>
#include <kprogress.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <qpainter.h>
#include <qslider.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <q3textbrowser.h>

DVDItem::DVDItem(Q3ListView* parent, const QDVD::Base* data) :
    Q3ListViewItem(parent), m_data(data)
{
  initItem();
}

DVDItem::DVDItem(Q3ListViewItem* parent, const QDVD::Base* data) :
    Q3ListViewItem(parent), m_data(data)
{
  initItem();
}

DVDItem::DVDItem(Q3ListViewItem* parent, Q3ListViewItem* after,
                 const QDVD::Base* data) :
    Q3ListViewItem(parent, after), m_data(data)
{
  initItem();
}

void DVDItem::initItem()
{
  QString icon;

  if(m_data->rtti() == QDVD::Base::INFO)
    icon = "dvd_unmount";
  else if(m_data->rtti() == QDVD::Base::TITLE)
    icon = "background";
  else if(m_data->rtti() == QDVD::Base::VIDEO)
    icon = "video";
  else if(m_data->rtti() == QDVD::Base::CELL)
    icon = "man";
  else if(m_data->rtti() == QDVD::Base::AUDIO)
    icon = "sound";
  else if(m_data->rtti() == QDVD::Base::SUBTITLE)
    icon = "font";

  setPixmap(0, KGlobal::iconLoader()->loadIcon(icon, KIcon::Small));
}

DVDItem::~DVDItem()
{
  //delete m_data;
}

QString DVDItem::text(int column) const
{
  switch(column)
  {
    case 0:
      return m_data->toString();
    case 1:
      return QString("%1 MB").arg(m_data->size() / (1024 * 1024));
    default:
      return "";
  }
}

DVDInfo::DVDInfo(QWidget *parent, const char *name, QString device)
 : DVDInfoLayout(parent, name)
{
  dvdListView->setColumnWidthMode(0, Q3ListView::Maximum);
  dvdListView->setAllColumnsShowFocus(true);
  dvdListView->setSorting(-1);

  textBrowser->setWordWrap(Q3TextEdit::NoWrap);

  url->setURL(device);
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
           dlg.progressBar(), SLOT(setTotalSteps(int)));
  connect(&m_info, SIGNAL(title(int)),
           dlg.progressBar(), SLOT(setValue(int)));
  dlg.setLabel(i18n("Analyzing DVD..."));
  dlg.show();
  kapp->processEvents();
  m_info.parseDVD(url->url());
  dlg.hide();
}

bool DVDInfo::isDVD()
{
  QFileInfo fi(url->url());
  bool dvd = false;

  if(fi.isDir())
  {
    QDir dir(fi.filePath() + "/VIDEO_TS");
    if(dir.exists())
      dvd = true;
  }
  else if(fi.filePath().startsWith("/dev/"))
  {
    dvd = true;
  }
  else if(fi.extension().lower() == "iso")
  {
    dvd = true;
  }
  return dvd;
}

void DVDInfo::itemChanged(Q3ListViewItem* item)
{
  if(!item)
    return;
  QString icon;
  const QDVD::Base* base = static_cast<const DVDItem*>(item)->data();
  QString text = "<table cellspacing=\"1\">\n";
  QString line = "<tr><td><b>%1:  </b></td><td>%2</td></tr>\n";

  if(base->rtti() == QDVD::Base::INFO)
  {
    const QDVD::Info* info = static_cast<const QDVD::Info*>(base);
    text += line.arg(i18n("DVD")).arg(info->device());
    text += line.arg(i18n("Titles")).arg(info->titles().count());
    text += line.arg(i18n("Longest title")).arg(QString("%1. %2")
            .arg(info->longestTitle()+1)
            .arg(info->longestTitleLength().toString()));
    text += line.arg(i18n("Name")).arg(info->title());
    text += line.arg(i18n("VMG Identifier")).arg(info->vmgIdentifier());
    text += line.arg(i18n("Provider indentifier"))
        .arg(info->providerIdentifier());
  }
  else if(base->rtti() == QDVD::Base::TITLE)
  {
    const QDVD::Title* title = static_cast<const QDVD::Title*>(base);
    text += line.arg(i18n("Index")).arg(title->titleNbr());
    text += line.arg(i18n("Length")).arg(title->length().toString());
    text += line.arg(i18n("VTS ID")).arg(title->vtsId());
    text += line.arg(i18n("VTS")).arg(title->vts());
    text += line.arg(i18n("TTN")).arg(title->ttn());
    text += line.arg(i18n("PGC")).arg(title->pgcNbr());
    text += line.arg(i18n("Start sector")).arg(title->startSector());
    text += line.arg(i18n("End sector")).arg(title->endSector());
  }
  else if(base->rtti() == QDVD::Base::VIDEO)
  {
    const QDVD::VideoTrack* video = static_cast<const QDVD::VideoTrack*>(base);
    text += line.arg(i18n("Width")).arg(video->width());
    text += line.arg(i18n("Height")).arg(video->height());
    text += line.arg(i18n("Format")).arg(video->formatString());
    text += line.arg(i18n("Aspect ratio")).arg(video->aspectRatioString());
    text += line.arg(i18n("Frames per second")).arg(video->fps());
    text += line.arg(i18n("Permitted DF")).arg(video->permittedDfString());
  }
  else if(base->rtti() == QDVD::Base::CELL)
  {
    const QDVD::Cell* cell = static_cast<const QDVD::Cell*>(base);
    text += line.arg(i18n("Index")).arg(cell->cellNbr());
    text += line.arg(i18n("Start")).arg(cell->start().toString());
    text += line.arg(i18n("Length")).arg(cell->length().toString());
    text += line.arg(i18n("Start sector")).arg(cell->startSector());
    text += line.arg(i18n("End sector")).arg(cell->endSector());
    text += line.arg(i18n("Is chapter"))
        .arg(cell->isChapter()?i18n("yes"):i18n("no"));
  }
  else if(base->rtti() == QDVD::Base::AUDIO)
  {
    const QDVD::AudioTrack* audio = static_cast<const QDVD::AudioTrack*>(base);
    text += line.arg(i18n("Language")).arg(audio->languageString());
    text += line.arg(i18n("Format")).arg(audio->formatString());
    text += line.arg(i18n("Channels")).arg(audio->channels());
    text += line.arg(i18n("Sample frequency")).arg(audio->sampleFreq());
    text += line.arg(i18n("Bitrate")).arg(audio->bitrate());
    text += line.arg(i18n("Quantization")).arg(audio->quantizationString());
    text += line.arg(i18n("AP Mode")).arg(audio->apMode());
    text += line.arg(i18n("Type")).arg(audio->typeString());
  }
  else if(base->rtti() == QDVD::Base::SUBTITLE)
  {
    const QDVD::Subtitle* sub = static_cast<const QDVD::Subtitle*>(base);
    text += line.arg(i18n("Language")).arg(sub->languageString());
    text += line.arg(i18n("Type")).arg(sub->typeString());
  }
  text += "</table>";
  textBrowser->setText(text);
}

void DVDInfo::open()
{
  if(!isDVD())
  {
    KMessageBox::error(this, i18n("Not a DVD."));
    return;
  }
  analyze();
  dvdListView->clear();

  const QDVD::TitleList& titles = m_info.titles();
  DVDItem* dvd = new DVDItem(dvdListView, &m_info);
  DVDItem* title = 0;
  DVDItem* track = 0;
  DVDItem* video = 0;
  dvd->setOpen(true);

  for(QDVD::TitleList::ConstIterator it = titles.begin();
      it != titles.end(); ++it)
  {
    title = new DVDItem(dvd, title, &(*it));
    title->setOpen(true);

    const QDVD::AudioList& audios = (*it).audioTracks();
    const QDVD::SubtitleList& subs = (*it).subtitles();
    const QDVD::CellList& cells = (*it).cells();

    video = new DVDItem(title, &(*it).videoTrack());

    track = 0;
    for(QDVD::CellList::ConstIterator jt = cells.begin();
        jt != cells.end(); ++jt)
    {
      track = new DVDItem(video, track, &(*jt));
    }

    track = video;
    for(QDVD::AudioList::ConstIterator jt = audios.begin();
        jt != audios.end(); ++jt)
    {
      track = new DVDItem(title, track, &(*jt));
    }

    for(QDVD::SubtitleList::ConstIterator jt = subs.begin();
        jt != subs.end(); ++jt)
    {
      track = new DVDItem(title, track, &(*jt));
    }
  }
  dvdListView->setSelected(dvd, true);
  itemChanged(dvd);
}

void DVDInfo::configureFileDialog(KURLRequester* url)
{
  url->fileDialog()->setMode(static_cast<KFile::Mode>(KFile::File |
      KFile::Directory | KFile::ExistingOnly |
      KFile::LocalOnly));
  url->fileDialog()->setFilter("*.mpg *.iso|" +
      i18n("DVD files and directories"));
}

#include "dvdinfo.moc"

#endif // HAVE_LIBDVDREAD
