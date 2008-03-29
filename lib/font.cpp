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
#include "font.h"
#include <kdebug.h>
#include <fontconfig/fontconfig.h>
#include <qregexp.h>
#include <qsize.h>
#include <Magick++.h>

QMap<QString, QString> KMF::Font::m_fileMap;

Magick::TypeMetric KMF::Font::typeMetrics(const QString& text) const
{
  //kdDebug() << k_funcinfo << family() << ": " << file() << endl;

  Magick::Image background;
  Magick::TypeMetric type_metrics;
  // Kepp these ascii() functions. Some platforms don't have STL support in QT.
  std::string txt = (const char*)text.utf8();
  if(file().right(4).lower() == ".ttf")
    background.font((const char*)("@" + file()).local8Bit());
  else if(!file().isEmpty())
    background.font((const char*)file().local8Bit());
  else
    background.font((const char*)family().local8Bit());
  background.fontPointsize(pointSize());
  background.fontTypeMetrics(txt, &type_metrics);
  return type_metrics;
}

const QString& KMF::Font::file() const 
{ 
  QString name = longName();
  /*
  QStringList list = fileMap().keys();
  for(QStringList::Iterator it = list.begin(); 
      it != list.end(); ++it ) 
  {
    kdDebug() << k_funcinfo << name << *it << endl;
  }
  */
  return fileMap()[name]; 
}

QSize KMF::Font::pixelSize(const QString& text) const
{
  Magick::TypeMetric t = typeMetrics(text);
  return QSize((int)t.textWidth(), (int)t.textHeight());
}

int KMF::Font::pixelHeight() const
{
  Magick::TypeMetric t = typeMetrics("Ag");
  return (int)t.textHeight();
}

int KMF::Font::pixelWidth(const QString& text) const
{
  return (int)typeMetrics(text).textWidth();
}

void KMF::Font::toXML(QDomElement& element) const
{
  element.setAttribute("name", family());
  //element.setAttribute("color", m_color);
  element.setAttribute("size", pointSize());
  element.setAttribute("weight", weight() * 10);
}

void KMF::Font::fromXML(const QDomElement& element)
{
  setFamily(element.attribute("name", "Helvetica"));
  // Widget has color attribute
  //m_color = element.attribute("color", "0").toLong();
  setPointSize(element.attribute("size", "22").toInt());
  setWeight(element.attribute("weight", "400").toInt() / 10);
  //kdDebug() << k_funcinfo << family() << pointSize() << weight() << endl;
}

int KMF::Font::fcWeight2QtWeight(int fcWeight)
{
  if(fcWeight <= FC_WEIGHT_LIGHT)
    return Light;
  else if(fcWeight >= FC_WEIGHT_BLACK)
    return Black;
  else if(fcWeight >= FC_WEIGHT_BOLD)
    return Bold;
  else if(fcWeight >= FC_WEIGHT_DEMIBOLD)
    return DemiBold;
  else
    return Normal;
}

QString KMF::Font::longName() const
{
  QString result = QString("%1-%2-%3-%4")
      .arg(family()).arg(stretch()).arg(weight()).arg(italic());
  //kdDebug() << k_funcinfo << result << endl;
  return result;
}

const QMap<QString, QString>& KMF::Font::fileMap()
{
  if(m_fileMap.count() == 0)
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
      KMF::Font font;
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
      kdDebug() << k_funcinfo << (const char*)family << ", " << weight << ", "
            << slant << ", " << width << ", " << (const char*)file
            << font.longName() << endl;
      */
      m_fileMap[font.longName()] = (const char*)file;

      // Font names with '-' cause mapping problems. Qt mangles them?
      QString s = (const char*)family;
      s.replace('-', " ");
      if(s != (const char*)family)
      {
        font.setFamily(s);
        //kdDebug() << k_funcinfo << font.longName() << endl;
        m_fileMap[font.longName()] = (const char*)file;
      }
    }
    FcFontSetDestroy (fontset);
  }
  return m_fileMap;
}
