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
#include "run.h"
#include <qdvdinfo.h>
#include <kmfmediafile.h>
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
#include <kprocess.h>
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
  : MediaObject(parent), m_loop(false), m_includeOriginals(true),
    dvdslideshow(0)
{
  setObjectName("slideshow");
  m_slideshowProperties = new KAction(KIcon("pencil"),
                                      i18n("&Properties"),this);
  m_slideshowProperties->setShortcut(Qt::CTRL + Qt::Key_W);
  plugin()->actionCollection()->addAction("mob_properties",
                                          m_slideshowProperties);
  connect(m_slideshowProperties, SIGNAL(triggered()), SLOT(slotProperties()));
  m_duration = SlideshowPluginSettings::slideDuration();
}

SlideshowObject::~SlideshowObject()
{
}

SlideList SlideshowObject::slideList(QStringList list) const
{
  SlideList result;
  KProgressDialog dlg(kapp->activeWindow());

  dlg.progressBar()->setMinimum(0);
  dlg.progressBar()->setMaximum(0);
  dlg.show();

  foreach(QString file, list)
  {
    KFileMetaInfo minfo(file, QString::null, KFileMetaInfo::ContentInfo);
    QString mime;
    QFileInfo fi(file);
    QDir dir(projectInterface()->projectDir("media"));
    KMimeType::Ptr type = KMimeType::findByUrl(file);

    dlg.setLabelText(i18n("Processing slides...\n") + (file));
    kapp->processEvents();

    if(fi.isDir())
    {
      KMessageBox::error(kapp->activeWindow(),
                         i18n("Cannot add directory."));
      continue;
    }

    if(type)
      mime = type->name();
    kDebug() << mime;
    if(mime.startsWith("application/vnd.oasis.opendocument") ||
       mime.startsWith("application/vnd.sun.xml") ||
       mime == "application/msexcel" ||
       mime == "application/msword" ||
       mime == "application/mspowerpoint")
    {
      QString output = QString("%1.pdf").arg(m_id);
      QDir dir(projectInterface()->projectDir("media"));
      output = dir.filePath(output);
      Run run(QString("kmf_oo2pdf \"%1\" \"%2\"").arg(file).arg(output));

      kDebug() << file << "->" << output;
      if(run.exitCode() == 0)
      {
        mime = "application/pdf";
        minfo = KFileMetaInfo(file, QString::null,
                              KFileMetaInfo::ContentInfo);
        file = output;
      }
    }
    if(mime == "application/pdf")
    {
      QString output = m_id + "_%d.png";
      QDir dir(projectInterface()->projectDir("media"));
      output = dir.filePath(output);
      Run run(QString("kmf_pdf2png \"%1\" \"%2\"").arg(file).arg(output));

      kDebug() << file << "->" << output;
      for(int i = 1; true; ++i)
      {
        Slide slide;
        QString fileNameTemplate = m_id + "_%1.png";
        QString file = dir.filePath(QString(fileNameTemplate).arg(i));
        QFileInfo fi(file);

        if(fi.exists())
        {
          kDebug() << "Slide: " << i;
          slide.comment = i18n("Page %1", i);
          slide.picture = file;
          result.append(slide);
        }
        else
          break;
      }
    }
    else
    {
      Slide slide;

      kDebug() << minfo.keys();
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
      if(slide.comment.isEmpty())
      {
        Run run(QString("kmf_comment \"%1\"").arg(file));

        if(run.exitStatus() == 0)
        {
          slide.comment = run.output();
        }
      }
      slide.comment = slide.comment.trimmed();
      slide.picture = file;
      result.append(slide);
      kapp->processEvents();
    }
  }
  int chapter = ((result.count() - 1) / 12) + 1;
  int i = 0;
  for(SlideList::Iterator it = result.begin();
      it != result.end(); ++it)
  {
    (*it).chapter = (((i++) % chapter) == 0);
  }
  dlg.close();
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

bool SlideshowObject::fromXML(const QDomElement& element)
{
  QDomNode n = element.firstChild();
  if(n.isNull())
    return false;
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
  return true;
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
    foreach(Slide slide, m_slides)
    {
      QString comment = slide.comment;
      comment.replace(":", "\\:");
      comment.replace("\n", " ");
      ts << slide.picture << ":" << QString::number(duration, 'f', 2) <<
          ":" << comment << "\n";
      if(slide.picture != m_slides.last().picture)
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

void SlideshowObject::output(QString line)
{
  bool stopped = false;

  QRegExp re2(" (\\d+)\\/(\\d+) ");
  int pos = re2.indexIn(line);
  if(pos > -1)
  {
    // Maximu is eg. 6/5
    uiInterface()->setItemTotalSteps(re2.cap(2).toInt() + 1);
    stopped = uiInterface()->setItemProgress(re2.cap(1).toInt() - 1);
  }
  if(stopped)
    dvdslideshow->kill();
}

bool SlideshowObject::convertToDVD()
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

    uiInterface()->message(KMF::Info, i18n("   Making Slideshow"));
    dvdslideshow = new KProcess();
    *dvdslideshow << slideshowPlugin->dvdslideshowBin();
    if(SlideshowPluginSettings::audioType() == 0)
      *dvdslideshow << "-mp2";
    *dvdslideshow << "-o" << projectInterface()->projectDir("media") <<
        "-n" << m_id <<
        "-f" << dir.filePath(QString("%1.slideshow").arg(m_id));
    if(projectInterface()->type() == "DVD-PAL")
      *dvdslideshow << "-p";
    for(QStringList::ConstIterator it = m_audioFiles.begin();
        it != m_audioFiles.end(); ++it)
    {
      *dvdslideshow << "-a" << *it;
    }
    dvdslideshow->setWorkingDirectory(projectInterface()->projectDir("media"));
    uiInterface()->logger()->connectProcess(dvdslideshow,
                                            "INFO: \\d+ bytes of data written");
    connect(uiInterface()->logger(), SIGNAL(line(QString)),
             this, SLOT(output(QString)));
    dvdslideshow->execute();
    if(dvdslideshow->exitCode() == QProcess::NormalExit)
    {
      if(dvdslideshow->exitStatus() == 0)
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
  delete dvdslideshow;
  dvdslideshow = 0;
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

    foreach(Slide slide, m_slides)
    {
      if(slide.chapter)
        chapters.append(start);

      // Forward over first fade in (0.520 is added to be sure we are out
      // of fade)
      if(start == 0.0)
        start = 0.520;
      start += duration;
    }
    for(int i = 0; i < chapters.count(); ++i)
    {
      QDomElement c = vob.ownerDocument().createElement("cell");

      c.setAttribute("start", KMF::Time(chapters[i]).toString());
      if(i == chapters.count() - 1)
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

QImage SlideshowObject::preview(int chap) const
{
  QImage img(chapter(chap).picture);
  QSize res = KMF::Tools::resolution(img.size(), img.size(),
      KMF::Tools::maxResolution(projectInterface()->type()), QSize(4,3));
  kDebug() << res;
  img = img.scaled(res, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  return img;
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

  foreach(Slide slide, m_slides)
  {
    if(slide.chapter)
      ++i;
  }
  return i;
}

const Slide& SlideshowObject::chapter(int chap) const
{
  int i = 0;

  foreach(const Slide& slide, m_slides)
  {
    if(slide.chapter)
      ++i;
    if(i == chap)
      return slide;
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

  foreach(QString file, m_audioFiles)
  {
    KMFMediaFile audio = KMFMediaFile::mediaFile(file);
    audioDuration += audio.duration();
  }
  return audioDuration;
}

QTime SlideshowObject::duration() const
{
  KMF::Time total;

  if(m_duration < 1.0)
    total = audioDuration();
  else
    total = m_duration * m_slides.count();
  return total;
}

QTime SlideshowObject::chapterTime(int chap) const
{
  KMF::Time total;
  int i = 0, n = 0;

  foreach(const Slide& slide, m_slides)
  {
    if(slide.chapter)
      ++i;
    ++n;
    if(chap == i)
      break;
  }
  total += calculatedSlideDuration() * n;
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
