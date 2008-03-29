//**************************************************************************
//   Copyright (C) 2004 by Petri Damst�
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
#include "videoobject.h"
#include "videoplugin.h"
#include "videooptions.h"
#include "videopluginsettings.h"
#include <kfileitem.h>
#include <kmftime.h>
#include <kmftools.h>
#include <qffmpeg.h>
#include <kapplication.h>
#include <kstandarddirs.h>
#include <kmimetype.h>
#include <kurl.h>
#include <kmdcodec.h>
#include <kstringhandler.h>
#include <klocale.h>
#include <kdebug.h>
#include <qdom.h>
#include <qimage.h>
#include <qcolor.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <unistd.h>
#include <errno.h>

char* VideoObject::m_prefixes[] = {".sub.mpg", ".mpg", "", ".xml", ".sub"};

VideoObject::VideoObject(QObject* parent)
  : MediaObject(parent, "video"), m_decoder(0), m_videoPlay(0),
    m_aspect(QDVD::VideoTrack::Aspect_Unknown)
{
  m_decoder = new QFFMpeg;
  m_videoProperties = new KAction(
      i18n("&Properties"), "pencil", 0, this,
      SLOT(slotProperties()),plugin()->actionCollection(),"mob_properties");

  m_kmfplayer = KStandardDirs::findExe("kmediafactoryplayer");
  if(!m_kmfplayer.isEmpty())
    m_videoPlay = new KAction(i18n("Play video"), "viewmag",
                              CTRL+Key_P,
                              this, SLOT(slotPlayVideo()),
                              plugin()->actionCollection(),
                              "mob_play");
  connect(m_decoder, SIGNAL(convertProgress(int)),
          this, SLOT(slotProgress(int)));
  connect(m_decoder, SIGNAL(message(const QString&)),
          uiInterface()->logger(), SLOT(message(const QString&)));
}

VideoObject::~VideoObject()
{
  delete m_decoder;
}

double VideoObject::frameRate() const
{
  return m_decoder->frameRate();
}

QTime VideoObject::duration() const
{
  //kdDebug() << k_funcinfo << m_decoder->duration() << endl;
  return m_decoder->duration();
}

QTime VideoObject::chapterTime(int chap) const
{
  return chapter(chap).start();
}

QStringList VideoObject::files() const
{
  return m_decoder->fileNames();
}

QString VideoObject::fileName() const
{
  return m_decoder->fileNames().first();
}

void VideoObject::actions(QPtrList<KAction>& actionList) const
{
  if(m_videoPlay)
    actionList.append(m_videoPlay);
  actionList.append(m_videoProperties);
}

void VideoObject::slotProgress(int progress)
{
  if(uiInterface()->setItemProgress(progress))
  {
    m_stopped = true;
    m_decoder->stop();
  }
}

void VideoObject::fromXML(const QDomElement& element)
{
  m_cells.clear();
  m_audioTracks.clear();
  QDomNode n = element.firstChild();
  while(!n.isNull())
  {
    QDomElement e = n.toElement();
    if(!e.isNull())
    {
      if(e.tagName() == "video")
      {
        m_id = e.attribute("id");
        setTitle(e.attribute("title"));
        setPreviewURL(e.attribute("custom_preview"));
        if(e.hasAttribute("auto_chapters"))
          setCellSecs(e.attribute("auto_chapters").toDouble());
        setAspect((QDVD::VideoTrack::AspectRatio)
            e.attribute("aspect", "3").toInt());

        QDomNode m = e.firstChild();
        while(!m.isNull())
        {
          QDomElement e2 = m.toElement();
          if(!e2.isNull())
          {
            if(e2.tagName() == "file")
            {
              m_decoder->addFile(e2.attribute("path"));
            }
            else if(e2.tagName() == "cell")
            {
              QDVD::Cell cell;
              KMF::Time start(e2.attribute("start"));
              KMF::Time length(e2.attribute("length"));
              QString name = e2.attribute("name");
              bool chapter = (e2.attribute("chapter") == "1");

              uint file = e2.attribute("file", 0).toUInt();
              if(file > 1)
              {
                const QFFMpegFileList& files = m_decoder->files();

                for(uint i= 0; i < file - 1 && i < files.count(); ++i)
                  start += files[i].duration();
              }
              //kdDebug() << k_funcinfo << file  << ", " << start
              //    << ", " << chapter << endl;
              if(e2.hasAttribute("name"))
                name = e2.attribute("name");
              else
                name = start.toString("h:mm:ss");

              addCell(QDVD::Cell(start, length, name, chapter));
            }
            else if(e2.tagName() == "audio")
            {
              QDVD::AudioTrack a(e2.attribute("language",
                  VideoPluginSettings::defaultAudioLanguage()));
              addAudioTrack(a);
            }
            else if(e2.tagName() == "subtitle")
            {
              KMF::Font font;

              QDomNode m2 = e2.firstChild();
              while(!m2.isNull())
              {
                QDomElement e3 = m2.toElement();
                if(!e3.isNull())
                {
                  if(e3.tagName() == "font")
                    font.fromXML(e3);
                }
                m2 = m2.nextSibling();
              }
              QDVD::Subtitle s;
              s.setFile(e2.attribute("file"));
              s.setLanguage(e2.attribute("language",
                             VideoPluginSettings::defaultSubtitleLanguage()));
              QString a = e2.attribute("align",
                  QString("%1").arg(Qt::AlignHCenter | Qt::AlignBottom));
              s.setAlignment(Qt::AlignmentFlags(a.toInt()));
              s.setFont(font);
              addSubtitle(s);
            }
            else if(e2.tagName() == "convert_params")
            {
              m_conversion.m_pass = e2.attribute("passes", "1").toInt();
              m_conversion.m_videoBitrate = e2.attribute("video_bitrate",
                  "8000").toInt();
              m_conversion.m_audioBitrate = e2.attribute("audio_bitrate",
                  "128").toInt();
              m_conversion.m_audioType = e2.attribute("audio_type",
                  "0").toInt();
            }
          }
          m = m.nextSibling();
        }
      }
    }
    n = n.nextSibling();
  }
  checkObjectParams();
}

void VideoObject::checkObjectParams()
{
  while(m_audioTracks.count() < m_decoder->audioStreamCount())
  {
    addAudioTrack(
        QDVD::AudioTrack(VideoPluginSettings::defaultAudioLanguage()));
  }
  if(m_cells.count() < 1)
    setCellSecs(300.0);
  if(m_id.isEmpty())
    generateId();
  if(title().isEmpty())
    setTitleFromFileName();
  if(m_aspect == QDVD::VideoTrack::Aspect_Unknown)
    m_aspect = (QDVD::VideoTrack::AspectRatio)m_decoder->aspectRatio();
}

void VideoObject::toXML(QDomElement& element) const
{
  QDomDocument doc = element.ownerDocument();
  QDomElement video = doc.createElement("video");
  video.setAttribute("title", title());
  video.setAttribute("aspect", (int)m_aspect);
  video.setAttribute("id", m_id);

  if(m_previewURL.isValid())
    video.setAttribute("custom_preview", m_previewURL.prettyURL());

  QDomElement convert = doc.createElement("convert_params");
  convert.setAttribute("passes", m_conversion.m_pass);
  convert.setAttribute("video_bitrate", m_conversion.m_videoBitrate);
  convert.setAttribute("audio_bitrate", m_conversion.m_audioBitrate);
  convert.setAttribute("audio_type", m_conversion.m_audioType);
  video.appendChild(convert);

  QStringList lst = m_decoder->fileNames();
  for(QStringList::ConstIterator it = lst.begin(); it != lst.end(); ++it)
  {
    QDomElement e = doc.createElement("file");
    e.setAttribute("path", (*it));
    video.appendChild(e);
  }
  for(QDVD::CellList::ConstIterator it = m_cells.begin();
      it != m_cells.end(); ++it)
  {
    QDomElement e = doc.createElement("cell");
    e.setAttribute("name", (*it).name());
    e.setAttribute("start", KMF::Time((*it).start()).toString());
    e.setAttribute("length", KMF::Time((*it).length()).toString());
    e.setAttribute("chapter", (*it).isChapter());
    video.appendChild(e);
  }
  for(QDVD::AudioList::ConstIterator it = m_audioTracks.begin();
      it != m_audioTracks.end(); ++it)
  {
    QDomElement e = doc.createElement("audio");
    e.setAttribute("language", (*it).language());
    video.appendChild(e);
  }
  for(QDVD::SubtitleList::ConstIterator it = m_subtitles.begin();
      it != m_subtitles.end(); ++it)
  {
    QDomElement e = doc.createElement("subtitle");
    e.setAttribute("language", (*it).language());
    e.setAttribute("file", (*it).file());
    e.setAttribute("align", (int)(*it).alignment());
    QDomElement e2 = doc.createElement("font");
    KMF::Font((*it).font()).toXML(e2);
    e.appendChild(e2);
    video.appendChild(e);
  }
  element.appendChild(video);
}

void VideoObject::addCell(QDomElement& vob, const QDVD::Cell& cell,
                          const KMF::Time& fileStart)
{
  KMF::Time start(cell.start());
  KMF::Time end(cell.start());

  start -= fileStart;
  QDomElement c = vob.ownerDocument().createElement("cell");

  c.setAttribute("start", KMF::Time(start).toString());
  if(cell.length().isNull())
    c.setAttribute("end", "-1");
  else
  {
    end -= fileStart;
    end += cell.length();
    c.setAttribute("end", end.toString());
  }
  c.setAttribute("chapter", cell.isChapter());
  vob.appendChild(c);
  //kdDebug() << k_funcinfo << start << ", " << end << endl;
}

void VideoObject::writeDvdAuthorXml(QDomElement& element,
                                    QString preferredLanguage,
                                    QString post, QString type)
{
  QDir dir(projectInterface()->projectDir("media"));
  int audioTrack = 0; // First audio track
  int subTrack = 62; // Let player decide
  bool audioFound = false;
  bool subFound = false;
  int i;

  QDomDocument doc = element.ownerDocument();
  QDomElement titles = doc.createElement("titles");

  QDomElement video = doc.createElement("video");
  video.setAttribute("aspect", QDVD::VideoTrack::aspectRatioString(m_aspect));
  titles.appendChild(video);

  i = 0;
  for(QDVD::AudioList::ConstIterator it = m_audioTracks.begin();
      it != m_audioTracks.end(); ++it)
  {
    QDomElement audioElem = doc.createElement("audio");
    if((*it).language() == preferredLanguage && audioFound == false)
    {
      audioTrack = i;
      audioFound = true;
    }
    audioElem.setAttribute("lang", (*it).language());
    titles.appendChild(audioElem);
    ++i;
  }

  i = 0;
  for(QDVD::SubtitleList::ConstIterator it = m_subtitles.begin();
      it != m_subtitles.end(); ++it)
  {
    QString lang = (*it).language();
    if(lang.isEmpty())
      lang = "xx";
    QDomElement sub = doc.createElement("subpicture");
    if(lang == preferredLanguage && subFound == false)
    {
      subFound = true;
      subTrack = i;
    }
    sub.setAttribute("lang", lang);
    titles.appendChild(sub);
    ++i;
  }

  QDomElement pgc = doc.createElement("pgc");
  QDomElement pre = doc.createElement("pre");
  QDomText text = doc.createTextNode("");
  QString commands = "";

  commands += QString("audio=%1; ").arg(audioTrack);
  if(!audioFound && subTrack < 32)
    subTrack += 64; // All subtitles, not just forced ones
  commands += QString("subtitle=%1; ").arg(subTrack);
  text.setData(QString(" { %1} ").arg(commands));
  pre.appendChild(text);
  pgc.appendChild(pre);

  KMF::Time pos;
  i = 0;
  QDVD::CellList::ConstIterator cell = m_cells.begin();

  for(QFFMpegFileList::const_iterator it = m_decoder->files().begin();
      it != m_decoder->files().end(); ++it, ++i)
  {
    QDomElement vob = doc.createElement("vob");
    if(type != "dummy")
    {
      QString file = videoFileFind(i);
      //kdDebug() << k_funcinfo << file << endl;
      vob.setAttribute("file", file);

      if(cell == m_cells.end() || pos != (*cell).start())
      {
        KMF::Time length;

        if(cell != m_cells.end() &&
          (*cell).start() < pos + (*it).duration())
        {
          length = KMF::Time((*cell).start()) - pos;
        }
        addCell(vob, QDVD::Cell(QTime(), length, "", false), pos);
      }
      while(((*cell).start() < pos + (*it).duration()
              || it == m_decoder->files().fromLast())
            && cell != m_cells.end())
      {
        addCell(vob, *cell, pos);
        ++cell;
      }
    }
    else
    {
      addCell(vob, QDVD::Cell(QTime(), QTime(), "", true), pos);
      vob.setAttribute("file", dir.filePath("dummy.mpg"));
    }

    pos += (*it).duration();
    pgc.appendChild(vob);
  }
  QDomElement postElem = doc.createElement("post");
  QDomText text2 = doc.createTextNode(post);
  postElem.appendChild(text2);
  pgc.appendChild(postElem);
  titles.appendChild(pgc);
  element.appendChild(titles);
}

int VideoObject::timeEstimate() const
{
  return TotalPoints;
}

QString VideoObject::videoFileName(int index, VideoFilePrefix prefix)
{
  QDir dir(projectInterface()->projectDir("media"));
  QString file = QFileInfo(m_decoder->files()[index].fileName()).fileName();

  /*
  kdDebug() << k_funcinfo << dir.filePath(QString("%1_%2")
      .arg(QString::number(index+1).rightJustify(3, '0'))
      .arg(file) + m_prefixes[prefix]) << endl;
  */
  return dir.filePath(QString("%1_%2")
      .arg(QString::number(index+1).rightJustify(3, '0'))
      .arg(file) + m_prefixes[prefix]);
}

QString VideoObject::videoFileFind(int index, VideoFilePrefix prefixStart) const
{
  QDir dir(projectInterface()->projectDir("media"));
  QString file = QFileInfo(m_decoder->files()[index].fileName()).fileName();

  for(int i = prefixStart; i < PrefixEmpty; ++i)
  {
    QString s = dir.filePath(QString("%1_%2")
        .arg(QString::number(index+1).rightJustify(3, '0'))
        .arg(file) + m_prefixes[i]);
    QFileInfo fi(s);
    if(fi.exists())
    {
      //kdDebug() << k_funcinfo << s << endl;
      return s;
    }
  }
  //kdDebug() << k_funcinfo << m_decoder->files()[index].fileName() << endl;
  return m_decoder->files()[index].fileName();
}

bool VideoObject::convertToDVD()
{
  static char* codecs[] = {"ac3", "mp2", "pcm"};
  bool result = true;
  int i = 0;
  QDir dir(projectInterface()->projectDir("media"));

  m_stopped = false;

  for(QFFMpegFileList::const_iterator it = m_decoder->files().begin();
      it != m_decoder->files().end() && m_stopped == false; ++it, ++i)
  {
    QString input = (*it).fileName();
    QFileInfo fii(input);
    QString output = videoFileName(i, PrefixMpg);
    output = dir.filePath(output);
    QFileInfo fio(output);

    if(fio.lastModified() < fii.lastModified())
    {
      for(int j = 0; j < m_conversion.m_pass && m_stopped == false; ++j)
      {
        QTime t; t.start();
        //kdDebug() << k_funcinfo << input << endl;

        QString pass;

        if(m_conversion.m_pass > 1)
          pass = QString(" (pass %1)").arg(j+1);
        uiInterface()->message(KMF::Info,
            i18n("   Converting %1 to DVD format%2")
          .arg(fii.fileName()).arg(pass));
        KMF::Time duration = m_decoder->duration();
        uiInterface()->setItemTotalSteps((int)((double)duration *
                                        m_decoder->frameRate()));
        // ffmpeg -i koe.dv -hq -acodec ac3 -aspect 4:3 -pass 2 -target pal-dvd
        //        ./output.mpeg
        QFFMpegConvertTo dvd;
        if(projectInterface()->type() == "DVD-PAL")
          dvd.append(QFFMpegParam("target", "pal-dvd"));
        else
          dvd.append(QFFMpegParam("target", "ntsc-dvd"));
        dvd.append(QFFMpegParam("aspect",
                   QDVD::VideoTrack::aspectRatioString(m_aspect)));
        dvd.append(QFFMpegParam("b", m_conversion.m_videoBitrate));
        dvd.append(QFFMpegParam("maxrate", m_conversion.m_videoBitrate));
        dvd.append(QFFMpegParam("minrate", 0));
        dvd.append(QFFMpegParam("ab", m_conversion.m_audioBitrate));
        dvd.append(QFFMpegParam("acodec", codecs[m_conversion.m_audioType]));
        if(m_conversion.m_pass > 1)
        {
          dvd.append(QFFMpegParam("pass", j+1));
          dvd.append(QFFMpegParam("passlogfile",
                     videoFileName(i, PrefixEmpty)));
        }
        result = m_decoder->convertTo(dvd, i, fio.filePath());
        if(result == false)
        {
          if(m_stopped == false)
          {
            uiInterface()->message(KMF::Error, i18n("   Conversion error."));
            m_stopped = true;
          }
          QFile(fio.filePath()).remove();
        }
        kdDebug() << k_funcinfo << QString("Time elapsed: %1 ms")
            .arg(t.elapsed()) << endl;
      }
    }
    else
    {
      /*
      kdDebug() << k_funcinfo << fii.fileName() << " -> " << fio.fileName()
          << endl;
      */
      uiInterface()->message(KMF::Info,
        i18n("   Conversion of %1 seems to be up to date").arg(fii.fileName()));
    }
  }
  return (result && !m_stopped);
}

QString VideoObject::checkFontFile(const QString& file)
{
  QFileInfo fi(file);
  QDir dir(QDir::home().filePath(".spumux"));
  QFileInfo link(dir.absFilePath(fi.fileName()));

  if(!dir.exists())
    dir.mkdir(dir.path());
  //kdDebug() << k_funcinfo << link.filePath() << " -> " << file << endl;
  if(!link.exists())
    if(symlink(file, link.filePath()) < 0)
      kdDebug() << k_funcinfo << strerror(errno) << endl;
  return fi.fileName();
}

bool VideoObject::writeSpumuxXml(QDomDocument& doc,
                                 const QString& subFile,
                                 const QDVD::Subtitle& subtitle)
{
  QDomElement root = doc.createElement("subpictures");
  QDomElement stream = doc.createElement("stream");
  QDomElement textsub = doc.createElement("textsub");

  textsub.setAttribute("filename", subFile);
  textsub.setAttribute("vertical-alignment", subtitle.verticalAlign());
  textsub.setAttribute("horizontal-alignment", subtitle.horizontalAlign());
  textsub.setAttribute("movie-width", "720");
  if(projectInterface()->type() == "DVD-PAL")
  {
    textsub.setAttribute("movie-fps", "25");
    textsub.setAttribute("movie-height", "574");
  }
  else
  {
    textsub.setAttribute("movie-fps", "29.97");
    textsub.setAttribute("movie-height", "478");
  }

  KMF::Font font(subtitle.font());
  if(!font.file().isEmpty())
    textsub.setAttribute("font", checkFontFile(font.file()));

  if(subtitle.font().pointSize() > 0)
    textsub.setAttribute("fontsize", subtitle.font().pointSize());

  stream.appendChild(textsub);
  root.appendChild(stream);
  doc.appendChild(root);
  return true;
}

bool VideoObject::writeSpumuxXml(const QString& fileName,
                                 const QString& subFile,
                                 const QDVD::Subtitle& subtitle)
{
  // Write spumux xml
  QString s;
  QDomDocument doc("");
  if(writeSpumuxXml(doc, subFile, subtitle) == false)
    return false;

  QFile file(fileName);
  if (!file.open(IO_WriteOnly))
    return false;
  QTextStream stream(&file);
  stream << doc.toString();
  file.close();
  return true;
}

void VideoObject::output(KProcess* process, char* buffer, int buflen)
{
  QRegExp re("[\n\r]");
  QRegExp bytes("INFO: (\\d+) bytes of data written");
  int n;

  m_buffer += QString::fromLatin1(buffer, buflen);
  while((n = m_buffer.find(re)) >= 0)
  {
    if(bytes.search(m_buffer.left(n)) > -1)
    {
      if(m_lastUpdate.elapsed() > 250)
      {
        if(uiInterface()->setItemProgress(bytes.cap(1).toInt() / 1024))
          process->kill();
        m_lastUpdate.start();
      }
    }
    ++n;
    m_buffer.remove(0, n);
  }
}

bool VideoObject::convertSubtitles(const QDVD::Subtitle& subtitle)
{
  uint i = 0;
  QDir dir(projectInterface()->projectDir("media"));
  QStringList subtitleFiles = QStringList::split(";", subtitle.file());

  for(QFFMpegFileList::const_iterator it = m_decoder->files().begin();
      it != m_decoder->files().end(); ++it, ++i)
  {
    if(i >= subtitleFiles.count())
      break;
    QFileInfo fio(videoFileName(i, PrefixSub));
    QFileInfo fiXml(videoFileName(i, PrefixXml));
    QFileInfo fii(videoFileFind(i, PrefixMpg));
    QFileInfo fiSub(subtitleFiles[i]);

    if(!fio.exists() ||
      fii.lastModified() > fio.lastModified() ||
      fiSub.lastModified() > fio.lastModified())
    {
      uiInterface()->message(KMF::Info,
          i18n("   Adding subtitles to %1").arg(fii.fileName()));

      KShellProcess m_spumux("bash");
      //kdDebug() << k_funcinfo << fiXml.filePath() << endl;
      writeSpumuxXml(fiXml.filePath(), fiSub.filePath(), subtitle);
      m_spumux << "spumux -P "
          + KShellProcess::quote(fiXml.filePath())
          + " < " + KShellProcess::quote(fii.filePath())
          + " > " + KShellProcess::quote(fio.filePath());
      m_spumux.setWorkingDirectory(projectInterface()->projectDir("media"));
      uiInterface()->logger()->connectProcess(&m_spumux,
          "INFO: \\d+ bytes of data written", KProcess::Stderr);
      connect(&m_spumux, SIGNAL(receivedStderr(KProcess*, char*, int)),
              this, SLOT(output(KProcess*, char*, int)));
      uiInterface()->setItemTotalSteps(fii.size()/1024);
      m_spumux.start(KProcess::Block, KProcess::Stderr);
      if(m_spumux.normalExit() && m_spumux.exitStatus() == 0)
        uiInterface()->setItemProgress(fii.size()/1024);
      else
      {
        QFile::remove(fio.filePath());
        uiInterface()->message(KMF::Error, i18n("   Conversion error."));
        return false;
      }
    }
    else
    {
      uiInterface()->message(KMF::Info,
          i18n("   Subtitle conversion seems to be up to date"));
    }
  }
  return true;
}

bool VideoObject::make(QString type)
{
  uiInterface()->message(KMF::Info,
      i18n("Preparing file(s) for %1").arg(title()));
  QString fileName;

  if(type != "dummy")
  {
    if(!m_decoder->isDVDCompatible())
    {
      if(!convertToDVD())
        return false;
    }

    for(QDVD::SubtitleList::ConstIterator it = m_subtitles.begin();
        it != m_subtitles.end(); ++it)
    {
      if(!(*it).file().isEmpty())
      {
        if(!convertSubtitles(*it))
          return false;
      }
    }
  }
  uiInterface()->progress(TotalPoints);
  return true;
}

void VideoObject::slotProperties()
{
  VideoOptions dlg(kapp->activeWindow());
  dlg.setData(*this);
  if (dlg.exec())
  {
    dlg.getData(*this);
    projectInterface()->setDirty(KMF::ProjectInterface::DirtyMedia);
  }
}

QPixmap VideoObject::pixmap() const
{
  return KMimeType::pixmapForURL(fileName());
}

#define PERCENT 1
#define LIGHT 50

bool VideoObject::isBlack(const QImage& img) const
{
  int pixelLimit = (img.height() * img.width() * PERCENT) / 100;
  int lightLimit = (255 * LIGHT) / 100;
  int pixel = 0;

  for (int y = 0; y < img.height(); y++)
  {
    for (int x = 0; x < img.width(); x++)
    {
      QRgb pix = img.pixel(x, y);
      if (qRed(pix) > lightLimit || qGreen(pix) > lightLimit ||
          qBlue(pix) > lightLimit)
        ++pixel;
      if(pixel > pixelLimit)
        return false;
    }
  }
  return true;
}

QImage VideoObject::getFrame(double time) const
{
  return m_decoder->frame(KMF::Time(time));
}

QImage VideoObject::preview(int chap) const
{
  bool black = true;
  double time = 0.0;
  int counter;
  QImage img;
  QString cacheFile;

  if(VideoPluginSettings::usePreviewCache())
  {
    QDir dir(projectInterface()->projectDir() + "/media/");
    if(!dir.exists())
      dir.mkdir(dir.path());

    QString s;
    cacheFile = dir.filePath(s.sprintf("%s_%4.4d.png",
                             (const char*)m_id.local8Bit(), chap));
    if(img.load(cacheFile))
      return img;
  }

  if (chap == MainPreview)
  {
    if(m_previewURL.isValid())
    {
      img.load(m_previewURL.path());
    }
  }

  if(img.isNull())
  {
    KMF::Time t = chapter(chap).start();
    time = t.toSeconds();

    counter = 0;
    while(black && counter < 60)
    {
      img = getFrame(time);
      black = isBlack(img);
      if(black)
        kdDebug() << "Black frame: " << time << endl;
      time += VideoPluginSettings::blackFrameJump();
      ++counter;
    }
  }

  // TODO Template aspect ratio other than 4:3 ?
  QSize templateRatio = QSize(4, 3);
  QSize videoRatio = (aspect() == QDVD::VideoTrack::Aspect_4_3) ?
                          QSize(4, 3) : QSize(16, 9);
  QSize imageRatio = KMF::Tools::guessRatio(img.size(), videoRatio);
  QSize templateSize = KMF::Tools::maxResolution(projectInterface()->type());
  QSize imageSize = img.size();
  QSize res = KMF::Tools::resolution(imageSize, imageRatio,
                                     templateSize, templateRatio);
  img = img.smoothScale(res, QImage::ScaleFree);
  /*
  kdDebug() << k_funcinfo << "Template size: " << templateSize << endl;
  kdDebug() << k_funcinfo << "Template ratio:" << templateRatio << endl;
  kdDebug() << k_funcinfo << "Video ratio:" << videoRatio << endl;
  kdDebug() << k_funcinfo << "Image size:" << imageSize << endl;
  kdDebug() << k_funcinfo << "Image ratio:" << imageRatio << endl;
  kdDebug() << k_funcinfo << "Final resolution:" << res << endl;
  */
  if(VideoPluginSettings::usePreviewCache())
    img.save(cacheFile, "PNG");
  return img;
}

QString VideoObject::text(int chap) const
{
  if(chap == MainTitle)
    return title();
  else
  {
    return chapter(chap).name();
  }
}

void VideoObject::setCellSecs(double secs)
{
  double d = KMF::Time(duration());
  int chapters = (int)(d / secs) + 1;

  // Do not put chapter too close to end
  if(((double)chapters * secs) + 30.0 > d && chapters > 1)
    --chapters;
  m_cells.clear();
  for(int i=0; i < chapters; ++i)
  {
    KMF::Time length(secs);
    if(i == chapters - 1)
      length = 0;
    QDVD::Cell c(KMF::Time(i*secs), length,
        QString(i18n("Chapter %1")).arg(i + 1));
    addCell(c);
  }
}

int VideoObject::chapters() const
{
  int i = 0;

  for(QDVD::CellList::ConstIterator it = m_cells.begin();
      it != m_cells.end(); ++it)
  {
    if((*it).isChapter())
      ++i;
  }
  return i;
}

const QDVD::Cell& VideoObject::chapter(int chap) const
{
  int i = 0;

  for(QDVD::CellList::ConstIterator it = m_cells.begin();
      it != m_cells.end(); ++it)
  {
    if((*it).isChapter())
      ++i;
    if(i == chap)
      return *it;
  }
  return m_cells.first();
}

void VideoObject::generateId()
{
  int serial = projectInterface()->serial();
  QString name = KMF::Tools::simpleBaseName(m_decoder->fileNames().first());
  m_id.sprintf("%3.3d_%s", serial, (const char*)name.local8Bit());
}

bool VideoObject::addFile(QString fileName)
{
  bool result = m_decoder->addFile(fileName);
  checkObjectParams();
  return result;
}

void VideoObject::setTitleFromFileName()
{
  QString name = KURL(fileName()).fileName();
  setTitle(KMF::Tools::simple2Title(name));
}

void VideoObject::slotPlayVideo()
{
  KProcess process;

  process << m_kmfplayer << fileName();
  process.start(KProcess::DontCare);
}

void VideoObject::printCells()
{
  KMF::Time next;
  int i = 1;

  for(QDVD::CellList::Iterator it = m_cells.begin();
      it != m_cells.end(); ++it, ++i)
  {
    kdDebug() << k_funcinfo << i << ": " << (*it).start()
        << " / " << (*it).length() << endl;
  }
}

void VideoObject::parseCellLengths()
{
  KMF::Time next;

  for(QDVD::CellList::Iterator it = m_cells.begin();
      it != m_cells.end(); ++it)
  {
    //kdDebug() << k_funcinfo << (*it).start() << endl;
    ++it;
    if(it != m_cells.end())
      next = (*it).start();
    else
      next = QTime();
    --it;
    if(!next.isNull())
      (*it).setLength(next - (*it).start());
    else
      (*it).setLength(QTime());
  }
}

void VideoObject::setCellList(const QDVD::CellList& list)
{
  m_cells = list;
  if(m_cells.count() == 0)
    m_cells.append(QDVD::Cell(QTime(), QTime(), QString("Chapter 1")));
}

uint64_t VideoObject::size() const
{
  uint64_t total = 0;
  int i = 0;

  for(QFFMpegFileList::const_iterator it = m_decoder->files().begin();
      it != m_decoder->files().end(); ++it, ++i)
  {
    QString file = videoFileFind(i);
    // DVD compatible or Converted file
    if((*it).isDVDCompatible() || (*it).fileName() != file)
    {
      KFileItem finfo(KFileItem::Unknown, KFileItem::Unknown, KURL(file));
      total += finfo.size();
    }
    else
    {
      double d = KMF::Time((*it).duration());
      int bitrate = m_conversion.m_videoBitrate;
      bitrate += m_audioTracks.count() * m_conversion.m_audioBitrate;
      bitrate += m_subtitles.count() * 4;
      total += (uint64_t)((d * bitrate * 1024.0) / 8.0);
    }
  }
  return total;
}

bool VideoObject::isDVDCompatible() const
{
  return m_decoder->isDVDCompatible();
}

QMap<QString, QString> VideoObject::subTypes() const
{
  QMap<QString, QString> result;

  result[""] = i18n("Make DVD");
  result["dummy"] = i18n("Make preview DVD with dummy videos");
  return result;
}

#include "videoobject.moc"
