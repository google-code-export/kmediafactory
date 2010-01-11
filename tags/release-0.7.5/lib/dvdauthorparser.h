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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//**************************************************************************

#ifndef KMEDIAFACTORY_DVDAUTHORPARSER_H
#define KMEDIAFACTORY_DVDAUTHORPARSER_H

#include <kdemacros.h>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QDomDocument>

namespace KMF
{
  class KDE_EXPORT DVDAuthorParser
  {
    public:
      DVDAuthorParser() {};
      virtual ~DVDAuthorParser() {};

      void setFile(const QString& fileName);
      QStringList files();

    private:
      void findFiles(const QDomElement& element);

      QDomDocument m_doc;
      QString m_file;
      QStringList m_files;
  };
}

#endif // KMEDIAFACTORY_DVDAUTHORPARSER_H