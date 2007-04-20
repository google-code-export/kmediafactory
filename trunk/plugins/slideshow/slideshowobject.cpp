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
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************
#include "slideshowobject.h"
#include "slideshowplugin.h"
#include "slideshowpluginsettings.h"
#include "slideshowproperties.h"
#include <qdvdinfo.h>
#include <kio/job.h>
#include <kfileitem.h>
#include <kstandarddirs.h>
#include <kfilemetainfo.h>
#include <kapplication.h>
#include <kactioncollection.h>
#include <kaboutdata.h>
#include <kmftools.h>
#include <kmftime.h>
#include <klocale.h>
#include <kmimetype.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kicon.h>
#include <kprogressdialog.h>
#include <kio/copyjob.h>
#include <QImage>
#include <QDir>
#include <QRegExp>
#include <QPixmap>
#include <QTextStream>
#include <list>
#include <kio/copyjob.h>

Slide::Slide() : chapter(true)
{
}

SlideshowObject::SlideshowObject(QObject* parent)
  : MediaObject(parent), m_loop(false), m_includeOriginals(true)
{
  setObjectName("slideshow");
  QAction* action = new KAction(KIcon("pencil"),
                                i18n("&Properties"),this);
  action->setShortcut(Qt::CTRL + Qt::Key_W);
  plugin()->actionCollection()->addAction("mob_properties", action);
  connect(action, SIGNAL(triggered()), SLOT(slotProperties()));
  m_duration = SlideshowPluginSettings::slideDuration();
}

SlideshowObject::~SlideshowObject()
{
}

bool SlideshowObject::oooConvert(QString* file) const
{
  KStandardDirs dirs;
  QStringList list = dirs.resourceDirs("lib");
  QString bin;
  bool result = false;
  QFileInfo fi(*file);
  QString output = QString("%1.pdf").arg(m_id);
  QDir dir(projectInterface()->projectDir("media"));
  QStringList oodirs;

  list << "/usr/lib" << "/opt";
  list += dirs.resourceDirs("lib");
  oodirs << "openoffice" << "openoffice2" << "openoffice.org2.0" <<
            "openoffice.org-2.0" << "ooo-1.9" << "ooo-2.0";

  output = dir.filePath(output);

  list += KMF::Tools::file2List("/etc/ld.so.conf", QString::null, "/");

  QStringList::ConstIterator it = oodirs.begin();
  while(bin.isEmpty() && it != list.end())
  {
    bin = KMF::Tools::findExe("soffice", list, (*it) + "/program");
    ++it;
  }

  if(!bin.isEmpty())
  {
    K3Process ooo;

    ooo << bin << "-invisible" << "-norestore" <<
        QString("macro:///KMediaFactory.converter.convertToPDF(%1,%2)")
        .arg(*file).arg(output);

    ooo.setWorkingDirectory(projectInterface()->projectDir("media"));
    uiInterface()->logger()->connectProcess(&ooo);
    ooo.start(K3Process::Block, K3Process::AllOutput);
    if(ooo.normalExit())
    {
      if(ooo.exitStatus() == 0)
        result = true;
    }
  }
  *file = output;
  return result;
}

SlideList SlideshowObject::slideList(QStringList list) const
{
  SlideList result;
  KProgressDialog dlg(kapp->activeWindow());
  int fileProgress = 10000 / list.count();

  dlg.progressBar()->setRange(0, 10000);
  dlg.setAutoClose(true);
  dlg.show();

  foreach(QString file, list)
  {
    KFileMetaInfo minfo(file, QString::null, KFileMetaInfo::ContentInfo);
    QString mime;
    QFileInfo fi(file);
    QDir dir(projectInterface()->projectDir("media"));
    KMimeType::Ptr type = KMimeType::findByUrl(file);

    dlg.setLabel(i18n("Processing slides...\n") + (file));
    kapp->processEvents();

    if(fi.isDir())
    {
      KMessageBox::error(kapp->activeWindow(),
                         i18n("Cannot add directory."));
      continue;
    }

    if(type)
      mime = type->name();
    //kdDebug() << k_funcinfo << mime << endl;
#warning TODO pdf conversion using command line tools
#if 0
    if(mime.startsWith("application/vnd.oasis.opendocument") ||
       mime.startsWith("application/vnd.sun.xml") ||
       mime == "application/msexcel" ||
       mime == "application/msword" ||
       mime == "application/mspowerpoint")
    {
      if(oooConvert(&file))
      {
        mime = "application/pdf";
        minfo = KFileMetaInfo(file, QString::null, KFileMetaInfo::ContentInfo);
      }
    }
    if(mime == "application/pdf")
    {
      QFileInfo fi(file);
      QString fileNameTemplate = m_id + "_%1.png";
      std::list<Magick::Image> imageList;
      int i = 0;
      bool landscape = true;
      QSize pdfRes(0, 0);
      int pageProgress = 0;

      if(minfo.contains("Page size"))
      {
        QStringList res = QStringList::split(" ",
                                             minfo.item("Page size").string());
        if(res.count() >= 3)
        {
          pdfRes = QSize(res[0].toInt(), res[2].toInt());
          //kdDebug() << k_funcinfo <<pdfRes << endl;
          if(pdfRes.width() < pdfRes.height())
            landscape = false;
        }
      }
      try
      {
        readImages(&imageList, (const char*)file.local8Bit());
      }
      catch(...)
      {
        kdDebug() << "Caught magic exception." << endl;
      }

      if(imageList.size() > 0)
        pageProgress = fileProgress / imageList.size();

      while(imageList.size() > 0)
      {
        Slide slide;
        QString file = dir.filePath(QString(fileNameTemplate).arg(++i));

        Magick::Image img = imageList.front();
        // In landscape mode we propably want to fill the whole screen
        // using ! and 4:3 800x600 resolution
        QSize imgRes(img.columns(), img.rows());

        // Try to fix empty white space
        if(pdfRes == QSize(0, 0) && imgRes == QSize(794, 842))
          pdfRes = QSize(794, 595);

        if(pdfRes != imgRes && pdfRes.width() > 0 && pdfRes.height() > 0)
        {
          img.crop(Magick::Geometry(pdfRes.width(), pdfRes.height(), 0,
                   imgRes.height() - pdfRes.height()));
        }
        kdDebug() << k_funcinfo << imgRes << ", " << pdfRes << endl;

        if(landscape)
          img.zoom("!800x600");
        else
          img.zoom("800x600");

        img.write((const char*)file.local8Bit());
        imageList.pop_front();

        slide.comment = i18n("Page %1", i);
        slide.picture = file;
        result.append(slide);
        dlg.progressBar()->advance(pageProgress);
        kapp->processEvents();
      }
    }
    else
#endif
    {
      Slide slide;

      if(minfo.keys().contains("Comment") &&
         !minfo.item("Comment").value().toString().isEmpty())
      {
        slide.comment = minfo.item("Comment").value().toString();
      }
      else
      {
        if(minfo.keys().contains("CreationDate"))
          slide.comment = minfo.item("CreationDate").value().toString();
        if(minfo.keys().contains("CreationTime"))
          slide.comment += " " + minfo.item("CreationTime").value().toString();
      }
      slide.comment = slide.comment.trimmed();
      slide.picture = file;
      result.append(slide);
      dlg.progressBar()->setValue(dlg.progressBar()->value() + fileProgress);
      kapp->processEvents();
    }
  }
  dlg.progressBar()->setRange(0, 10000);

  int chapter = ((result.count() - 1) / 12) + 1;
  int i = 0;
  for(SlideList::Iterator it = result.begin();
      it != result.end(); ++it)
  {
    (*it).chapter = (((i++) % chapter) == 0);
  }
  return result;
}

void SlideshowObject::addPics(QStringList lst)
{
  if(m_id.isEmpty())
    generateId();
  m_slides = slideList(lst);
}

void SlideshowObject::generateId()
{
  int serial = projectInterface()->serial();
  QString name = KMF::Tools::simpleName(title());
  m_id.sprintf("%3.3d_%s", serial, (const char*)name.toLocal8Bit());
}

void SlideshowObject::toXML(QDomElement& element) const
{
  QDomDocument doc = element.ownerDocument();
  QDomElement slideshow = doc.createElement("slideshow");
  slideshow.setAttribute("title", title());
  slideshow.setAttribute("id", m_id);
  slideshow.setAttribute("duration", m_duration);
  slideshow.setAttribute("loop", m_loop);
  slideshow.setAttribute("include_originals", m_includeOriginals);

  for(QStringList::ConstIterator it = m_audioFiles.begin();
      it != m_audioFiles.end(); ++it)
  {
    QDomElement e = doc.createElement("audio");
    e.setAttribute("path", *it);
    slideshow.appendChild(e);
  }

  for(SlideList::ConstIterator it = m_slides.begin();
      it != m_slides.end(); ++it)
  {
    QDomElement e = doc.createElement("file");
    e.setAttribute("path", (*it).picture);
    e.setAttribute("comment", (*it).comment);
    e.setAttribute("chapter", (*it).chapter);
    slideshow.appendChild(e);
  }
  element.appendChild(slideshow);
}

void SlideshowObject::fromXML(const QDomElement& element)
{
  QDomNode n = element.firstChild();
  while(!n.isNull())
  {
    QDomElement e = n.toElement();
    if(!e.isNull())
    {
      if(e.tagName() == "slideshow")
      {
        m_id = e.attribute("id");
        setTitle(e.attribute("title"));
        m_duration = e.attribute("duration",
            QString::number(SlideshowPluginSettings::slideDuration()))
            .toDouble();
        m_loop = e.attribute("loop", "0").toInt();
        m_includeOriginals = e.attribute("include_originals", "1").toInt();
        QDomNode m = e.firstChild();
        while(!m.isNull())
        {
          QDomElement e2 = m.toElement();
          if(!e2.isNull())
          {
            if(e2.tagName() == "file")
            {
              Slide slide;

              slide.picture = e2.attribute("path");
              slide.comment = e2.attribute("comment");
              slide.chapter = e2.attribute("chapter", "1").toInt();
              m_slides.append(slide);
            }
            else if(e2.tagName() == "audio")
            {
              m_audioFiles.append(e2.attribute("path"));
            }
          }
          m = m.nextSibling();
        }
      }
    }
    n = n.nextSibling();
  }
  if(m_id.isEmpty())
    generateId();
}

QPixmap SlideshowObject::pixmap() const
{
  if(m_slides.count() > 0)
    return KIO::pixmapForUrl(m_slides[0].picture);
  else
    return KIO::pixmapForUrl(KUrl(""));
}

void SlideshowObject::actions(QList<QAction*>& actionList) const
{
  actionList.append(m_slideshowProperties);
}

bool SlideshowObject::copyOriginals() const
{
  KUrl::List files;

  for(SlideList::ConstIterator it = m_slides.begin();
      it != m_slides.end(); ++it)
  {
    files.append((*it).picture);
  }
  KMF::Tools::stripExisting(&files,
                            projectInterface()->projectDir("DVD/PICTURES"));
  if(files.count() > 0)
    KIO::copy(files, projectInterface()->projectDir("DVD/PICTURES"));
  return true;
}

bool SlideshowObject::make(QString type)
{
  uiInterface()->message(KMF::Info, i18n("Preparing file(s) for %1", title()));
  if(type != "dummy")
  {
    if(m_includeOriginals)
      copyOriginals();
    if(!convertToDVD())
      return false;
  }
  uiInterface()->progress(TotalPoints);
  return true;
}

bool SlideshowObject::writeSlideshowFile() const
{
  if(m_slides.count() < 1)
    return false;

  QDir dir(projectInterface()->projectDir("media"));
  QString output = dir.filePath(QString("%1.slideshow").arg(m_id));
  QFile file(output);
  double duration = calculatedSlideDuration();

   if(file.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    QTextStream ts(&file);
    ts << QString(
        "#**************************************************************\n"
        "#\n"
        "# This file was made with %1 - %2\n"
        "# http://www.iki.fi/damu/software/kmediafactory/\n"
        "# \n"
        "#**************************************************************\n")
        .arg(KGlobal::mainComponent().aboutData()->programName())
        .arg(KGlobal::mainComponent().aboutData()->version());

    ts << "background:0::black\n";
    ts << "fadein:1\n";
    for(SlideList::ConstIterator it = m_slides.begin();
        it != m_slides.end(); ++it)
    {
      QString comment = (*it).comment;
      comment.replace(":", "\\:");
      comment.replace("\n", " ");
      ts << (*it).picture << ":" << QString::number(duration, 'f', 2) <<
          ":" << comment << "\n";
      if(it != m_slides.end())
        ts << "crossfade:1\n";
    }
    ts << "fadeout:1\n";
    file.close();
    return true;
  }
  return false;
}

void SlideshowObject::clean()
{
  QStringList list;
  QString name = QString("%1").arg(m_id);

  list.append(name + ".vob");
  list.append(name + ".xml");
  list.append(name + ".slideshow");
  list.append("dvd-slideshow.log");
  plugin()->projectInterface()->cleanFiles("media", list);
}

void SlideshowObject::output(K3Process* process, char* buffer, int buflen)
{
  bool stopped = false;
  int find = 0, start = 0;
  QRegExp re("[\n\r]");
  m_buffer += QString::fromLatin1(buffer, buflen);

  while((find = m_buffer.indexOf(re, find)) > -1)
  {
    QString line = m_buffer.mid(start, find - start);

    //kdDebug() << k_funcinfo << line << endl;
    QRegExp re2(" (\\d+)\\/(\\d+) ");
    int pos = re2.indexIn(line);
    if(pos > -1)
    {
      // Maximu is eg. 6/5
      uiInterface()->setItemTotalSteps(re2.cap(2).toInt() + 1);
      stopped = uiInterface()->setItemProgress(re2.cap(1).toInt() - 1);
    }
    ++find;
    start = find;
    if(stopped)
      process->kill();
  }
  m_buffer.remove(0, start);
}

bool SlideshowObject::convertToDVD() const
{
  QDir dir(projectInterface()->projectDir("media"));
  QString output = dir.filePath(QString("%1.vob").arg(m_id));
  QFileInfo fio(output);
  SlideshowPlugin* slideshowPlugin = static_cast<SlideshowPlugin*>(plugin());
  bool result = false;

  if(!fio.exists())
  {
    if(writeSlideshowFile() == false)
    {
      uiInterface()->message(KMF::Error,
          i18n("   Can't write slideshow file."));
      return false;
    }

    K3Process dvdslideshow;

    uiInterface()->message(KMF::Info, i18n("   Making Slideshow"));
    dvdslideshow << slideshowPlugin->dvdslideshowBin() <<
        "-o" << projectInterface()->projectDir("media") <<
        "-n" << m_id <<
        "-f" << dir.filePath(QString("%1.slideshow").arg(m_id));
    if(projectInterface()->type() == "DVD-PAL")
      dvdslideshow << "-p";
    for(QStringList::ConstIterator it = m_audioFiles.begin();
        it != m_audioFiles.end(); ++it)
    {
      dvdslideshow << "-a" << *it;
    }
    dvdslideshow.setWorkingDirectory(projectInterface()->projectDir("media"));
    uiInterface()->logger()->connectProcess(&dvdslideshow,
                                            "INFO: \\d+ bytes of data written");
    connect(&dvdslideshow, SIGNAL(receivedStdout(K3Process*, char*, int)),
             this, SLOT(output(K3Process*, char*, int)));
    connect(&dvdslideshow, SIGNAL(receivedStderr(K3Process*, char*, int)),
             this, SLOT(output(K3Process*, char*, int)));
    kDebug() << k_funcinfo << dvdslideshow.args() << endl;
    dvdslideshow.start(K3Process::Block, K3Process::AllOutput);
    if(dvdslideshow.normalExit())
    {
      if(dvdslideshow.exitStatus() == 0)
        result = true;
    }
    if(!result)
      uiInterface()->message(KMF::Error, i18n("   Slideshow error."));
  }
  else
  {
    uiInterface()->message(KMF::Info,
        i18n("   Slideshow \"%1\" seems to be up to date", title()));
    result = true;
  }
  return result;
}

int SlideshowObject::timeEstimate() const
{
  return TotalPoints;
}

void SlideshowObject::writeDvdAuthorXml(QDomElement& element,
                                        QString preferredLanguage,
                                        QString post, QString type)
{
  QDir dir(projectInterface()->projectDir("media"));

  QDomDocument doc = element.ownerDocument();
  QDomElement titles = doc.createElement("titles");

  QDomElement video = doc.createElement("video");
  video.setAttribute("aspect",
      QDVD::VideoTrack::aspectRatioString(QDVD::VideoTrack::Aspect_4_3));
  titles.appendChild(video);

  QDomElement audioElem = doc.createElement("audio");
  audioElem.setAttribute("lang", preferredLanguage);
  titles.appendChild(audioElem);

  QDomElement sub = doc.createElement("subpicture");
  sub.setAttribute("lang", preferredLanguage);
  titles.appendChild(sub);

  QDomElement pgc = doc.createElement("pgc");
  QDomElement pre = doc.createElement("pre");
  QDomText text = doc.createTextNode("");
  QString commands = "";

  QDomElement vob = doc.createElement("vob");

  if(type != "dummy")
  {
    vob.setAttribute("file", dir.filePath(QString("%1.vob").arg(m_id)));

    QList<double> chapters;
    double start = 0.0;
    double duration = calculatedSlideDuration();

    for(SlideList::ConstIterator it = m_slides.begin();
        it != m_slides.end(); ++it)
    {
      if((*it).chapter)
        chapters.append(start);

      // Forward over first fade in (0.4 is added to be sure we are out
      // of fade)
      if(start == 0.0)
        start = 1.4;
      start += duration + 1.0;
    }
    for(int i; i < chapters.count(); ++i)
    {
      QDomElement c = vob.ownerDocument().createElement("cell");

      c.setAttribute("start", KMF::Time(chapters[i]).toString());
      if(i == chapters.count())
        c.setAttribute("end", "-1");
      else
      {
        c.setAttribute("end", KMF::Time(chapters[i+1]).toString());
      }
      c.setAttribute("chapter", 1);
      vob.appendChild(c);
    }
  }
  else
  {
    vob.setAttribute("file", dir.filePath("dummy.mpg"));
    QDomElement c = vob.ownerDocument().createElement("cell");

    c.setAttribute("start", KMF::Time().toString());
    c.setAttribute("end", "-1");
    c.setAttribute("chapter", 1);
    vob.appendChild(c);
  }

  pgc.appendChild(vob);

  QDomElement postElem = doc.createElement("post");
  QDomText text2;
  if(m_loop)
    text2 = doc.createTextNode(" jump chapter 1 ; ");
  else
    text2 = doc.createTextNode(post);
  postElem.appendChild(text2);
  pgc.appendChild(postElem);

  titles.appendChild(pgc);
  element.appendChild(titles);
}

bool SlideshowObject::lastChapter(SlideList::ConstIterator& iter)
{
  SlideList::ConstIterator it = iter;

  for(++it; it != m_slides.end(); ++it)
  {
    if((*it).chapter)
      return false;
  }
  return true;
}

QImage SlideshowObject::preview(int chap) const
{
  int n = 0;
  for(SlideList::ConstIterator it = m_slides.begin();
      it != m_slides.end(); ++it)
  {
    if((*it).chapter == true)
      ++n;
    if(n >= chap)
    {
      QImage img((*it).picture);
      QSize res = KMF::Tools::resolution(img.size(), img.size(),
          KMF::Tools::maxResolution(projectInterface()->type()), QSize(4,3));
      img = img.scaled(res, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
      return img;
    }
  }
  return QImage();
}

QString SlideshowObject::text(int chap) const
{
  if(chap == MainTitle)
    return title();
  else
  {
    return chapter(chap).comment;
  }
}

int SlideshowObject::chapters() const
{
  int i = 0;

  for(SlideList::ConstIterator it = m_slides.begin();
      it != m_slides.end(); ++it)
  {
    if((*it).chapter)
      ++i;
  }
  return i;
}

const Slide& SlideshowObject::chapter(int chap) const
{
  int i = 0;

  for(SlideList::ConstIterator it = m_slides.begin();
      it != m_slides.end(); ++it)
  {
    if((*it).chapter)
      ++i;
    if(i == chap)
      return *it;
  }
  return m_slides.first();
}

uint64_t SlideshowObject::size() const
{
  QDir dir(projectInterface()->projectDir("media"));
  QString output = dir.filePath(QString("%1.vob").arg(m_id));
  KFileItem finfo(KFileItem::Unknown, KFileItem::Unknown, KUrl(output));
  uint64_t size = finfo.size();

  if(size == 0)
  {
    double d = (double)KMF::Time(duration());
    size = (uint64_t)(d * 655415.35);
  }
  return size;
}

double SlideshowObject::calculatedSlideDuration() const
{
  double duration = m_duration;

  if(duration < 1.0)
  {
    duration = KMF::Time(audioDuration());
    duration = (((duration - 1.0) / m_slides.count()) - 1.0);
  }
  return duration;
}

QTime SlideshowObject::audioDuration() const
{
  KMF::Time audioDuration = 0.0;

  for(QStringList::ConstIterator it = m_audioFiles.begin();
      it != m_audioFiles.end(); ++it)
  {
# warning TODO: Get audio duration using tools that dvdslideshow depends
    //QFFMpeg audio(*it);
    //audioDuration += audio.duration();
  }
  return audioDuration;
}

QTime SlideshowObject::duration() const
{
  KMF::Time total;

  if(m_duration < 1.0)
    total = audioDuration();
  else
    total = (1.0 + m_duration) * m_slides.count() + 1.0;
  return total;
}

QTime SlideshowObject::chapterTime(int chap) const
{
  KMF::Time total;
  int i = 0, n = 0;

  for(SlideList::ConstIterator it = m_slides.begin();
      it != m_slides.end(); ++it)
  {
    if((*it).chapter)
      ++i;
    ++n;
    if(chap == i)
      break;
  }
  total += (1.0 + calculatedSlideDuration()) * n + 1.0;
  return total;
}

void SlideshowObject::slotProperties()
{
  SlideshowProperties dlg(kapp->activeWindow());
  dlg.setData(*this);
  if (dlg.exec())
  {
    clean();
    dlg.getData(*this);
    projectInterface()->setDirty(KMF::ProjectInterface::DirtyMedia);
  }
}

#include "slideshowobject.moc"
