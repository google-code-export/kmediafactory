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
#include "kmfapplication.h"
#include "kmfplugininterface.h"
#include "kmediafactory.h"
#include "kmftools.h"
#include "kmfdbusinterface.h"
#include "kmediafactoryadaptor.h"
#include <KCmdLineArgs>
#include <KServiceTypeTrader>
#include <KStandardDirs>
#include <KDesktopFile>
#include <kparts/componentfactory.h>
#include <KDebug>
#include <KMainWindow>
#include <KLocale>
#include <KToolInvocation>
#include <klauncher_iface.h>
#include <QObject>
#include <QFileInfo>
#include <QtDBus>
#include <errno.h>

#if defined Q_WS_X11
#include <KStartupInfo>
#include <X11/Xlib.h>
#endif

KMFApplication::KMFApplication()
  : KApplication(), m_mainWin(0), m_project(0), m_pluginInterface(0),
    m_interface(0)
{
  iface = new KMFDbusInterface(this);
  new KMediaFactoryAdaptor(iface);
}

KMFApplication::~KMFApplication()
{
  delete iface;
}

void KMFApplication::init()
{
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  QDBusConnection::sessionBus().registerObject("/KMediaFactory", iface);

  if(args->count() > 0)
    m_url = args->url(0);

  m_mainWin = new KMediaFactory();
  m_mainWin->show();

  loadPlugins();

  m_mainWin->initGUI();

  args->clear();
}

KMFProject *KMFApplication::newProject()
{
  if(m_project)
    delete m_project;
  m_project = new KMFProject(this);
  return m_project;
}

void KMFApplication::loadPlugins()
{
  kDebug();
  m_pluginInterface = new QObject(this);
  m_pluginInterface->setObjectName("pluginInterface");
  m_interface = new KMFPluginInterface(m_pluginInterface);

  //KMF::Tools::printChilds(m_pluginInterface);

  KService::List offers = KServiceTypeTrader::self()->query("KMediaFactory/Plugin");
  QStringList files = KGlobal::dirs()->findAllResources("appdata", "tools/services/*.desktop");
  foreach(QString file, files)
  {
    kDebug() << "Custom service: " << file;
    offers << KService::Ptr(new KService(file));
  }

  foreach(KService::Ptr service, offers)
  {
    KPluginLoader loader(*service);
    KPluginFactory* factory = loader.factory();
    if (!factory)
    {
       kDebug() << "Loading factory failed:" << loader.errorString();
    }
    else
    {
      QVariant arg = qVariantFromValue(service);

      KMF::Plugin* plugin = factory->create<KMF::Plugin>(m_pluginInterface,
          QVariantList() << arg);
      if (plugin)
      {
        m_supportedProjects += plugin->supportedProjectTypes();
        kDebug() << "Loaded plugin " << plugin->objectName();
      }
      else
      {
        kDebug() << "Loading plugin failed:" << service->name();
      }
    }
  }
  KMF::Tools::removeDuplicates(&m_supportedProjects);
}

KMF::PluginList KMFApplication::plugins()
{
  KMF::PluginList plugins;
  const QObjectList& list = kmfApp->pluginInterface()->children();

  for(int i = 0; i < list.size(); ++i)
  {
    if(list[i]->inherits("KMF::Plugin"))
      plugins.append((KMF::Plugin*)list[i]);
  }
  return plugins;
}

void KMFApplication::finalize()
{
  // Save the url for session handling
  m_url = m_project->url();

  delete m_project;
  m_project = 0;

  const KMF::PluginList list = plugins();
  for(KMF::PluginList::ConstIterator obj = list.begin();
      obj != list.end(); ++obj)
  {
    (*obj)->init("");
    delete (*obj);
  }

  delete m_interface;
  m_interface = 0;
  delete m_pluginInterface;
  m_pluginInterface = 0;
}

// From kapplication.cpp - modified to accept env variables
static int startServiceInternal(const char *_function,
                                const QString& _name, const QStringList &URLs,
                                QStringList& envs,
                                QString *error, QString *serviceName, int *pid,
                                const QByteArray& startup_id, bool noWait)
{
  QString function = QLatin1String(_function);
  org::kde::KLauncher *launcher = KToolInvocation::klauncher();
  QDBusMessage msg = QDBusMessage::createMethodCall(launcher->service(),
      launcher->path(),
      launcher->interface(),
      function);
  msg << _name << URLs;
#ifdef Q_WS_X11
  if (QX11Info::display())
  {
    QByteArray dpystring(XDisplayString(QX11Info::display()));
    envs << QString::fromLatin1( QByteArray("DISPLAY=") + dpystring );
  }
  else if( getenv( "DISPLAY" ))
  {
    QByteArray dpystring( getenv( "DISPLAY" ));
    envs << QString::fromLatin1( QByteArray("DISPLAY=") + dpystring );
  }
#endif
  msg << envs;
#if defined Q_WS_X11
  // make sure there is id, so that user timestamp exists
  msg << QString( startup_id.isEmpty() ? KStartupInfo::createNewStartupId()
      : startup_id );
#else
  msg << QString();
#endif
  if( !function.startsWith( QLatin1String("kdeinit_exec") ) )
      msg << noWait;

  QDBusMessage reply = QDBusConnection::sessionBus().call(msg);
  if ( reply.type() != QDBusMessage::ReplyMessage )
  {
    QString args;

    if(reply.arguments().count() > 0)
      args = reply.arguments().at(0).toString();
    kDebug() << i18n("KLauncher could not be reached via D-Bus, "  \
        "error when calling %1:\n%2\n",function, args) << error;
    return EINVAL;
  }

  if (noWait)
    return 0;

  Q_ASSERT(reply.arguments().count() == 4);
  if (serviceName)
    *serviceName = reply.arguments().at(1).toString();
  if (error)
    *error = reply.arguments().at(2).toString();
  if (pid)
    *pid = reply.arguments().at(3).toInt();
  return reply.arguments().at(0).toInt();
}

int KMFApplication::startServiceByDesktopPath(const QString& _name,
                                              const QString &URL,
                                              QStringList& envs,
                                              QString *error,
                                              QString *serviceName,
                                              int *pid,
                                              const QByteArray& startup_id,
                                              bool noWait)
{
  QStringList URLs;
  if (!URL.isEmpty())
    URLs.append(URL);
  return startServiceInternal("start_service_by_desktop_path"
      "(QString,QStringList,QValueList<QCString>,QCString ,bool)",
      _name, URLs, envs, error, serviceName, pid, startup_id, noWait);
}
