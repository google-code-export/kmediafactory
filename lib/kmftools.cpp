//**************************************************************************
//   Copyright (C) 2004-2008 by Petri Damsten
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

#include "kmftools.h"
#include <KXMLGUIClient>
#include <KActionCollection>
#include <KStringHandler>
#include <KStandardDirs>
#include <KDebug>
#include <KApplication>
#include <KSaveFile>
#include <KMessageBox>
#include <KTemporaryFile>
#include <KLocale>
#include <KIO/NetAccess>
#include <QAction>
#include <QFileInfo>
#include <QDir>
#include <QSet>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QFont>
#include <QDomElement>
#include <QAbstractItemView>
#include <QItemSelection>
#include <QStringListModel>
#include <QPainter>
#include <QDomElement>
#include <sys/stat.h>
#include <errno.h>
#include <fontconfig/fontconfig.h>
#include <Q4puGenericSignalSpy.h>

KMF::Tools::Tools()
{
}

KMF::Tools::~Tools()
{
}

QString KMF::Tools::toAscii(QString text)
{
  QString s = text;

  s.replace(QString::fromUtf8("\xc3\x85"), "A");
  s.replace(QString::fromUtf8("\xc3\x84"), "A");
  s.replace(QString::fromUtf8("\xc3\x96"), "O");
  s.replace(QString::fromUtf8("\xc3\x9c"), "U");
  s.replace(QString::fromUtf8("\xc3\xa5"), "a");
  s.replace(QString::fromUtf8("\xc3\xa4"), "a");
  s.replace(QString::fromUtf8("\xc3\xb6"), "o");
  s.replace(QString::fromUtf8("\xc3\xbc"), "u");
  return s;
}

QString KMF::Tools::simpleName(QString s)
{
  s.replace(' ', "_");
  s = toAscii(s);
  return s.toLower();
}

QString KMF::Tools::simple2Title(QString s)
{
  int i;

  s.replace('-', QChar::Nbsp);
  s.replace('_', QChar::Nbsp);
  i = s.lastIndexOf('.');
  if(i > 0)
    s = s.left(i);
  s = KStringHandler::capwords(s);
  return s;
}

QString KMF::Tools::simpleBaseName(QString file)
{
  QFileInfo info(file);
  QString s = info.baseName();

  return simpleName(s);
}

// from void KBuildImageIOFactory::save(QDataStream &str)

void KMF::Tools::removeDuplicates(QStringList* list)
{
  *list = list->toSet().toList();
}

QString KMF::Tools::sizeString(uint64_t size)
{
  const char prefixes[][2] = { "", "k", "M", "G", "T" };
  int n = 0;
  double s = size;
  int p = 0;

  while (s / 1024.0 > 1.0)
  {
    s /= 1024.0;
    ++n;
  }

  if(s >= 100.0 || n == 0)
    p = 0;
  else if(s >= 10.0)
    p = 1;
  else if(s >= 1.0)
    p = 2;
  else
    p = 3;
  return QString("%1 %2B").arg(s, 0, 'f', p).arg(prefixes[n]);
}

/* // Does not work on FreeBSD. Use KFileItem.size() instead
uint64_t KMF::Tools::fileSize(QString file)
{
  struct stat64 buf;

  int rc = stat64(file, &buf);
  if(rc)
  {
    // 2 = No such file or directory
    if (errno != 2)
      kDebug () << QString("stat(%1) error: %2").arg(file).arg(strerror(errno))
         ;
    return 0;
  }
  return buf.st_size;
}
*/

void KMF::Tools::stripExisting(KUrl::List* src, const KUrl& dest)
{
  KUrl::List::iterator it = src->begin();
  QDir dir(dest.path());

  while(it != src->end())
  {
    QFileInfo fi((*it).path());
    QFileInfo fiDest(dir.filePath(fi.fileName()));

    if(fiDest.exists())
      it = src->erase(it);
    else
      ++it;
  }
}

QString KMF::Tools::addSlash(QString path)
{
  int len = path.length();

  if(len > 0 && path[len - 1] == QDir::separator())
    return path;
  return path + QDir::separator();
}

QString KMF::Tools::joinPaths(QString path1, QString path2)
{
  QString path = addSlash(path1);
  int len = path2.length();

  if(len > 0 && path2[0] == QDir::separator())
    path += path2.mid(1);
  else
    path += path2;
  return path;
}

QString KMF::Tools::findExe(const QString& exe, const QStringList& paths,
                            const QString& extraPrefix)
{
  QString bin = KStandardDirs::findExe(exe);
  if(!bin.isEmpty())
    return bin;

  for(QStringList::ConstIterator it = paths.begin(); it != paths.end(); ++it)
  {
    QString file = joinPaths(*it, extraPrefix);
    file = joinPaths(file, exe);
    QFileInfo fi(file);

    if(fi.exists() && fi.isExecutable())
      return file;
  }
  return QString::null;
}

QStringList KMF::Tools::file2List(const QString& file,
                                  const QString& comment,
                                  const QString& startsWith)
{
  QStringList result;
  QFile f(file);
  QString line;

  if(f.open(QIODevice::ReadOnly))
  {
    QTextStream stream(&f);
    while(!stream.atEnd())
    {
      line = stream.readLine();
      if(!comment.isEmpty() && line.startsWith(comment))
        continue;
      if(!startsWith.isEmpty() && !line.startsWith(startsWith))
        continue;
      result.append(line);
    }
    f.close();
  }
  return result;
}

QStringList KMF::Tools::findAllResources(const char* type,
                                         const QString& filter)
{
  QStringList dirs = KGlobal::dirs()->resourceDirs(type);
  QStringList result, files;
  int n = filter.lastIndexOf(QDir::separator());
  QString d = filter.left(n + 1);
  QString f = filter.mid(n + 1);

  for(QStringList::ConstIterator it = dirs.begin(); it != dirs.end(); ++it)
  {
    QDir dir(*it + d);

    files = dir.entryList(QStringList(f));
    for(QStringList::ConstIterator jt = files.begin(); jt != files.end(); ++jt)
      result.append(dir.filePath(*jt));
  }
  return result;
}

QSize KMF::Tools::resolution(const QSize& original,
                             const QSize& originalRatio,
                             const QSize& dest,
                             const QSize& destRatio,
                             Qt::AspectRatioMode mode)
{
  double sourceRatio = ((double)originalRatio.width() /
                        (double)originalRatio.height()) /
                       ((double)original.width() /
                        (double)original.height());
  double destinationRatio = ((double)destRatio.width() /
                             (double)destRatio.height()) /
                            ((double)dest.width() /
                             (double)dest.height());
  double x, y;
  QSize result;

  if(original.width() == 0 || original.height() == 0)
    return QSize(0, 0);
  // Source to destination pixel ratio
  x = (double)original.width();
  y = (double)original.height() * destinationRatio / sourceRatio;
  // Scale
  result = QSize((int)x, (int)y);
  result.scale(dest, mode);
  return result;
}

QSize KMF::Tools::maxResolution(const QString &type)
{
  if(type == "DVD-PAL")
    return QSize(720, 576);
  else if(type == "DVD-NTSC")
    return QSize(720, 480);
  else
    return QSize(360, 240);
}

QSize KMF::Tools::guessRatio(const QSize& image, const QSize& video)
{
  // Common resolutions
  static int res[][2] = {
    // PAL VCD
    { 352, 288 },
    // PAL SVCD
    { 480, 576 },
    // PAL DVD
    { 720, 576 }, { 704, 576 }, { 544, 576 }, { 352, 576 }, { 352, 288 },
    // NTSC VCD
    { 352, 240 },
    // NTSC SVCD
    { 480, 480 },
    // NTSC DVD
    { 720, 480 }, { 352, 480 }, { 352, 240 }
  };

  for(uint i=0; i < 12; ++i)
  {
    if(image == QSize(res[i][0], res[i][1]))
      return video;
  }
  // Square pixels
  return image;
}

void KMF::Tools::fontToXML(const QFont& font, QDomElement* element)
{
  element->setAttribute("name", font.family());
  //element.setAttribute("color", m_color);
  element->setAttribute("size", font.pointSize());
  element->setAttribute("weight", font.weight() * 10);
}

QFont KMF::Tools::fontFromXML(const QDomElement& element)
{
  QFont f;

  f.setFamily(element.attribute("name", "Helvetica"));
  // Widget has color attribute
  //m_color = element.attribute("color", "0").toLong();
  f.setPointSize(element.attribute("size", "22").toInt());
  f.setWeight(element.attribute("weight", "400").toInt() / 10);
  //kDebug() << family() << pointSize() << weight();
  return f;
}

int KMF::Tools::fcWeight2QtWeight(int fcWeight)
{
  if(fcWeight <= FC_WEIGHT_LIGHT)
    return QFont::Light;
  else if(fcWeight >= FC_WEIGHT_BLACK)
    return QFont::Black;
  else if(fcWeight >= FC_WEIGHT_BOLD)
    return QFont::Bold;
  else if(fcWeight >= FC_WEIGHT_DEMIBOLD)
    return QFont::DemiBold;
  else
    return QFont::Normal;
}

QString KMF::Tools::longFontName(const QFont& font)
{
  QString result = QString("%1-%2-%3-%4")
      .arg(font.family()).arg(font.stretch())
      .arg(font.weight()).arg(font.italic());
  return result;
}

QFont KMF::Tools::realFont(const QFont& font)
{
  QFont result(font);
  QFontInfo fi(font);

  result.setFamily(fi.family());
  result.setWeight(fi.weight());
  result.setItalic(fi.italic());
  result.setPointSize(fi.pointSize());
  return result;
}

QString KMF::Tools::fontFile(const QFont& font)
{
  QString name = longFontName(realFont(font));
  static QMap<QString, QString> fileMap;

  if(fileMap.count() == 0)
  {
    FcObjectSet* os;
    FcPattern* pat;
    FcFontSet* fontset;
    int i;

    os = FcObjectSetBuild(FC_FAMILY, FC_FILE, FC_WEIGHT, FC_SLANT,
                          FC_WIDTH, NULL);
    pat = FcPatternCreate();
    fontset = FcFontList(NULL, pat, os);
    FcPatternDestroy(pat);
    FcObjectSetDestroy(os);

    for(i=0; i<fontset->nfont; i++)
    {
      QFont font;
      FcChar8* family;
      FcChar8* file;
      int weight;
      int slant;
      int width;

      FcPatternGetString(fontset->fonts[i], FC_FAMILY, 0, &family);
      FcPatternGetString(fontset->fonts[i], FC_FILE, 0, &file);
      FcPatternGetInteger(fontset->fonts[i], FC_WEIGHT, 0, &weight);
      FcPatternGetInteger(fontset->fonts[i], FC_SLANT, 0, &slant);
      FcPatternGetInteger(fontset->fonts[i], FC_WIDTH, 0, &width);
      font.setFamily((const char*)family);
      font.setWeight(fcWeight2QtWeight(weight));
      font.setItalic(slant >= FC_SLANT_ITALIC);
      if(width < QFont::UltraCondensed || width > QFont::UltraExpanded)
        width = QFont::Unstretched;
      font.setStretch(width);
      /*
      if(QString((const char*)family).startsWith("Bit"))
      kDebug() << (const char*)family << ", " << weight << ", "
      << slant << ", " << width << ", " << (const char*)file
      << font.longName();
      */
      fileMap[longFontName(font)] = (const char*)file;

      // Font names with '-' cause mapping problems. Qt mangles them?
      QString s = (const char*)family;
      s.replace('-', " ");
      if(s != (const char*)family)
      {
        font.setFamily(s);
        //kDebug() << font.longName();
        fileMap[longFontName(font)] = (const char*)file;
      }
    }
    FcFontSetDestroy (fontset);
  }
  /*
  // Write font list for debugging
  QFile file(QDir::homePath() + "/.spumux/fonts.txt");
  if(file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QTextStream out(&file);
    QMap<QString, QString>::const_iterator i = fileMap.constBegin();

    out << name << endl << endl;
    while (i != fileMap.constEnd())
    {
      out << i.key() << ": " << i.value() << endl;
      ++i;
    }
  }
  // End of font list
  */
  return fileMap[name];
}

void KMF::Tools::printChilds(QObject* obj, int level)
{
  static QString s;

  foreach(QObject* child, obj->children())
  {
    kDebug() << s.leftJustified(level, '-')
        << child->metaObject()->className() << ": "
        << child->objectName();
    printChilds(child, level + 1);
  }
}

void KMF::Tools::printActions(KXMLGUIClient* client)
{
  kDebug() << "*******************************************************";
  kDebug() << client->actionCollection()->componentData().componentName();

  foreach(QAction* action, client->actionCollection()->actions())
  {
    kDebug() << action->text();
  }
  kDebug() << "*******************************************************";
}

// This function is from qcolor.cpp
int KMF::Tools::hex2int(QChar hexchar)
{
  int v;
  if ( hexchar.isDigit() )
    v = hexchar.digitValue();
  else if ( hexchar >= 'A' && hexchar <= 'F' )
    v = hexchar.cell() - 'A' + 10;
  else if ( hexchar >= 'a' && hexchar <= 'f' )
    v = hexchar.cell() - 'a' + 10;
  else
    v = 0;
  return v;
}


QColor KMF::Tools::toColor(const QString& s)
{
  QColor result;

  if(s.isEmpty())
  {
    result.setRgb(0);
  }
  else if (s[0].isDigit()) // Color as integer
  {
    result.setRgb(s.toLong());
  }
  else if (s[0] == '#' && s.length() == 9) // Special alpha channel case
  {
    result.setRgba(qRgba((hex2int(s[1]) << 4) + hex2int(s[2]),
                         (hex2int(s[3]) << 4) + hex2int(s[4]),
                         (hex2int(s[5]) << 4) + hex2int(s[6]),
                         (hex2int(s[7]) << 4) + hex2int(s[8])));
  }
  else
  {
    result.setNamedColor(s);
  }
  return result;
}

QMap<QString, QString> KMF::Tools::readIniFile(const QString& ini)
{
  QMap<QString, QString> info;
  QFile f(ini);

  if(f.open(QIODevice::ReadOnly))
  {
    QStringList lines = QString(f.readAll()).split("\n");

    for(QStringList::Iterator it = lines.begin(); it != lines.end(); ++it)
    {
      QStringList keyAndValue = it->split("=");
      if(keyAndValue.count() == 2)
        info[keyAndValue[0]] = keyAndValue[1];
    }
    f.close();
  }
  return info;
}

// From Qt4 examples painting/painterpaths/window.cpp GPL-2
void KMF::Tools::drawRoundRect(QPainter* painter, const QRect& rect,
                               int radius)
{
  int dr = radius * 2;
  QPainterPath roundRectPath;

  roundRectPath.moveTo(rect.right(), rect.top() + radius);
  roundRectPath.arcTo(rect.right() - dr, rect.top(), dr, dr, 0.0, 90.0);
  roundRectPath.lineTo(rect.left() + radius, rect.top());
  roundRectPath.arcTo(rect.left(), rect.top(), dr, dr, 90.0, 90.0);
  roundRectPath.lineTo(rect.left(), rect.bottom() - radius);
  roundRectPath.arcTo(rect.left(), rect.bottom() - dr, dr, dr, 180.0, 90.0);
  roundRectPath.lineTo(rect.right() - radius, rect.bottom());
  roundRectPath.arcTo(rect.right() - dr, rect.bottom() -dr, dr, dr,
                      270.0, 90.0);
  roundRectPath.closeSubpath();

  painter->drawPath(roundRectPath);
}

void KMF::Tools::spy(QObject* obj)
{
  Q4puGenericSignalSpy* spy = new Q4puGenericSignalSpy(kapp->activeWindow());
  spy->spyOn(obj);
}

uint KMF::Tools::frames(const QString &type)
{
  if(type == "DVD-PAL")
  {
    return 25;
  }
  else // NTSC
  {
    return (uint)(30000.0 / 1001.0);
  }
}

void KMF::Tools::cleanFiles(const QString& d, const QStringList& files)
{
  KUrl::List list;
  QDir dir(d);

  for(QStringList::ConstIterator it=files.begin(); it!=files.end(); ++it)
  {
    dir.setNameFilters(QStringList(*it));
    QStringList files2 = dir.entryList(QDir::Files | QDir::NoSymLinks);
    for(QStringList::Iterator jt=files2.begin(); jt!=files2.end(); ++jt)
    {
      QFile file(dir.filePath(*jt));
      file.remove();
    }
  }
  // Remove dirs if they are empty
  int pos = -1;

  while((pos = d.lastIndexOf(QDir::separator(), pos)) != -1)
  {
    QString s = d.left(pos--);
    if(dir.rmdir(s) == false)
      break;
  }
}

QImage KMF::Tools::variantList2Image(QVariant v)
{
  QByteArray a;
  QImage img;

  foreach (const QVariant& var, v.toList())
  {
    a.append(var.toString()[0].cell());
  }
  img.loadFromData(a);
  return img;
}

bool KMF::Tools::saveString2File(const KUrl& url, const QString& string, bool showFailed)
{
  //kDebug() << url;
  bool result = false;
  QFile *f;

  if (url.isLocalFile())
  {
    f = new KSaveFile(url.path());
  }
  else
  {
    f = new KTemporaryFile();
  }

  if (f->open(QIODevice::WriteOnly))
  {
      QTextStream stream(f);
      stream.setCodec("UTF-8");
      stream << string;
      stream.flush();
      f->close();
      if (url.isLocalFile())
      {
        result = true;
      }
      else
      {
        if (KIO::NetAccess::upload(f->fileName(), url, kapp->activeWindow()))
        {
          result = true;
        }
      }
  }

  delete f;

  if (showFailed && !result)
  {
    KMessageBox::error(kapp->activeWindow(), i18n("Error saving file %1", url.prettyUrl()));
  }
  return result;
}

bool KMF::Tools::loadStringFromFile(const KUrl& url, QString* string, bool showFailed)
{
  bool result = false;
  QString tmpFile;

  if(KIO::NetAccess::download(url, tmpFile, kapp->activeWindow()))
  {
    QFile file(tmpFile);
    if(file.open(QIODevice::ReadOnly))
    {
      QTextStream stream(&file);
      stream.setCodec("UTF-8");
      *string = stream.readAll();
      file.close();
      result = true;
    }
    KIO::NetAccess::removeTempFile(tmpFile);
  }

  if (showFailed && !result)
  {
    KMessageBox::error(kapp->activeWindow(), i18n("Error opening file %1", url.prettyUrl()));
  }
  return result;
}

QByteArray KMF::Tools::loadByteArray(const KUrl& url)
{
  QString tmpFile;
  QByteArray ba;

  if(KIO::NetAccess::download(url, tmpFile, kapp->activeWindow()))
  {
    QFile file(tmpFile);
    if(file.open(QIODevice::ReadOnly))
    {
      ba = file.readAll();
      file.close();
    }
    KIO::NetAccess::removeTempFile(tmpFile);
  }
  return ba;
}

QString KMF::Tools::xmlElement2String(const QDomElement& elem)
{
  QString s;
  QTextStream ts(&s);

  elem.save(ts, 1);
  return s;
}

QDomElement KMF::Tools::string2XmlElement(const QString& s)
{
  QDomDocument doc;
  doc.setContent(s);
  return doc.documentElement();
}

QMap<QString, QString> KMF::Tools::variantMap2StringMap(const QMap<QString, QVariant>& map)
{
  QMap<QString, QString> result;

  foreach (const QString& key, map.keys())
  {
    result[key] = map[key].toString();
  }
  return result;
}

QStringList KMF::Tools::variantList2StringList(const QVariantList& list)
{
  QStringList result;

  foreach (const QVariant& v, list)
  {
    result.append(v.toString());
  }
  return result;
}
