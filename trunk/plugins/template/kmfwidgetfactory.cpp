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
#include <QRegExp>
#include <QVariant>

KMFWidgetFactory::KMFWidgetFactory(QObject *parent)
  : KMFTemplateBase(parent), m_title(0), m_chapter(0),
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
  //kDebug() << element.tagName() << ": " << result << "/" << parent << endl;
  if (!result && !parent)
    return 0;

  if(result)
  {
    bool set;

    result->fromXML(element);

    KConfigSkeletonItem::List items = m_customProperties->items();

    foreach(KConfigSkeletonItem* item, items)
    {
      if(item->group().startsWith("%"))
      {
        set = (item->group().remove('%') == result->metaObject()->className());
        //if(set) kDebug() << k_funcinfo << result->className() << endl;
      }
      else
      {
        set = QRegExp(item->group()).exactMatch(result->objectName());
        //if(set) kDebug() << k_funcinfo << result->name() << endl;
      }

      if(set)
      {
        QString name = item->name();
        int n = name.indexOf("::");
        if(n >= 0)
          name = name.mid(n+2);
        //kDebug() << k_funcinfo << name << " : " << item->property() << endl;
        result->setProperty(name, item->property());
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
