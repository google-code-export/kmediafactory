//**************************************************************************
//   Copyright (C) 2004 by Petri Damstén
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
#include "kmftemplatebase.h"
#include "templateobject.h"
#include "kmfmenu.h"
#include "kmfmenupage.h"
#include <kmediafactory/uiinterface.h>
#include <kmediafactory/plugin.h>
#include <qregexp.h>

KMFTemplateBase::KMFTemplateBase(QObject *parent, const char *name) :
  QObject(parent, name), m_uiIf(0), m_prjIf(0)
{
  KMF::Plugin* p = plugin();
  if(p)
  {
    m_uiIf = p->uiInterface();
    m_prjIf = p->projectInterface();
  }
}

KMFTemplateBase::~KMFTemplateBase()
{
}

KMF::Plugin* KMFTemplateBase::plugin()
{
  QObject* obj = parent();

  while(obj)
  {
    if(obj->inherits("KMF::Plugin"))
      return static_cast<KMF::Plugin*>(obj);
    obj = obj->parent();
  }
  return 0;
}

TemplateObject* KMFTemplateBase::templateObject()
{
  QObject* obj = parent();

  while(obj)
  {
    if(obj->inherits("TemplateObject"))
      return static_cast<TemplateObject*>(obj);
    obj = obj->parent();
  }
  return 0;
}

KMFMenuPage* KMFTemplateBase::page()
{
  QObject* obj = this;

  while(obj)
  {
    if(obj->inherits("KMFMenuPage"))
      return static_cast<KMFMenuPage*>(obj);
    obj = obj->parent();
  }
  return 0;
}

KMFMenu* KMFTemplateBase::menu()
{
  QObject* obj = this;

  while(obj)
  {
    if(obj->inherits("KMFMenu"))
      return static_cast<KMFMenu*>(obj);
    obj = obj->parent();
  }
  return 0;
}

QString KMFTemplateBase::uiText(QString string)
{
  int i = 0;
  QString s = string.replace("_", " ");
  QRegExp rx("\\b\\w");
      
  s = s.simplifyWhiteSpace().lower();
  while(i!=-1)
  {
    s.at(i) = s.at(i).upper();
    i = s.find(rx, i+1);
  }
  return s;
}

#include "kmftemplatebase.moc"
