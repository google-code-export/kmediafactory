//**************************************************************************
//   Copyright (C) 2004 by Petri Damst�
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
#include "kmfwidgetfactory.h"
#include "kmfmenu.h"
#include "kmfmenupage.h"
#include "kmfimage.h"
#include "kmflabel.h"
#include "kmfframe.h"
#include "kmfbutton.h"
#include "kmflayout.h"
#include "templateobject.h"
#include <kdebug.h>
#include <qregexp.h>
#include <qvariant.h>

KMFWidgetFactory::KMFWidgetFactory(QObject *parent, const char *name)
  : KMFTemplateBase(parent, name), m_title(0), m_chapter(0),
    m_customProperties(0)
{
  m_customProperties = &templateObject()->customProperties();
}

KMFWidgetFactory::~KMFWidgetFactory()
{
}

KMFWidget* KMFWidgetFactory::createPage(const QDomElement& element,
                                        QObject* parent, int title,
                                        int chapter)
{
  KMFWidgetFactory* factory = new KMFWidgetFactory(parent);
  KMFWidget* result = 0;
  if(factory)
  {
    factory->m_title = title;
    factory->m_chapter = chapter;
    result = factory->create(element, parent);
  }
  delete factory;
  return result;
}

KMFWidget* KMFWidgetFactory::newWidget(QString type, QObject* parent)
{
  if(type == "page")
  {
    KMFMenuPage* page =  new KMFMenuPage(parent);
    page->setTitleStart(m_title);
    page->setChapterStart(m_chapter);
    return page;
  }
  else if(type == "image")
    return new KMFImage(parent);
  else if(type == "label")
    return new KMFLabel(parent);
  else if(type == "frame")
    return new KMFFrame(parent);
  else if(type == "button")
    return new KMFButton(parent);
  else if(type == "grid")
    return new KMFGrid(parent);
  else if(type == "vbox")
    return new KMFVBox(parent);
  else if(type == "hbox")
    return new KMFHBox(parent);
  else if(type == "spacer" || type == "group")
    return new KMFWidget(parent);
  return 0;
}

KMFWidget* KMFWidgetFactory::create(const QDomElement& element,
                                    QObject* parent)
{
  KMFWidget* result = newWidget(element.tagName(), parent);
  //kdDebug() << element.tagName() << ": " << result << "/" << parent << endl;
  if (!result && !parent)
    return 0;

  if(result)
  {
    bool set;

    result->fromXML(element);

    KConfigSkeletonItem::List items = m_customProperties->items();
    KConfigSkeletonItem::List::ConstIterator it;

    for(it = items.begin(); it != items.end(); ++it)
    {
      if((*it)->group().startsWith("%"))
      {
        set = ((*it)->group().remove('%') == result->className());
        //if(set) kdDebug() << k_funcinfo << result->className() << endl;
      }
      else
      {
        set = QRegExp((*it)->group()).exactMatch(result->name());
        //if(set) kdDebug() << k_funcinfo << result->name() << endl;
      }

      if(set)
      {
        QString name = (*it)->name();
        int n = name.find("::");
        if(n >= 0)
          name = name.mid(n+2);
        result->setProperty(name, (*it)->property());
        //kdDebug() << k_funcinfo << name << " : " << (*it)->property() << endl;
      }
    }
  }

  QDomNode n = element.firstChild();
  while(!n.isNull())
  {
    QDomElement e = n.toElement();
    if(!e.isNull())
    {
      create(e, result);
    }
    n = n.nextSibling();
  }
  return result;
}

#include "kmfwidgetfactory.moc"