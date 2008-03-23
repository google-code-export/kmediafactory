//**************************************************************************
//   Copyright (C) 2008 by Petri Damsten
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

#include <KDebug>
#include "krossplugin.h"
#include <kross/core/action.h>
#include <kross/core/interpreter.h>
#include <kross/core/manager.h>

KrossPlugin::KrossPlugin(QObject *parent, const QVariantList &args) :
  KMF::Plugin(parent)
{
  KService::Ptr service = args[0].value<KService::Ptr>();
  QString name = service->property("Name").toString();
  // Custom property ScriptName?
  setObjectName(name);
  kDebug() << name;
  // Initialize GUI
  //setComponentData(KrossFactory::componentData());
  //setXMLFile("kmediafactory_krossui.rc");


}
    
KrossPlugin::~KrossPlugin()
{
}

void KrossPlugin::init(const QString &type)
{
  kDebug() << type;
  deleteChildren();

  Kross::Action *action = new Kross::Action(this, "test");
  action->setFile("/home/damu/test.py");
  action->addObject(this, "plugin");
  action->trigger();
  kDebug() << action->functionNames();
  action->callFunction("test3", QVariantList());
}

QStringList KrossPlugin::supportedProjectTypes()
{
  kDebug();
  QStringList result;
  return result;
}

void KrossPlugin::test(QVariant v)
{
  kDebug() << v;
}

#include "krossplugin.moc"