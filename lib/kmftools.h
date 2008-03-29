//**************************************************************************
//   Copyright (C) 2004, 2005 by Petri Damstén
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
#ifndef KMFKMFTOOLS_H
#define KMFKMFTOOLS_H

#include <qstring.h>
#include <qsize.h>
#include <stdint.h>
#include <kurl.h>

class QStringList;

namespace KMF
{

  /**
    @author Petri Damsten <petri.damsten@iki.fi>
  */
  class Tools
  {
    public:
      Tools();
      ~Tools();

      static QString toAscii(QString text);
      static QString simpleName(QString s);
      static QString simpleBaseName(QString file);
      static QString simple2Title(QString s);
      static void removeDuplicates(QStringList* list);
      static QString sizeString(uint64_t size);
      //static uint64_t fileSize(QString file);
      static void stripExisting(KURL::List* src, const KURL& dest);
      static QString addSlash(QString path);
      static QString joinPaths(QString path1, QString path2);
      static QStringList file2List(const QString& file,
                                   const QString& comment = QString::null,
                                   const QString& startsWith = QString::null);
      static QString findExe(const QString& exe, const QStringList& paths,
                             const QString& extraPrefix = QString::null);
      static QStringList findAllResources(const char* type,
                                          const QString& filter=QString::null);
      static QSize resolution(const QSize& original, const QSize& originalRatio,
                              const QSize& dest, const QSize& destRatio,
                              QSize::ScaleMode mode = QSize::ScaleMax);
      static QSize maxResolution(const QString &type);
      static QSize guessRatio(const QSize& image, const QSize& video);
  };
}

#endif
