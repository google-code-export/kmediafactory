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

#include "dvdauthorparser.h"
#include <kmftools.h>

void KMF::DVDAuthorParser::setFile(const QString& fileName)
{
  m_file = fileName;
  m_files.clear();
  QString s;
  if (KMF::Tools::loadStringFromFile(fileName, &s))
    m_doc.setContent(s);
}

QStringList KMF::DVDAuthorParser::files()
{
  if(m_files.count() == 0)
  {
    QDomElement element = m_doc.documentElement();
    findFiles(element);
  }
  return m_files;
}

void KMF::DVDAuthorParser::findFiles(const QDomElement& element)
{
  QDomNode n = element.firstChild();
  while(!n.isNull())
  {
    QDomElement e = n.toElement();
    if(e.tagName() == "vob")
    {
      m_files.append(e.attribute("file"));
    }
    else
      findFiles(e);
    n = n.nextSibling();
  }
}


