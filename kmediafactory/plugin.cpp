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
#include "plugin.h"
#include <qobjectlist.h>
#include <kdebug.h>
#include <kmessagebox.h>

KMF::Plugin::Plugin(QObject *parent, const char* name)
  : QObject(parent, name)
{
}

KMF::Plugin::~Plugin()
{
  deleteChildren();
}

KMF::UiInterface* KMF::Plugin::uiInterface()
{
  if(parent())
  {
    QObject* obj = parent()->child(0, "KMF::UiInterface");
    if(obj)
      return static_cast<KMF::UiInterface*>(obj);
  }
  return 0;
}

KMF::ProjectInterface* KMF::Plugin::projectInterface()
{
  if(parent())
  {
    QObject* obj = parent()->child(0, "KMF::ProjectInterface");
    if(obj)
      return static_cast<KMF::ProjectInterface*>(obj);
  }
  return 0;
}

void KMF::Plugin::deleteChildren()
{
  if(children())
  {
    QObjectListIt it(*children());
    QObject* obj;
    while((obj=it.current()))
    {
      ++it;
      delete obj;
    }
  }
}

#include "plugin.moc"
