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

#include "plugininterface.h"

#include <QtCore/QMutexLocker>


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

QImage KMF::TemplateObject::preview(const QString&)
{ 
  return QImage(); 
}

QStringList KMF::TemplateObject::menus() 
{ 
  return QStringList(); 
}

bool KMF::TemplateObject::clicked() 
{ 
  return false; 
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

static QMutex mutex;
static QMap<uint, uint> parents;
static uint id = 0;

uint KMF::PluginInterface::messageId()
{
  QMutexLocker locker(&mutex);
  return ++id;
}

uint KMF::PluginInterface::subId(uint parentId)
{
  QMutexLocker locker(&mutex);
  uint subid = ++id;
  parents[subid] = parentId;
  return subid;
}

uint KMF::PluginInterface::parent(uint subid)
{
  QMutexLocker locker(&mutex);
  if (parents.keys().contains(subid))
    return parents[subid];
  return 0;
}

#include "plugininterface.moc"

