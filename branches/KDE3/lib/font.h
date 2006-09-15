//**************************************************************************
//   Copyright (C) 2004, 2005 by Petri Damst�
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
#ifndef KMFFONT_H
#define KMFFONT_H

#include <qstring.h>
#include <qfont.h>
#include <qdom.h>
#include <qmap.h>

namespace Magick { class TypeMetric; };

namespace KMF
{
  class Font : public QFont
  {
    public:
      Font() : QFont() {};
      Font(QFont font) : QFont(font) {};

      const QString& file() const;
      void toXML(QDomElement& element) const;
      void fromXML(const QDomElement& element);
      QSize pixelSize(const QString& text) const;
      int pixelHeight() const;
      int pixelWidth(const QString& text) const;

      static const QMap<QString, QString>& fileMap();

    private:
      static QMap<QString, QString> m_fileMap;

      Magick::TypeMetric typeMetrics(const QString& text) const;
      static int fcWeight2QtWeight(int fcWeight);
      QString longName() const;
  };
}

#endif
