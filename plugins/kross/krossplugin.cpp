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

#include "krossplugin.h"
#include <ksharedptr.h>
#include <KDebug>
#include <KStandardDirs>
#include <QTimer>
#include <kross/core/action.h>
#include <kross/core/interpreter.h>
#include <kross/core/manager.h>
#include "krossuiinterface.h"
#include "krossprojectinterface.h"

K_EXPORT_KMEDIAFACTORY_PLUGIN(kross, KrossPlugin);

KrossPlugin::KrossPlugin(QObject* parent, const QVariantList& args) :
  KMF::Plugin(parent), m_action(0), m_uiIf(0), m_projectIf(0)
{
  KService::Ptr service = args[0].value<KService::Ptr>();
  QString name = service->property("Name").toString();
  QString script = service->property("X-KMediaFactory-Script").toString();
  QString uirc = service->property("X-KMediaFactory-UiRcFile").toString();

  kDebug() << name << script << uirc;
  setObjectName(name);
  m_action = new Kross::Action(this, script);

  script = KGlobal::dirs()->locate("appdata", "tools/bin/" + script);
  m_action->setFile(script);

  m_action->addObject(this, "kmediafactory");
  if (!uirc.isEmpty()) {
    setXMLFile(uirc);
  }
  m_uiIf = new KrossUiInterface(this, KMF::Plugin::uiInterface());

  kDebug() << "Running" << script;
  m_action->trigger();
}
    
KrossPlugin::~KrossPlugin()
{
}

void KrossPlugin::init(const QString &type)
{
  kDebug() << type;
  deleteChildren();

  if (m_plugin) {
    if (!m_projectIf) {
      m_projectIf = new KrossProjectInterface(this, KMF::Plugin::projectInterface());
    }
    // init is reserved word in ?? well in some scripting language
    m_plugin->callMethod("initPlugin", QVariantList() << type);
  }
}

QStringList KrossPlugin::supportedProjectTypes() const
{
  kDebug();
  if (m_plugin) {
    return m_plugin->callMethod("supportedProjectTypes").toStringList();
  }
  return QStringList();
}

KMF::MediaObject* KrossPlugin::createMediaObject(const QDomElement&)
{
  return 0;
}

const KMF::ConfigPage* KrossPlugin::configPage() const
{
  return 0;
}

void KrossPlugin::registerPlugin(Kross::Object::Ptr plugin)
{
  m_plugin = plugin;
}

QObject* KrossPlugin::uiInterface() 
{ 
  return m_uiIf; 
}

QObject* KrossPlugin::projectInterface() 
{ 
  return m_projectIf; 
}

#include "krossplugin.moc"
