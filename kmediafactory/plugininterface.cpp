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
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************

#include "plugininterface.h"
#include <QMutexLocker>

KMF::MediaObject::MediaObject(QObject* parent) :
  Object(parent)
{
}

KMF::MediaObject::~MediaObject()
{
}

KMF::TemplateObject::TemplateObject(QObject *parent) :
  Object(parent)
{
}

KMF::TemplateObject::~TemplateObject()
{
}

KMF::OutputObject::OutputObject(QObject *parent) :
  Object(parent)
{
}

KMF::OutputObject::~OutputObject()
{
}

KMF::ProgressDialog::ProgressDialog(QObject *parent) :
  QObject(parent)
{
}

KMF::ProgressDialog::~ProgressDialog()
{
}

KMF::PluginInterface::PluginInterface(QObject *parent) :
  QObject(parent)
{
  setObjectName("KMF::PluginInterface");
}

KMF::PluginInterface::~PluginInterface()
{
}

uint KMF::PluginInterface::messageId()
{
  static QMutex mutex;
  static uint id = 0;
  QMutexLocker locker(&mutex);

  return ++id;
}

#include "plugininterface.moc"
