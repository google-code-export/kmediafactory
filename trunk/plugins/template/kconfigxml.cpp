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
#include "kconfigxml.h"
#include <kdebug.h>
#include <qfile.h>
#include <qdom.h>
#include <ktempfile.h>

class TempFileStore
{
  public:
    static KTempFile* tempFile(KConfigXML* config)
    {
      if(!m_tempFiles.contains(config))
      {
        m_tempFiles[config] = new KTempFile();
        m_tempFiles[config]->setAutoDelete(true);
      }
      return m_tempFiles[config];
    }
    static void removeTempFile(KConfigXML* config)
    {
      if(m_tempFiles.contains(config))
      {
        KTempFile* tmp = m_tempFiles[config];
        m_tempFiles.remove(config);
        delete tmp;
      }
    }
  private:
    static QMap<KConfigXML*, KTempFile*> m_tempFiles;
};

QMap<KConfigXML*, KTempFile*> TempFileStore::m_tempFiles;

KConfigXML::KConfigXML()
  : KConfigSkeleton(TempFileStore::tempFile(this)->name())
{
}

KConfigXML::KConfigXML(QString kcfgFile)
  : KConfigSkeleton(TempFileStore::tempFile(this)->name())
{
  parse(kcfgFile);
}

KConfigXML::KConfigXML(QIODevice* kcfgFile)
  : KConfigSkeleton(TempFileStore::tempFile(this)->name())
{
  parse(kcfgFile);
}

KConfigXML::KConfigXML(QByteArray* kcfgFile)
  : KConfigSkeleton(TempFileStore::tempFile(this)->name())
{
  parse(kcfgFile);
}

KConfigXML::~KConfigXML()
{
  TempFileStore::removeTempFile(this);
}

bool KConfigXML::parse(QString kcfgFile)
{
  QFile file(kcfgFile);

  if(file.open(QIODevice::ReadOnly))
  {
    parse(&file);
    file.close();
    return true;
  }
  kError() << "Unable to open file: " << kcfgFile << endl;
  return false;
}

bool KConfigXML::parse(QIODevice* kcfgFile)
{
  QDomDocument doc("kcfg");
  if(!doc.setContent(kcfgFile))
  {
    kError() << "Unable to load document." << endl;
    return false;
  }
  return parse(doc);
}

bool KConfigXML::parse(QByteArray* kcfgFile)
{
  QDomDocument doc("kcfg");
  if(!doc.setContent(*kcfgFile))
  {
    kError() << "Unable to load document." << endl;
    return false;
  }
  return parse(doc);
}

// Lot's of code taken from KConfigEditor / kdesubsystem.cpp
bool KConfigXML::parse(QDomDocument& doc)
{
  QDomElement el = doc.documentElement();
  if(el.isNull())
  {
    kError() << "No document in cfg file" << endl;
    return false;
  }

  QDomNode n;

  for(n = el.firstChild(); !n.isNull(); n = n.nextSibling())
  {
    QDomElement e = n.toElement();

    QString tag = e.tagName();
    if(tag == "kcfgfile")
    {
      // ignore
    }
    else if(tag == "include")
    {
       //ignore
    }
    else if(tag == "group")
    {
      QString group = e.attribute("name");
      if(group.isEmpty())
      {
        kError() << "Group without name" << endl;
        continue;
      }

      setCurrentGroup(group);

      QDomNode n2;
      for(n2 = e.firstChild(); !n2.isNull(); n2 = n2.nextSibling())
      {
        QDomElement e2 = n2.toElement();
        parseKCFGXMLEntry(e2);
      }
    }
  }
  return true;
}

void KConfigXML::parseKCFGXMLEntry(const QDomElement& element)
{
  QString label;
  QString defaultValue;
  double min = 0;
  double max = 0;
  QString whatsThis;
  QString code;
  QStringList choices;
  QStringList values;
  QString type = element.attribute( "type" );
  QString name = element.attribute( "name" );
  QString key  = element.attribute( "key" );
  //bool hidden  = ( element.attribute( "hidden" ) == "true" );

  if(name.isEmpty() && key.isEmpty())
    return;//FIXME: report an error?

  QDomNode n;
  for(n = element.firstChild(); !n.isNull(); n = n.nextSibling())
  {
    QDomElement e = n.toElement();
    QString tag = e.tagName();
    if(tag == "label")
      label = e.text();
    else if(tag == "whatsthis")
      whatsThis = e.text();
    else if ( tag == "min" )
      min = e.text().toDouble();
    else if ( tag == "max" )
      max = e.text().toDouble();
    else if ( tag == "code" )
      code = e.text();
    else if ( tag == "values" )
    {
      QDomNode n3;
      for(n3 = e.firstChild(); !n3.isNull(); n3 = n3.nextSibling())
      {
        QDomElement e3 = n3.toElement();
        if(e3.tagName() == "value")
        {
          values.append(e3.text());
        }
      }
    }
    else if(tag == "parameter")
    {
      //ignore
    }
    else if(tag == "default")
    {
      if(e.attribute("code") == "true")
        defaultValue = parseCode(e.text());
      else
        defaultValue = e.text();
    }
    else if(tag == "choices")
    {
      QDomNode n2;
      for(n2 = e.firstChild(); !n2.isNull(); n2 = n2.nextSibling())
      {
        QDomElement e2 = n2.toElement();
        if(e2.tagName() == "choice")
        {
          choices.append(e2.text());
        }
      }
    }
  }

  if(!key.isEmpty())
  {
    name = key;
  }

  if(type.isEmpty())
    type = "String";
  /*
  kdDebug() << k_funcinfo << type << ": " << name << ", " << defaultValue
      << endl;
  */

  if(type == "String")
  {
    m_strings.append("");
    addItemString(name, m_strings.last(), defaultValue);
  }
  else if(type == "Bool")
  {
    m_bools.append(false);
    addItemBool(name, m_bools.last(), (defaultValue == "true"));
  }
  else if(type == "Color")
  {
    m_colors.append(QColor(192, 192, 192));
    addItemColor(name, m_colors.last(), QColor(defaultValue));
  }
  else if(type == "Font")
  {
    m_fonts.append(QFont());
    QFont f;

    f.fromString(defaultValue);
    addItemFont(name, m_fonts.last(), f);
  }
  else if(type == "Int")
  {
    m_ints.append(0);
    addItemInt(name, m_ints.last(), defaultValue.toInt());
  }
}

