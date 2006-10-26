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
#ifndef KMFAPPLICATION_H
#define KMFAPPLICATION_H

#include "kmfproject.h"
#include "kmflogger.h"
#include "kmediafactorysettings.h"
#include <kapplication.h>
#include <kurl.h>
#include <QMap>

class KMediaFactory;
class QObject;
class KMFUiInterface;
class KMFProjectInterface;
class KCmdLineArgs;
class KMFDbusInterface;

/**
 * @short KMediaFactory application class
 * @author Petri Damsten <petri.damsten@iki.fi>
 */

#define kmfApp ((KMFApplication*)qApp)
#define KMFWINID (kmfApp->mainWidget()->winId())

class KMFApplication : public KApplication
{
  public:
    KMFApplication();
    ~KMFApplication();

    void init();
    void loadPlugins();
    KMFProject* newProject();
    KMFProject* project() { return m_project; };
    QObject *pluginInterface() { return m_pluginInterface; };
    KMFUiInterface* uiInterface() { return m_uiInterface; };
    KMediaFactory* mainWindow() { return m_mainWin; };
    QWidget* widget() { return (QWidget*)m_mainWin; };
    const KUrl& url() { return m_url; };
    KMFLogger& logger() { return m_logger; };
    KMF::PluginList plugins();
    void finalize();
    KConfigBase* config() { return KMediaFactorySettings::self()->config(); };
    const QStringList& supportedProjects() { return m_supportedProjects; };
    int startServiceByDesktopPath(const QString& _name,
                                  const QString &URL,
                                  QStringList& envs,
                                  QString* error = 0,
                                  QString* serviceName = 0,
                                  int* pid = 0,
                                  const QByteArray& startup_id = "",
                                  bool noWait = false);

  private:
    KMediaFactory* m_mainWin;
    KMFProject* m_project;
    QObject* m_pluginInterface;
    KMFUiInterface* m_uiInterface;
    KMFProjectInterface* m_projectInterface;
    KUrl m_url;
    KMFLogger m_logger;
    QStringList m_supportedProjects;
    KMFDbusInterface* iface;
};

#endif // KMFAPPLICATION_H
