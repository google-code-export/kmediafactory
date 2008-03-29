//**************************************************************************
//   Copyright (C) 2004, 2005 by Petri Damst?
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
#include "kmftools.h"
#include <kstringhandler.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qstringlist.h>
#include <sys/stat.h>
#include <errno.h>

KMF::Tools::Tools()
{
}


KMF::Tools::~Tools()
{
}

QString KMF::Tools::toAscii(QString text)
{
  QString s = text;

  s.replace('Å', "A");
  s.replace('Ä', "A");
  s.replace('Ö', "O");
  s.replace('Õ', "O");
  s.replace('Ü', "U");
  s.replace('å', "a");
  s.replace('ä', "a");
  s.replace('ö', "o");
  s.replace('õ', "o");
  s.replace('ü', "u");
  return s;
}

QString KMF::Tools::simpleName(QString s)
{
  s.replace(' ', "_");
  s = toAscii(s);
  return s.lower();
}

QString KMF::Tools::simple2Title(QString s)
{
  int i;

  s.replace('-', ' ');
  s.replace('_', ' ');
  i = s.findRev('.');
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
  QString last;

  list->sort();
  for(QStringList::Iterator it = list->begin(); it != list->end(); )
  {
    QStringList::Iterator it2 = it++;
    if (*it2 == last)
    {
      // remove duplicate
      list->remove(it2);
    }
    else
    {
      last = *it2;
    }
  }
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
      kdDebug () << QString("stat(%1) error: %2").arg(file).arg(strerror(errno))
          << endl;
    return 0;
  }
  return buf.st_size;
}
*/

void KMF::Tools::stripExisting(KURL::List* src, const KURL& dest)
{
  KURL::List::iterator it = src->begin();
  QDir dir(dest.path());

  while(it != src->end())
  {
    QFileInfo fi((*it).path());
    QFileInfo fiDest(dir.filePath(fi.fileName()));

    if(fiDest.exists())
      it = src->remove(it);
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

  if(f.open(IO_ReadOnly))
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
  int n = filter.findRev(QDir::separator());
  QString d = filter.left(n + 1);
  QString f = filter.mid(n + 1);

  for(QStringList::ConstIterator it = dirs.begin(); it != dirs.end(); ++it)
  {
    QDir dir(*it + d);

    files = dir.entryList(f);
    for(QStringList::ConstIterator jt = files.begin(); jt != files.end(); ++jt)
      result.append(dir.filePath(*jt));
  }
  return result;
}

QSize KMF::Tools::resolution(const QSize& original,
                             const QSize& originalRatio,
                             const QSize& dest,
                             const QSize& destRatio,
                             QSize::ScaleMode mode)
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
