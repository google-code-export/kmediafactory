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
#include "kmfmenupage.h"
#include "kmfbutton.h"
#include "kmfgeometry.h"
#include "kmfmenu.h"
#include "kmftime.h"
#include "kmftools.h"
#include "templateobject.h"
#include "templatepluginsettings.h"
#include <kmediafactory/projectinterface.h>
#include <kapplication.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kdebug.h>
#include <kprocio.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <QVariant>
#include <QObject>
#include <QImage>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QRegExp>
#include <QTextStream>
#include <sys/types.h>
#include <sys/stat.h>


int KMFMenuPage::m_mjpegtoolsVersion = -1;

KMFMenuPage::KMFMenuPage(QObject *parent) :
  KMFWidget(parent), m_language("en"), m_modifiedLayers(0), m_titles(0),
  m_chapters(0), m_titleStart(0), m_chapterStart(0), m_index(0), m_titleset(0),
  m_titlesetCount(0), m_count(0), m_vmgm(false),
  m_directPlay(false), m_directChapterPlay(false), m_continueToNextTitle(true)
{
  m_buttons = new QList<KMFButton*>;
  setResolution(KMF::Tools::maxResolution(m_prjIf->type()));
}

KMFMenuPage::~KMFMenuPage()
{
  qDeleteAll(*m_buttons);
  delete m_buttons;
}

void KMFMenuPage::fromXML(const QDomElement& element)
{
  KMFWidget::fromXML(element);
  m_titles = element.attribute("titles", "0").toInt();
  m_chapters = element.attribute("chapters", "0").toInt();
  QString s;
  if(m_titles > 0)
    s = QString("%1").arg((m_titleStart / m_titles) +1);
  else if(m_chapters > 0)
    s = QString("%1_%2").arg(m_titleStart+1)
                        .arg((m_chapterStart / m_chapters) +1);
  else if(m_chapters > 0)
    s = QString("%1_%2").arg(m_titleStart+1)
                        .arg(m_chapterStart+1);
  QString n = QString("%1_%2").arg(objectName()).arg(s);
  setObjectName(n);
}

void KMFMenuPage::setResolution(QSize resolution)
{
#warning TODO
#if 0
  KMFUnit::setMaxRes(resolution);
  m_resolution = resolution;
  QString size =
      QString("%1x%2").arg(resolution.width()).arg(resolution.height());
  m_background.size((const char*)size.local8Bit());
  m_background.read("xc:#44444400");
  m_sub.size((const char*)size.local8Bit());
  m_sub.read("xc:#000000FF");
  m_subHighlight.size((const char*)size.local8Bit());
  m_subHighlight.read("xc:#000000FF");
  m_subSelect.size((const char*)size.local8Bit());
  m_subSelect.read("xc:#000000FF");
  m_temp.size((const char*)size.local8Bit());
  geometry().left().set(0, KMFUnit::Absolute);
  geometry().top().set(0, KMFUnit::Absolute);
  geometry().width().set(resolution.width(), KMFUnit::Absolute);
  geometry().height().set(resolution.height(), KMFUnit::Absolute);
#endif
}

bool KMFMenuPage::writeSpumuxXml(QDomDocument& doc)
{
  QDomElement root = doc.createElement("subpictures");
  QDomElement stream = doc.createElement("stream");
  QDomElement spu = doc.createElement("spu");

  spu.setAttribute("end", "00:00:00.00");
  if(m_modifiedLayers & Sub)
    spu.setAttribute("image", QString("%1_sub.png").arg(objectName()));
  if(m_modifiedLayers & Highlight)
    spu.setAttribute("highlight",
                     QString("%1_highlight.png").arg(objectName()));
  if(m_modifiedLayers & Select)
    spu.setAttribute("select",
                     QString("%1_select.png").arg(objectName()));
  spu.setAttribute("force", "yes");

  // We can't search and loop in the same list concurrently
  QList<KMFButton*> temp = *m_buttons;
  foreach(KMFButton *btn, temp)
  {
    if(btn->isHidden())
      continue;
    // TODO: Button class should do this
    /*QDomComment comment =
        doc.createComment(QString("Button: %1").arg(btn->objectName()));
    spu.appendChild(comment);*/
    QDomElement button = doc.createElement("button");
    button.setAttribute("name", btn->objectName());
    QRect rc = btn->paintRect();
    //kdDebug() << rc << endl;
    button.setAttribute("x0", rc.left());
    button.setAttribute("y0", rc.top());
    button.setAttribute("x1", rc.right());
    button.setAttribute("y1", rc.bottom());
    if(btn->down())
      button.setAttribute("down", btn->down()->objectName());
    if(btn->up())
      button.setAttribute("up", btn->up()->objectName());
    if(btn->right())
      button.setAttribute("right", btn->right()->objectName());
    if(btn->left())
      button.setAttribute("left", btn->left()->objectName());
    spu.appendChild(button);
  }
  stream.appendChild(spu);
  root.appendChild(stream);
  doc.appendChild(root);
  return true;
}

bool KMFMenuPage::parseButtons(bool addPages)
{
  //kdDebug() << k_funcinfo << endl;
  foreach(KMFButton* btn, *m_buttons)
    if(btn->parseJump(addPages) == false)
      return false;
  foreach(KMFButton* btn, *m_buttons)
    btn->parseDirections();
  return true;
}

void KMFMenuPage::writeDvdAuthorXml(QDomElement& element, QString type)
{
  QDomDocument doc = element.ownerDocument();
  QDomElement pgc = doc.createElement("pgc");
  QDomElement pre = doc.createElement("pre");
  QDomText text = doc.createTextNode("");

  checkDummyVideo();
  if(isVmgm())
  {
    if(isFirst())
      pgc.setAttribute("entry", "title");

    QString s = "\n    {\n";
    // Direct play
    if(isFirst())
    {
      s += "      if (g3 gt 0)\n"
           "      {\n"
           "        g7 = g3;\n"
           "        g3 = 0;\n"
           "        g1 = 1;\n";
      for(int i = 1; i <= m_titlesetCount; ++i)
      {
        s += QString("        if(g7 eq %1)\n").arg(i);
        s += "        {\n";
        s += QString("          jump titleset %1 menu; \n").arg(i);
        s += "        }\n";
      }
      s += "      }\n";
    }

    // Jump to saved page if necessary
    if(isFirst())
      for(int i = 2; i <= m_count; ++i)
        s += QString("      if (g5 eq %1)\n"
                     "        jump menu %2;\n").arg(i).arg(i);

    // Highlight saved button
    s += "      if (g2 lt 1024)\n"
         "        button = 1024;\n"
         "      else\n"
         "        button = g2;\n";
    // Reset titleset page index
    s += QString("      g6 = 0;\n");
    // Reset titleset page button
    s += QString("      g4 = 0;\n");
    // Reset played chapter
    s += QString("      g0 = 0;\n");
    // Close code
    s += "    }\n    ";
    text.setData(s);
  }
  else
  {
    if(isFirst())
      pgc.setAttribute("entry", "root");

    QString s = "\n    {\n";
    // If chapter specified jump there
    if(isFirst())
      s += "      if (g1 gt 0)\n"
           "      {\n"
           "        g0 = g1;\n"
           "        g1 = 0;\n"
           "        jump title 1 chapter 1;\n"
           "      }\n";

    // Jump to saved page if necessary
    if(isFirst())
      for(int i = 2; i <= m_count; ++i)
        s += QString("      if (g6 eq %1)\n"
                     "        jump menu %1;\n").arg(i).arg(i);

    // Highlight saved button
    s += "      if (g4 lt 1024)\n"
         "        button = 1024;\n"
         "      else\n"
         "        button = g4;\n";
    // Close code
    s += "    }\n    ";
    text.setData(s);
  }
  pre.appendChild(text);
  pgc.appendChild(pre);

  pgc.setAttribute("pause", "0");
  QDomElement vob = doc.createElement("vob");
  QString file = QString("./menus/%1.mpg").arg(objectName());
  vob.setAttribute("file", file);
  vob.setAttribute("pause", "inf");
  pgc.appendChild(vob);

  foreach(KMFButton* btn, *m_buttons)
  {
    if(btn->isHidden())
      continue;
    QDomElement button = doc.createElement("button");
    button.setAttribute("name", btn->objectName());
    QDomText text = doc.createTextNode("");
    btn->writeDvdAuthorXml(text, type);
    button.appendChild(text);
    pgc.appendChild(button);
  }
  element.appendChild(pgc);
}

void KMFMenuPage::checkDummyVideo()
{
  QFileInfo fi(m_prjIf->projectDir("media") + "dummy.mpg");

  if(!fi.exists())
  {
#warning TODO
#if 0
    QImage temp;
    QString size;
    if(m_prjIf->type() == "DVD-PAL")
      temp.size("720x576");
    else
      temp.size("720x480");
    temp.read("xc:#00000000");
    temp.depth(8);
    temp.type(Magick::TrueColorType);
    saveImage(temp, m_prjIf->projectDir("media") + "dummy.pnm");
    m_sound = "";
    runScript("dummy", "media");
#endif
  }
}

void KMFMenuPage::writeDvdAuthorXmlNoMenu(QDomElement& element)
{
  QDomDocument doc = element.ownerDocument();
  QDomElement pgc = doc.createElement("pgc");
  QDomElement pre = doc.createElement("pre");
  QDomText text = doc.createTextNode("");

  checkDummyVideo();
  pgc.setAttribute("entry", "root");

  QString s = "\n    {\n";
  // If chapter specified jump there
  s += "      if (g1 gt 0)\n"
      "      {\n"
      "        g1 = 0;\n"
      "        jump title 1 chapter 1;\n"
      "      }\n"
      "      else\n"
      "      {\n"
      "        jump vmgm menu ;\n"
      "      }\n";
  // Close code
  s += "    }\n    ";

  text.setData(s);
  pre.appendChild(text);
  pgc.appendChild(pre);

  pgc.setAttribute("pause", "0");
  QDomElement vob = doc.createElement("vob");
  QString file = "./media/dummy.mpg";
  vob.setAttribute("file", file);
  vob.setAttribute("pause", "inf");
  pgc.appendChild(vob);
  element.appendChild(pgc);
}

bool KMFMenuPage::saveImage(QImage& image, const QString& file)
{
  return image.save(file);
}

bool KMFMenuPage::saveImages()
{
  QString file;
  QString menus = m_prjIf->projectDir("menus");

  // Save subpicture files
  if(m_modifiedLayers & Sub)
  {
    //m_sub.quantizeColorSpace(Magick::TransparentColorspace);
    //m_sub.quantizeDither(true);
    //m_sub.quantizeColors(4);
    //m_sub.quantize();
    file = menus + QString("%1_sub.png").arg(objectName());
    saveImage(m_sub, file);
  }

  if(m_modifiedLayers & Highlight)
  {
    //m_subHighlight.quantizeColorSpace(Magick::TransparentColorspace);
    //m_subHighlight.quantizeDither(true);
    //m_subHighlight.quantizeColors(4);
    //m_subHighlight.quantize();
    file = menus + QString("%1_highlight.png").arg(objectName());
    saveImage(m_subHighlight, file);
  }

  if(m_modifiedLayers & Select)
  {
    //m_subSelect.quantizeColorSpace(Magick::TransparentColorspace);
    //m_subSelect.quantizeDither(true);
    //m_subSelect.quantizeColors(4);
    //m_subSelect.quantize();
    file = menus + QString("%1_select.png").arg(objectName());
    saveImage(m_subSelect, file);
  }

#warning TODO
#if 0
  file = menus + QString("%1.pnm").arg(objectName());
  // 8 bits per channle for ppmtoy4m
  m_background.depth(8);
  m_background.type(Magick::TrueColorType);
#endif
  return saveImage(m_background, file);
}

bool KMFMenuPage::writeSpumuxXml()
{
  // Write spumux xml
  QString s;
  QDomDocument doc("");
  if(writeSpumuxXml(doc) == false)
    return false;

  QFile file(m_prjIf->projectDir("menus") + objectName() + ".xml");
  if (!file.open(QIODevice::WriteOnly))
    return false;
  QTextStream stream(&file);
  stream << doc.toString();
  file.close();
  return true;
}

#define MAKE_VERSION(A, B, C, D) (((A) << 24) | ((B) << 16) | \
                                  ((C) << 8)  | ((D)<< 0))

int KMFMenuPage::makeVersion(KProcIO& proc)
{
  QRegExp rx("(\\d+)\\.(\\d+)\\.(\\d+)[.\\-_a-z]*(\\d*)");

  if(proc.start(KProcess::Block, true) == true)
  {
    QString v;
    proc.readln(v);
    v = v.trimmed();
    if(rx.indexIn(v) != -1)
    {
      kDebug() << k_funcinfo
          << rx.cap(1).toInt() << "."
          << rx.cap(2).toInt() << "."
          << rx.cap(3).toInt() << "."
          << rx.cap(4).toInt() << endl;
      return MAKE_VERSION(rx.cap(1).toInt(), rx.cap(2).toInt(),
                          rx.cap(3).toInt(), rx.cap(4).toInt());
    }
  }
  return -1;
}

int KMFMenuPage::mjpegtoolsVersion()
{
  if(m_mjpegtoolsVersion == -1)
  {
    KProcIO pkgconfig;

    pkgconfig << "pkg-config" << "mjpegtools" << "--modversion";
    m_mjpegtoolsVersion = makeVersion(pkgconfig);
    if(m_mjpegtoolsVersion == -1)
    {
      KProcIO mplex;
      mplex << "mplex";
      m_mjpegtoolsVersion = makeVersion(mplex);
    }
    if(m_mjpegtoolsVersion == -1)
    {
      kDebug() << "Can't determine mjpegtools version." << endl;
      m_mjpegtoolsVersion = MAKE_VERSION(0, 0, 0, 0);
    }
  }
  return m_mjpegtoolsVersion;
}

bool KMFMenuPage::runScript(QString scriptName, QString place)
{
  QString menuSound = "silence.mp2";
  KMF::Time seconds = 1.0;

#warning TODO
#if 0
  if(!m_sound.isEmpty())
  {
    m_converter = new QFFMpeg;

    m_converter->addFile(m_sound);
    seconds = m_converter->duration();

    if(!m_converter->isDVDCompatible())
    {
      QFileInfo sound(m_sound);
      QFileInfo fi(m_prjIf->projectDir(place) +
          QString("%1.ac3").arg(sound.fileName()));

      if(fi.lastModified() < m_converter->lastModified())
      {
        connect(m_converter, SIGNAL(convertProgress(int)),
                this, SLOT(slotProgress(int)));
        connect(m_converter, SIGNAL(message(const QString&)),
                m_uiIf->logger(), SLOT(message(const QString&)));
        m_stopped = false;

        m_uiIf->message(KMF::Info,
            i18n("   Converting %1 to ac3 format").arg(sound.fileName()));
        m_uiIf->setItemTotalSteps(fi.size());

        QFFMpegConvertTo dvd;
        dvd.append(QFFMpegParam("ab", 192));
        dvd.append(QFFMpegParam("ar", 48000));
        if(m_converter->convertTo(dvd, 0, fi.filePath()) == false)
        {
          if(!m_stopped)
            m_uiIf->message(KMF::Error, i18n("   Conversion error."));
          QFile(fi.filePath()).remove();
        }
      }
      menuSound = fi.filePath();
    }
    else
      menuSound = m_sound;
    delete m_converter;
  }
  else
  {
    QFileInfo fi(m_prjIf->projectDir(place) + "silence.mp2");

    if(!fi.exists())
    {
      QFFmpegEncoder encoder;

      encoder.setOutput(QF_MP2);
      encoder.write(fi.filePath());
    }
  }
#endif

  // TODO: This is for converting pnm to mpg. This should be done with ffmpeg
  // but I don't know how or if ffmpeg can do menu mpegs for dvdauthor/spumux
  QFile file(m_prjIf->projectDir(place) +
             QString("%1.sh").arg(scriptName));
  QString videoNorm;
  QString aspectRatio;
  QString frameRate;
  uint frameRateMplex;
  uint frames;

  if(m_prjIf->type() == "DVD-PAL")
  {
    videoNorm = "p";
    frameRate = "25:1";
    frameRateMplex = 3;
    frames = (uint)(seconds.toSeconds() * 25.0);
    aspectRatio = "59:54";
  }
  else // NTSC
  {
    videoNorm = "n";
    frameRate = "30000:1001";
    frameRateMplex = 4;
    frames = (uint)(seconds.toSeconds() * 30000.0 / 1001.0);
    aspectRatio = "10:11";
  }

  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    m_uiIf->message(KMF::Error,
        i18n("   Script file open failed. %1")
        .arg(file.errorString()));
    return false;
  }

  QTextStream stream(&file);
  QString s;
  QFileInfo fi(m_prjIf->projectDir(place) +
      QString("%1.xml").arg(scriptName));

  if(mjpegtoolsVersion() > MAKE_VERSION(1, 6, 2, 0))
    s = "420mpeg2";
  else
    s = "420_mpeg2";
  stream << "#!/bin/sh\n" <<
      QString("ppmtoy4m -S %4 -n 1 -r -I p -F %1 -A %2 %3.pnm | ")
          .arg(frameRate).arg(aspectRatio).arg(scriptName).arg(s) <<
      QString("mpeg2enc -a 2 -f 8 -n %1 -F %2 -o %3.m2v && \\\n")
          .arg(videoNorm).arg(frameRateMplex).arg(scriptName);
  if(fi.exists())
  {
    stream <<
        QString("mplex -f 8 -o %2_bg.mpg %3.m2v \"%1\" && \\\n")
          .arg(menuSound).arg(scriptName).arg(scriptName) <<
        QString("spumux %1.xml < %2_bg.mpg > %3.mpg && \\\n")
          .arg(scriptName).arg(scriptName).arg(scriptName) <<
        QString("rm %1.m2v %2_bg.mpg\n")
          .arg(scriptName).arg(scriptName);
  }
  else
  {
    stream <<
        QString("mplex -f 8 -o %2.mpg %3.m2v \"%1\" && \\\n")
            .arg(menuSound).arg(scriptName).arg(scriptName) <<
        QString("rm %1.m2v\n")
            .arg(scriptName);
  }
  file.close();
  chmod((const char*)file.fileName().toLocal8Bit(),
      S_IRUSR|S_IWRITE|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH);

  KProcess pnm2mpg;
  pnm2mpg.setWorkingDirectory(m_prjIf->projectDir(place));
  pnm2mpg << "sh" << file.fileName();
  m_uiIf->logger()->connectProcess(&pnm2mpg);
  pnm2mpg.start(KProcess::Block, KProcess::AllOutput);
  if(!pnm2mpg.normalExit() || pnm2mpg.exitStatus() != 0)
  {
    kDebug() << k_funcinfo << pnm2mpg.exitStatus() << endl;
    m_uiIf->message(KMF::Error, i18n("   Conversion error."));
    return false;
  }
  return true;
}

bool KMFMenuPage::makeMpeg()
{
  QString file;

  QDir dir(m_prjIf->projectDir("menus"));

  if(paint() == false)
    return false;
  if(writeSpumuxXml() == false)
    return false;
  if(saveImages() == false)
    return false;
  if(runScript(objectName()) == false)
    return false;
  return true;
}

QImage& KMFMenuPage::layer(Layer layer)
{
  switch(layer)
  {
    case Temp:
      return m_temp;
    case Sub:
      return m_sub;
    case Highlight:
      return m_subHighlight;
    case Select:
      return m_subSelect;
    case None:
    case Background:
    default:
      return m_background;
  }
  return m_background;
}

bool KMFMenuPage::paint()
{
  m_modifiedLayers = 0;
  return paintChildWidgets(this);
}

bool KMFMenuPage::paintChildWidgets(QObject* parent)
{
#warning TODO
#if 0
  foreach(QObject *obj, parent->children())
  {
    if(obj->inherits("KMFWidget"))
    {
      KMFWidget* widget = static_cast<KMFWidget*>(obj);
      if(widget->isHidden())
        continue;
      if(TemplatePluginSettings::widgetDebug())
      {
        QRect paintRC = widget->paintRect();
        QRect rc = widget->rect();
        QString type = widget->metaObject()->className();
        QString color;
        std::list<Magick::Drawable> drawList;

        if(TemplatePluginSettings::widgetDebugInfo())
          kdDebug() << type << " (" << widget->objectName() << "): " << rc
              << " - " << paintRC << endl;

        if(type == "KMFImage" &&
            TemplatePluginSettings::widgetDebugImage())
        {
          color = "blue";
        }
        else if(type == "KMFLabel" &&
                TemplatePluginSettings::widgetDebugLabel())
        {
          color = "red";
        }
        else if(type == "KMFFrame" &&
                TemplatePluginSettings::widgetDebugFrame())
        {
          color = "green";
        }
        else if(type == "KMFButton" &&
                TemplatePluginSettings::widgetDebugButton())
        {
          color = "yellow";
        }
        else if((type == "KMFVBox" || type == "KMFHBox")
                  && TemplatePluginSettings::widgetDebugBox())
        {
          color = "white";
        }
        else if(type == "KMWidget" &&
                TemplatePluginSettings::widgetDebugWidget())
        {
          color = "gray";
        }
        if(!color.isEmpty())
        {
          drawList.push_back(Magick::DrawableFillColor(
                  (const char*)color.local8Bit()));
          drawList.push_back(Magick::DrawableStrokeColor(
                  (const char*)color.local8Bit()));
          drawList.push_back(Magick::DrawableStrokeWidth(1));
          if(TemplatePluginSettings::withMargin())
          {
            drawList.push_back(Magick::DrawableFillOpacity(0.1));
            drawList.push_back(Magick::DrawableRectangle(rc.left(),
                                                          rc.top(),
                                                          rc.right(),
                                                          rc.bottom()));
          }
          drawList.push_back(Magick::DrawableFillOpacity(0.3));
          drawList.push_back(Magick::DrawableRectangle(paintRC.left(),
                                                        paintRC.top(),
                                                        paintRC.right(),
                                                        paintRC.bottom()));
          m_background.draw(drawList);
        }
      }
      try
      {
        widget->paint(this);
      }
      catch(Magick::Exception &e)
      {
        magickReport(e, QString("%1 : %2, %3, %4, %5")
            .arg(widget->objectName())
            .arg(widget->paintRect().x())
            .arg(widget->paintRect().y())
            .arg(widget->paintRect().width())
            .arg(widget->paintRect().height()));
        return false;
      }
      m_modifiedLayers |= widget->layer();
      if(paintChildWidgets(obj) == false)
        return false;
      // Just to make ui more responsible
      if(m_uiIf->progress(0))
        return false;
    }
  }
  return true;
#endif
}

KMFButton* KMFMenuPage::button(const QString& name)
{
  // We can't search and loop in the same list concurrently
  QList<KMFButton*> temp = *m_buttons;
  foreach(KMFButton *btn, temp)
  {
    if(!btn->isHidden() && btn->objectName() == name)
      return btn;
  }
  return 0;
}

void KMFMenuPage::setProperty(const QString& name, QVariant value)
{
  KMFWidget::setProperty(name, value);
  if(name == "language")
  {
    //kdDebug() << k_funcinfo << "Setting language" << endl;
    m_language = value.toString();
    menu()->setLanguage("template", m_language);
  }
  else if(name == "direct_play")
    m_directPlay = value.toBool();
  else if(name == "direct_chapter_play")
    m_directChapterPlay = value.toBool();
  else if(name == "sound")
    m_sound = value.toString();
  else if(name == "continue_to_next_title")
    m_continueToNextTitle = value.toInt();
}

KMFWidget::Layer KMFMenuPage::layerType(const QImage& img)
{
  if(&img == &m_background)
    return Background;
  else if(&img == &m_sub)
    return Background;
  else if(&img == &m_subHighlight)
    return Highlight;
  else if(&img == &m_subSelect)
    return Select;
  else if(&img == &m_temp)
    return Temp;
  return None;
}

#include "kmfmenupage.moc"
