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
#include "videoobject.h"
#include "videoplugin.h"
#include "videooptions.h"
#include "videopluginsettings.h"
#include <kmediafactory/job.h>
#include <kmfmediafile.h>
#include <kactioncollection.h>
#include <kfileitem.h>
#include <kmftime.h>
#include <kmftools.h>
#include <kapplication.h>
#include <kstandarddirs.h>
#include <kmimetype.h>
#include <kurl.h>
#include <kstringhandler.h>
#include <klocale.h>
#include <kdebug.h>
#include <kicon.h>
#include <krun.h>
#include <kprocess.h>
#include <kshell.h>
#include <KAction>
#include <QImage>
#include <QColor>
#include <QFileInfo>
#include <QDir>
#include <QPixmap>
#include <QTextStream>
#include <unistd.h>
#include <errno.h>

const char* VideoObject::m_prefixes[] =
  {".sub.mpg", ".mpg", "", ".xml", ".sub"};

class ConvertSubtitlesJob : public KMF::Job
{
public:
  QDVD::Subtitle subtitle;
  QString subtitleFile;
  QString subtitleXmlFile;
  QString videoFile;
  QString videoFileWithSubtitles;
  QString mediaDir;
  QString type;

  void run()
  {
    message(KMF::Info, i18n("   Adding subtitles to %1", videoFile));

    QStringList subtitleFiles = subtitle.file().split(";");
  
    writeSpumuxXml();

    KProcess *spumux = process("INFO: \\d+ bytes of data written", KProcess::OnlyStderrChannel);
    *spumux << "spumux" << "-P" << subtitleXmlFile;
    spumux->setStandardInputFile(videoFile);
    spumux->setStandardOutputFile(videoFileWithSubtitles);
    spumux->setWorkingDirectory(mediaDir);

    QFileInfo info(videoFile);
    setMaximum(info.size() / 1024);
    lastUpdate = 0;
    half = info.size() / 200;
    spumux->execute();

    if(spumux->exitCode() == QProcess::NormalExit && spumux->exitStatus() == 0)
    {
      setValue(info.size() / 1024);
    }
    else
    {
      QFile::remove(videoFileWithSubtitles);
      message(KMF::Error, i18n("   Conversion error."));
    }
  }

  void writeSpumuxXml()
  {
    QDomDocument doc("");
    QDomElement root = doc.createElement("subpictures");
    QDomElement stream = doc.createElement("stream");
    QDomElement textsub = doc.createElement("textsub");
  
    textsub.setAttribute("filename", subtitleFile);
    textsub.setAttribute("vertical-alignment", subtitle.verticalAlign());
    textsub.setAttribute("horizontal-alignment", subtitle.horizontalAlign());
    textsub.setAttribute("left-margin", 40);
    textsub.setAttribute("right-margin", 40);
    textsub.setAttribute("top-margin", 30);
    textsub.setAttribute("bottom-margin", 40);
    textsub.setAttribute("movie-width", "720");

    if(type == "DVD-PAL")
    {
      textsub.setAttribute("movie-fps", "25");
      textsub.setAttribute("movie-height", "574");
    }
    else
    {
      textsub.setAttribute("movie-fps", "29.97");
      textsub.setAttribute("movie-height", "478");
    }
  
    QFont font(subtitle.font());
    QString fontFile = KMF::Tools::fontFile(font);
    if(!fontFile.isEmpty())
      textsub.setAttribute("font", checkFontFile(fontFile));
    if(subtitle.font().pointSize() > 0)
      textsub.setAttribute("fontsize", subtitle.font().pointSize());
  
    stream.appendChild(textsub);
    root.appendChild(stream);
    doc.appendChild(root);

    // Write spumux xml
    QString s;
    QFile file(subtitleXmlFile);
    if (file.open(QIODevice::WriteOnly))
    {
      QTextStream stream(&file);
      doc.save(stream, 1);
      file.close();
    }
  }

  void output(const QString& line)
  {
    QRegExp bytes("INFO: (\\d+) bytes of data written");
  
    if(bytes.indexIn(line) > -1)
    {
      qulonglong temp = bytes.cap(1).toULongLong();
      if(temp - lastUpdate > half)
      {
        setValue(temp / 1024);
        lastUpdate = temp;
      }
    }
  }

  QString checkFontFile(const QString& file)
  {
    QFileInfo fi(file);
    QDir dir(QDir::home().filePath(".spumux"));
    QFileInfo link(dir.absoluteFilePath(fi.fileName()));
  
    if(!dir.exists())
      dir.mkdir(dir.path());
    //kDebug() << link.filePath() << " -> " << file;
    if(!link.exists())
      if(symlink(file.toLocal8Bit(), link.filePath().toLocal8Bit()) < 0)
        kDebug() << strerror(errno);
    return fi.fileName();
  }

private:
  qulonglong lastUpdate;
  qulonglong half;
};

VideoObject::VideoObject(QObject* parent)
  : MediaObject(parent), m_videoPlay(0),
    m_aspect(QDVD::VideoTrack::Aspect_Unknown), m_spumux(0)
{
  setObjectName("video");
  m_videoProperties = new KAction(KIcon("pencil"), i18n("&Properties"), this);
  plugin()->actionCollection()->addAction("video", m_videoProperties);
  connect(m_videoProperties, SIGNAL(triggered()), SLOT(slotProperties()));

  m_kmfplayer = KStandardDirs::findExe("kmediafactoryplayer");
  if(!m_kmfplayer.isEmpty())
  {
    m_videoPlay = new KAction(KIcon("media-playback-start"),
                              i18n("&Play Video"), this);
    m_videoPlay->setShortcut(Qt::CTRL + Qt::Key_P);
    plugin()->actionCollection()->addAction("mob_play", m_videoPlay);
    connect(m_videoPlay, SIGNAL(triggered()), SLOT(slotPlayVideo()));
  }
}

VideoObject::~VideoObject()
{
}

double VideoObject::frameRate() const
{
  return KMFMediaFile::mediaFile(m_files[0]).frameRate();
}

QTime VideoObject::duration() const
{
  KMF::Time result;

  for(QStringList::ConstIterator it = m_files.begin();
      it != m_files.end(); ++it)
  {
    result += duration(*it);
  }
  return result;
}

QTime VideoObject::duration(QString file) const
{
  return KMFMediaFile::mediaFile(file).duration();
}

QTime VideoObject::chapterTime(int chap) const
{
  return chapter(chap).start();
}

QStringList VideoObject::files() const
{
  return m_files;
}

QString VideoObject::fileName() const
{
  return m_files.first();
}

void VideoObject::actions(QList<QAction*>* actionList) const
{
  if(m_videoPlay)
    actionList->append(m_videoPlay);
  actionList->append(m_videoProperties);
}

bool VideoObject::fromXML(const QDomElement& element)
{
  m_cells.clear();
  m_audioTracks.clear();
  QDomNode n = element.firstChild();
  bool parseLengths = false;

  while(!n.isNull())
  {
    QDomElement e = n.toElement();
    if(!e.isNull())
    {
      if(e.tagName() == "video")
      {
        m_id = e.attribute("id");
        setTitle(e.attribute("title"));
        setPreviewUrl(e.attribute("custom_preview"));
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
              m_files.append(e2.attribute("path"));
            }
            else if(e2.tagName() == "cell")
            {
              QDVD::Cell cell;
              KMF::Time start(e2.attribute("start"));
              KMF::Time length(e2.attribute("length"));
              QString name = e2.attribute("name");
              bool chapter = (e2.attribute("chapter") == "1");

              if(!e2.hasAttribute("length"))
                parseLengths = true;
              int file = e2.attribute("file", 0).toInt();
              if(file > 1)
              {
                for(int i= 0; i < file - 1 && i < m_files.count(); ++i)
                  start += duration(m_files[i]);
              }
              //kDebug() << file  << ", " << start
              //    << ", " << chapter;
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
              QFont font;

              QDomNode m2 = e2.firstChild();
              while(!m2.isNull())
              {
                QDomElement e3 = m2.toElement();
                if(!e3.isNull())
                {
                  if(e3.tagName() == "font")
                    font = KMF::Tools::fontFromXML(e3);
                }
                m2 = m2.nextSibling();
              }
              QDVD::Subtitle s;
              s.setFile(e2.attribute("file"));
              s.setLanguage(e2.attribute("language",
                             VideoPluginSettings::defaultSubtitleLanguage()));
              QString a = e2.attribute("align",
                  QString("%1").arg(Qt::AlignHCenter | Qt::AlignBottom));
              s.setAlignment(QFlags<Qt::AlignmentFlag>(a.toInt()));
              s.setFont(font);
              addSubtitle(s);
            }
          }
          m = m.nextSibling();
        }
      }
    }
    n = n.nextSibling();
  }
  if(parseLengths)
    parseCellLengths();
  return checkObjectParams();
}

bool VideoObject::checkObjectParams()
{
  //kDebug() << VideoPluginSettings::defaultAudioLanguage();
  if(m_files.count() > 0)
  {
    const KMFMediaFile& media = KMFMediaFile::mediaFile(m_files[0]);

    while(m_audioTracks.count() < (int)media.audioStreams())
    {
      addAudioTrack(
          QDVD::AudioTrack(VideoPluginSettings::defaultAudioLanguage()));
    }
    if(m_cells.count() < 1)
      setCellSecs(900.0);
    if(m_id.isEmpty())
      generateId();
    if(title().isEmpty())
      setTitleFromFileName();
    if(m_aspect == QDVD::VideoTrack::Aspect_Unknown)
      m_aspect = media.aspectRatio();
    return true;
  }
  return false;
}

void VideoObject::toXML(QDomElement* element) const
{
  QDomDocument doc = element->ownerDocument();
  QDomElement video = doc.createElement("video");
  video.setAttribute("title", title());
  video.setAttribute("aspect", (int)m_aspect);
  video.setAttribute("id", m_id);

  if(m_previewUrl.isValid())
    video.setAttribute("custom_preview", m_previewUrl.prettyUrl());

  for(QStringList::ConstIterator it = m_files.begin();
      it != m_files.end(); ++it)
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
    KMF::Tools::fontToXML((*it).font(), &e2);
    e.appendChild(e2);
    video.appendChild(e);
  }
  element->appendChild(video);
}

QVariant VideoObject::writeDvdAuthorXml(QVariantList args) const
{
  QDomDocument doc;
  QString preferredLanguage = args[0].toString();

  QDir dir(interface()->projectDir("media"));
  int audioTrack = 0; // First audio track
  int subTrack = 62; // Let player decide
  bool audioFound = false;
  bool subFound = false;
  int i;

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

  // Add cells
  KMF::Time pos;
  bool open = false;
  QDomElement vob;
  QDVD::Cell cell;
  int file = 0;

  for(i = 0; i <= m_cells.count(); ++i)
  {
    if(!open)
    {
      vob = doc.createElement("vob");
      if(m_type != "dummy")
      {
        vob.setAttribute("file", videoFileFind(i));
      }
      else
      {
        vob.setAttribute("file", dir.filePath("dummy.mpg"));
      }
      open = true;
    }
    if(KMF::Time(cell.start()) > pos + duration(m_files[file]) ||
       i >= m_cells.count())
    {
      pos += duration(m_files[file]);
      pgc.appendChild(vob);
      if(i >= m_files.count() || i >= m_cells.count())
        break;
      open = false;
      ++file;
    }

    cell = m_cells[i];
    KMF::Time start(cell.start());
    KMF::Time end(cell.start());

    start -= pos;
    QDomElement c = vob.ownerDocument().createElement("cell");

    c.setAttribute("start", KMF::Time(start).toString());
    if(cell.length() == KMF::Time())
      c.setAttribute("end", "-1");
    else
    {
      end -= pos;
      end += cell.length();
      c.setAttribute("end", end.toString());
    }
    c.setAttribute("chapter", cell.isChapter());
    vob.appendChild(c);
    //kDebug() << "Cell: " << start << ", " << end;
  }
  titles.appendChild(pgc);
  
  QVariant result;
  result.setValue(titles);
  return result;
}

int VideoObject::timeEstimate() const
{
  return TotalPoints;
}

QString VideoObject::videoFileName(int index, VideoFilePrefix prefix)
{
  QDir dir(interface()->projectDir("media"));
  QString file = QFileInfo(m_files[index]).fileName();

  /*
  kDebug() << dir.filePath(QString("%1_%2")
      .arg(QString::number(index+1).rightJustify(3, '0'))
      .arg(file) + m_prefixes[prefix]);
  */
  return dir.filePath(QString("%1_%2")
      .arg(QString::number(index+1).rightJustified(3, '0'))
      .arg(file) + m_prefixes[prefix]);
}

QString VideoObject::videoFileFind(int index, VideoFilePrefix prefixStart) const
{
  QDir dir(interface()->projectDir("media"));
  QString file = QFileInfo(m_files[index]).fileName();

  for(int i = prefixStart; i < PrefixEmpty; ++i)
  {
    QString s = dir.filePath(QString("%1_%2")
        .arg(QString::number(index+1).rightJustified(3, '0'))
        .arg(file) + m_prefixes[i]);
    QFileInfo fi(s);
    if(fi.exists())
    {
      //kDebug() << s;
      return s;
    }
  }
  //kDebug() << m_files()[index].fileName();
  return m_files[index];
}

bool VideoObject::make(QString type)
{
  interface()->message(KMF::Info, i18n("Preparing file(s) for %1", title()));
  QString fileName;

  m_type = type;
  if (type != "dummy") 
  {
    foreach(const QDVD::Subtitle& subtitle, m_subtitles) 
    {
      if(!subtitle.file().isEmpty())
      {
        QStringList subtitleFiles = subtitle.file().split(";");

        for(int i = 0; i < m_files.count(); ++i)
        {
          if(i >= subtitleFiles.count())
            break;
          QFileInfo subtitleFile(subtitleFiles[i]);
          QFileInfo videoFile(videoFileFind(i, PrefixMpg));
          QFileInfo videoFileWithSubtitles(videoFileName(i, PrefixSub));

          if(!videoFileWithSubtitles.exists() ||
              videoFile.lastModified() > videoFileWithSubtitles.lastModified() ||
              subtitleFile.lastModified() > videoFileWithSubtitles.lastModified())
          {
            ConvertSubtitlesJob *job = new ConvertSubtitlesJob();
            job->subtitle = subtitle;
            job->subtitleFile = subtitleFile.fileName();
            job->subtitleXmlFile = videoFileName(i, PrefixXml);
            job->videoFile = videoFile.fileName();
            job->videoFileWithSubtitles = videoFileWithSubtitles.fileName();
            job->mediaDir = interface()->projectDir("media");
            job->type = interface()->projectType();
            interface()->addJob(job);
          }
          else
          {
            interface()->message(KMF::Info,
                i18n("   Subtitle conversion seems to be up to date: %1")
                    .arg(videoFile.fileName()));
          }
        }
      }
    }
  }
  return true;
}

void VideoObject::slotProperties()
{
  VideoOptions dlg(kapp->activeWindow());
  dlg.setData(*this);
  if (dlg.exec())
  {
    dlg.getData(*this);
    interface()->setDirty(KMF::DirtyMedia);
  }
}

QPixmap VideoObject::pixmap() const
{
  return KIO::pixmapForUrl(fileName());
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

QImage VideoObject::getFrame(QTime time, QString file) const
{
  bool ok = false;
  KMF::Time t = time;

  for(QStringList::ConstIterator it = m_files.begin();
      it != m_files.end(); ++it)
  {
    //kDebug() << *it;
    const KMFMediaFile& media = KMFMediaFile::mediaFile(*it);
    if(t <= KMF::Time(media.duration()))
    {
      media.frame(t, file);
      ok = true;
      break;
    }
    else
    {
      t -= media.duration();
    }
  }
  if(ok)
    return QImage(file);
  else
    return QImage();
}

QImage VideoObject::preview(int chap) const
{
  bool black = true;
  int counter;
  QImage img;
  QString cacheFile;

  if (chap == MainPreview)
  {
    if(m_previewUrl.isValid())
    {
      img.load(m_previewUrl.path());
      return img;
    }
  }

  KMF::Time t = chapter(chap).start();
  QDir dir(interface()->projectDir("media"));
  QString s;
  cacheFile = dir.filePath(s.sprintf("%s_%s.pnm",
      (const char*)m_id.toLocal8Bit(),
      (const char*)t.toString().toLocal8Bit()));
  if(img.load(cacheFile))
    return img;

  counter = 0;
  while(black && counter < 60)
  {
    img = getFrame(t, cacheFile);
    black = isBlack(img);
    if(black)
      kDebug() << "Black frame: " << t.toString();
    t += VideoPluginSettings::blackFrameJump();
    ++counter;
  }

  QSize templateRatio = QSize(4, 3);
  QSize videoRatio = (aspect() == QDVD::VideoTrack::Aspect_4_3) ?
                          QSize(4, 3) : QSize(16, 9);
  QSize imageRatio = KMF::Tools::guessRatio(img.size(), videoRatio);
  QSize templateSize = KMF::Tools::maxResolution(interface()->projectType());
  QSize imageSize = img.size();
  QSize res = KMF::Tools::resolution(imageSize, imageRatio,
                                     templateSize, templateRatio);

  img = img.scaled(res, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  /*
  kDebug() << "Template size: " << templateSize;
  kDebug() << "Template ratio:" << templateRatio;
  kDebug() << "Video ratio:" << videoRatio;
  kDebug() << "Image size:" << imageSize;
  kDebug() << "Image ratio:" << imageRatio;
  kDebug() << "Final resolution:" << res;
  */
  if(!VideoPluginSettings::usePreviewCache())
    QFile::remove(cacheFile);
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
        i18n("Chapter %1", i + 1));
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
  int serial = interface()->serial();
  QString name = KMF::Tools::simpleBaseName(m_files.first());
  m_id.sprintf("%3.3d_%s", serial, (const char*)name.toLocal8Bit());
}

bool VideoObject::addFile(QString fileName)
{
  m_files.append(fileName);
  checkObjectParams();
  return true;
}

void VideoObject::setTitleFromFileName()
{
  QString name = KUrl(fileName()).fileName();
  setTitle(KMF::Tools::simple2Title(name));
}

void VideoObject::slotPlayVideo()
{
  KRun::runUrl(KUrl(fileName()), "video/mpeg", qApp->activeWindow());
}

void VideoObject::printCells()
{
  KMF::Time next;
  int i = 1;

  for(QDVD::CellList::Iterator it = m_cells.begin();
      it != m_cells.end(); ++it, ++i)
  {
    kDebug() << i << ": " << (*it).start()
        << " / " << (*it).length();
  }
}

void VideoObject::parseCellLengths()
{
  KMF::Time next;

  for(QDVD::CellList::Iterator it = m_cells.begin();
      it != m_cells.end(); ++it)
  {
    //kDebug() << (*it).start();
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

  for(QStringList::ConstIterator it = m_files.begin();
      it != m_files.end(); ++it, ++i)
  {
    KFileItem finfo(KFileItem::Unknown, KFileItem::Unknown, KUrl(*it));
    total += finfo.size();
  }
  return total;
}

bool VideoObject::isDVDCompatible() const
{
  return KMFMediaFile::mediaFile(m_files[0]).dvdCompatible();
}

QMap<QString, QString> VideoObject::subTypes() const
{
  QMap<QString, QString> result;

  result[""] = i18n("Make DVD");
  result["dummy"] = i18n("Make preview DVD with dummy videos");
  return result;
}

#include "videoobject.moc"
