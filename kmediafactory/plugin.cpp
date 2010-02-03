//**************************************************************************
//   Copyright (C) 2004-2008 by Petri Damsten
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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//**************************************************************************

#include "plugin.h"


KMF::Plugin::Plugin(QObject *parent)
  : QObject(parent)
{
}

KMF::Plugin::~Plugin()
{
  deleteChildren();
}

KMF::PluginInterface* KMF::Plugin::interface() const
{
  KMF::PluginInterface* obj = 0;

  if(parent()) {
    obj = parent()->findChild<KMF::PluginInterface*>("KMF::PluginInterface");
  }
  return obj;
}

void KMF::Plugin::deleteChildren()
{
  QObjectList list = children();

  while(!list.isEmpty())
    delete list.takeFirst();
}

KMF::MediaObject* KMF::Plugin::createMediaObject(const QDomElement&)
{ 
  return 0; 
}

const KMF::ConfigPage* KMF::Plugin::configPage() const 
{ 
  return 0; 
}

#include "plugin.moc"

