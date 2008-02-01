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
#include "kmflayout.h"
#include <kdebug.h>
#include <qobject.h>

bool KMFGrid::isHidden() const
{
  foreach(QObject* ob, children())
  {
    KMFWidget* widget = static_cast<KMFWidget*>(ob);
    if(!widget->isHidden())
      return false;
  }
  return true;
}

int KMFGrid::childY(const KMFWidget* child) const
{
  int result = paintY();

  foreach(QObject* ob, children())
  {
    KMFWidget* widget = static_cast<KMFWidget*>(ob);
    //kDebug() << child->className()
    //    << " (" << child->name() << "): " << result;
    if(widget == child)
      return result;
    if(child->column() == widget->column())
    {
      result += widget->height();
      //kDebug() << widget->className()
      //    << " (" << widget->name() << "): " << widget->height();
    }
  }
  return result;
}

int KMFGrid::childHeight() const
{
  int result = 0;

  foreach(QObject* ob, children())
  {
    KMFWidget* widget = static_cast<KMFWidget*>(ob);
    if(widget->geometry().height().type() != KMFUnit::Percentage)
      if(widget->column() == 0)
        result += widget->height();
  }
  return paintHeight() - result;
}

int KMFGrid::childHeightPercentage() const
{
  int result = 0;

  foreach(QObject* ob, children())
  {
    KMFWidget* widget = static_cast<KMFWidget*>(ob);
    if(widget->geometry().height().type() == KMFUnit::Percentage &&
        (!widget->isHidden() || widget->takeSpace()))
    {
      if(widget->column() == 0)
        result += widget->geometry().height().value();
    }
  }
  return result;
}

int KMFGrid::childX(const KMFWidget* child) const
{
  int result = paintX();

  foreach(QObject* ob, children())
  {
    KMFWidget* widget = static_cast<KMFWidget*>(ob);
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

  foreach(QObject* ob, children())
  {
    KMFWidget* widget = static_cast<KMFWidget*>(ob);
    if(widget->geometry().width().type() != KMFUnit::Percentage)
      if(widget->row() == 0)
        result += widget->width();
  }
  return paintWidth() - result;
}

int KMFGrid::childWidthPercentage() const
{
  int result = 0;

  foreach(QObject* ob, children())
  {
    KMFWidget* widget = static_cast<KMFWidget*>(ob);
    if(widget->geometry().width().type() == KMFUnit::Percentage &&
        (!widget->isHidden() || widget->takeSpace()))
    {
      if(widget->row() == 0)
        result += widget->geometry().width().value();
    }
  }
  return result;
}

int KMFGrid::minimumPaintWidth() const
{
  int result = 0;

  foreach(QObject* ob, children())
  {
    KMFWidget* widget = static_cast<KMFWidget*>(ob);
    if(widget->row() == 0 && !widget->isHidden())
      result += widget->minimumWidth();
  }
  return result;
}

int KMFGrid::minimumPaintHeight() const
{
  int result = 0;

  foreach(QObject* ob, children())
  {
    KMFWidget* widget = static_cast<KMFWidget*>(ob);
    if(widget->column() == 0 && !widget->isHidden())
      result += widget->minimumHeight();
  }
  return result;
}

KMFVBox::KMFVBox(QObject *parent)
  : KMFGrid(parent)
{
}

KMFVBox::~KMFVBox()
{
}

int KMFVBox::minimumPaintWidth() const
{
  int result = 0;

  foreach(QObject* ob, children())
  {
    KMFWidget* widget = static_cast<KMFWidget*>(ob);
    if(!widget->isHidden())
      result = qMax(widget->minimumWidth(), result);
  }
  return result;
}

KMFHBox::KMFHBox(QObject *parent)
  : KMFGrid(parent)
{
}

KMFHBox::~KMFHBox()
{
}

int KMFHBox::minimumPaintHeight() const
{
  int result = 0;

  foreach(QObject* ob, children())
  {
    KMFWidget* widget = static_cast<KMFWidget*>(ob);
    //kDebug() << widget->className() << ": "
    //    << widget->minimumHeight();
    if(!widget->isHidden())
      result = qMax(widget->minimumHeight(), result);
  }
  return result;
}

#include "kmflayout.moc"
