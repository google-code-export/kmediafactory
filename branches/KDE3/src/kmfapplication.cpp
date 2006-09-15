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
#include "kmfapplication.h"
#include "kmfuiinterface.h"
#include "kmfprojectinterface.h"
#include "kmediafactory.h"
#include "kmftools.h"
#include "kmfdcopinterface.h"
#include <qobjectlist.h>
#include <qfileinfo.h>
#include <kcmdlineargs.h>
#include <ktrader.h>
#include <kstandarddirs.h>
#include <kparts/componentfactory.h>
#include <kdebug.h>
#include <kmainwindow.h>
#include <klocale.h>
#include <dcopclient.h>
#if defined Q_WS_X11
#include <kstartupinfo.h>
#include <X11/Xlib.h>
#endif

KMFApplication::KMFApplication()
  : KApplication(), m_mainWin(0), m_project(0), m_pluginInterface(0),
    m_uiInterface(0), m_projectInterface(0)
{
  iface = new KMFDcopInterface();
}

KMFApplication::~KMFApplication()
{
  delete iface;
}

void KMFApplication::init()
{
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  if(dcopClient()->isRegistered())
  {
    dcopClient()->registerAs(name());
    dcopClient()->setDefaultObject(iface->objId());
  }

  loadPlugins();
  if(args->count() > 0)
    m_url = args->url(0);

  m_mainWin = new KMediaFactory();
  setMainWidget(m_mainWin);
  m_mainWin->show();
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
  m_pluginInterface = new QObject(this, "pluginInterface");
  m_uiInterface = new KMFUiInterface(m_pluginInterface,
                                     "ui interface");
  m_projectInterface = new KMFProjectInterface(m_pluginInterface,
                                               "project interface");

  KTrader::OfferList offers = KTrader::self()->query("KMediaFactory/Plugin");
  KTrader::OfferList::ConstIterator iter;

  for(iter = offers.begin(); iter != offers.end(); ++iter)
  {
    int error = 0;
    KService::Ptr service = *iter;
    KMF::Plugin* plugin =
        KParts::ComponentFactory::createInstanceFromService<KMF::Plugin>
        (service, m_pluginInterface, service->name(), QStringList(), &error);
    if (plugin)
    {
      m_supportedProjects += plugin->supportedProjectTypes();
      kdDebug() << "Loaded plugin " << plugin->name() << endl;
    }
    else
    {
      switch (error)
      {
        case KParts::ComponentFactory::ErrNoServiceFound:
          kdDebug() << "No service implementing the given "
                    << "mimetype and fullfilling the given constraint "
                    << "expression can be found."
                    << endl;
          break;

        case KParts::ComponentFactory::ErrServiceProvidesNoLibrary:
          kdDebug() << "the specified service provides no shared library."
                    << endl;
          break;

        case KParts::ComponentFactory::ErrNoLibrary:
          kdDebug() << "the specified library could not be loaded." << endl;
          break;

        case KParts::ComponentFactory::ErrNoFactory:
          kdDebug() << "the library does not export a factory for creating "
                    << "components."
                    << endl;
          break;

        case KParts::ComponentFactory::ErrNoComponent:
          kdDebug() << "the factory does not support creating components of "
                    << "the specified type."
                    << endl;
          break;
      }
      kdDebug() << "Loading plugin '" << service->name()
                << "' failed, KLibLoader reported error: '" << endl
                << KLibLoader::self()->lastErrorMessage()
                << "'" << endl;
    }
  }
  KMF::Tools::removeDuplicates(&m_supportedProjects);
}

KMF::PluginList KMFApplication::plugins()
{
  KMF::PluginList plugins;
  const QObjectList *list = kmfApp->pluginInterface()->children();
  QObject *obj;

  for(QObjectListIt it(*list); (obj=it.current())!= 0; ++it)
  {
    if(obj->inherits("KMF::Plugin"))
      plugins.append((KMF::Plugin*)obj);
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

  delete m_uiInterface;
  m_uiInterface = 0;
  delete m_projectInterface;
  m_projectInterface = 0;
  delete m_pluginInterface;
  m_pluginInterface = 0;
}

// From kapplication.cpp - modifies to accept env variables

static int startServiceInternal(const QCString &function,
                                const QString& _name,
                                const QStringList &URLs,
                                QValueList<QCString> &envs,
                                QString *error,
                                QCString *dcopService,
                                int *pid,
                                const QCString& startup_id,
                                bool noWait)
{
  struct serviceResult
  {
    int result;
    QCString dcopName;
    QString error;
    pid_t pid;
  };

  // Register app as able to send DCOP messages
  DCOPClient *dcopClient;
  if (kapp)
    dcopClient = kapp->dcopClient();
  else
    dcopClient = new DCOPClient;

  if (!dcopClient->isAttached())
  {
    if (!dcopClient->attach())
    {
      if (error)
        *error = i18n("Could not register with DCOP.\n");
      return -1;
    }
  }
  QByteArray params;
  QDataStream stream(params, IO_WriteOnly);
  stream << _name << URLs;
  QCString replyType;
  QByteArray replyData;
  QCString _launcher = KApplication::launcher();
#ifdef Q_WS_X11
  if (qt_xdisplay())
  {
    QCString dpystring(XDisplayString(qt_xdisplay()));
    envs.append( QCString("DISPLAY=") + dpystring );
   }
   else if( getenv( "DISPLAY" ))
   {
     QCString dpystring( getenv( "DISPLAY" ));
     envs.append( QCString("DISPLAY=") + dpystring );
   }
#endif
   stream << envs;
#if defined Q_WS_X11
   // make sure there is id, so that user timestamp exists
   stream << ( startup_id.isEmpty() ?
      KStartupInfo::createNewStartupId() : startup_id );
#endif
   if( function.left( 12 ) != "kdeinit_exec" )
       stream << noWait;

   if (!dcopClient->call(_launcher, _launcher,
        function, params, replyType, replyData))
   {
     if (error)
       *error = i18n("KLauncher could not be reached via DCOP.\n");
     if (!kapp)
       delete dcopClient;
     return -1;
   }
   if (!kapp)
     delete dcopClient;

   if (noWait)
     return 0;

   QDataStream stream2(replyData, IO_ReadOnly);
   serviceResult result;
   stream2 >> result.result >> result.dcopName >> result.error >> result.pid;
   if (dcopService)
     *dcopService = result.dcopName;
   if (error)
     *error = result.error;
   if (pid)
     *pid = result.pid;
   return result.result;
}

int KMFApplication::startServiceByDesktopPath(const QString& _name,
                                              const QString &URL,
                                              QValueList<QCString> &envs,
                                              QString *error,
                                              QCString *dcopService,
                                              int *pid,
                                              const QCString& startup_id,
                                              bool noWait)
{
  QStringList URLs;
  if (!URL.isEmpty())
    URLs.append(URL);
  return startServiceInternal("start_service_by_desktop_path"
      "(QString,QStringList,QValueList<QCString>,QCString ,bool)",
      _name, URLs, envs, error, dcopService, pid, startup_id, noWait);
}
