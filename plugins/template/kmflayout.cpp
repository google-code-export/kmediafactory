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
#include "kmflayout.h"
#include <kdebug.h>
#include <qobjectlist.h>

bool KMFGrid::isHidden() const
{
  for(QObjectListIt it(*children()); it.current() != 0; ++it)
    if(!static_cast<KMFWidget*>(*it)->isHidden())
      return false;
  return true;
}

int KMFGrid::childY(const KMFWidget* child) const
{
  int result = paintY();

  for(QObjectListIt it(*children()); it.current() != 0; ++it)
  {
    KMFWidget* widget = static_cast<KMFWidget*>(*it);
    //kdDebug() << k_funcinfo << child->className()
    //    << " (" << child->name() << "): " << result << endl;
    if(widget == child)
      return result;
    if(child->column() == widget->column())
    {
      result += widget->height();
      //kdDebug() << k_funcinfo << widget->className()
      //    << " (" << widget->name() << "): " << widget->height() << endl;
    }
  }
  return result;
}

int KMFGrid::childHeight() const
{
  int result = 0;

  for(QObjectListIt it(*children()); it.current() != 0; ++it)
  {
    KMFWidget* widget = static_cast<KMFWidget*>(*it);
    if(widget->geometry().height().type() != KMFUnit::Percentage)
      if(widget->column() == 0)
        result += widget->height();
  }
  return paintHeight() - result;
}

int KMFGrid::childHeightPercentage() const
{
  const QObjectList *list = children();
  int result = 0;

  if(list)
  {
    for(QObjectListIt it(*list); it.current() != 0; ++it)
    {
      if((*it)->inherits("KMFWidget"))
      {
        KMFWidget* widget = static_cast<KMFWidget*>(*it);
        if(widget->geometry().height().type() == KMFUnit::Percentage &&
           (!widget->isHidden() || widget->takeSpace()))
        {
          if(widget->column() == 0)
            result += widget->geometry().height().value();
        }
      }
    }
  }
  return result;
}

int KMFGrid::childX(const KMFWidget* child) const
{
  int result = paintX();

  for(QObjectListIt it(*children()); it.current() != 0; ++it)
  {
    KMFWidget* widget = static_cast<KMFWidget*>(*it);
    if(widget == child)
      return result;
    if(child->row() == widget->row())
      result += widget->width();
  }
  return result;
}

int KMFGrid::childWidth() const
{
  int result = 0;

  for(QObjectListIt it(*children()); it.current() != 0; ++it)
  {
    KMFWidget* widget = static_cast<KMFWidget*>(*it);
    if(widget->geometry().width().type() != KMFUnit::Percentage)
      if(widget->row() == 0)
        result += static_cast<KMFWidget*>(*it)->width();
  }
  return paintWidth() - result;
}

int KMFGrid::childWidthPercentage() const
{
  const QObjectList *list = children();
  int result = 0;

  if(list)
  {
    for(QObjectListIt it(*list); it.current() != 0; ++it)
    {
      if((*it)->inherits("KMFWidget"))
      {
        KMFWidget* widget = static_cast<KMFWidget*>(*it);
        if(widget->geometry().width().type() == KMFUnit::Percentage &&
           (!widget->isHidden() || widget->takeSpace()))
        {
          if(widget->row() == 0)
            result += widget->geometry().width().value();
        }
      }
    }
  }
  return result;
}

int KMFGrid::minimumPaintWidth() const
{
  int result = 0;

  for(QObjectListIt it(*children()); it.current() != 0; ++it)
  {
    KMFWidget* widget = static_cast<KMFWidget*>(*it);
    if(widget->row() == 0 && !widget->isHidden())
      result += static_cast<KMFWidget*>(*it)->minimumWidth();
  }
  return result;
}

int KMFGrid::minimumPaintHeight() const
{
  int result = 0;

  for(QObjectListIt it(*children()); it.current() != 0; ++it)
  {
    KMFWidget* widget = static_cast<KMFWidget*>(*it);
    if(widget->column() == 0 && !widget->isHidden())
      result += static_cast<KMFWidget*>(*it)->minimumHeight();
  }
  return result;
}

KMFVBox::KMFVBox(QObject *parent, const char *name)
  : KMFGrid(parent, name)
{
}

KMFVBox::~KMFVBox()
{
}

int KMFVBox::minimumPaintWidth() const
{
  int result = 0;

  for(QObjectListIt it(*children()); it.current() != 0; ++it)
  {
    KMFWidget* widget = static_cast<KMFWidget*>(*it);
    if(!widget->isHidden())
      result = QMAX(widget->minimumWidth(), result);
  }
  return result;
}

KMFHBox::KMFHBox(QObject *parent, const char *name)
  : KMFGrid(parent, name)
{
}

KMFHBox::~KMFHBox()
{
}

int KMFHBox::minimumPaintHeight() const
{
  int result = 0;

  for(QObjectListIt it(*children()); it.current() != 0; ++it)
  {
    KMFWidget* widget = static_cast<KMFWidget*>(*it);
    //kdDebug() << widget->className() << ": "
    //    << widget->minimumHeight() << endl;
    if(!widget->isHidden())
      result = QMAX(widget->minimumHeight(), result);
  }
  return result;
}

#include "kmflayout.moc"
