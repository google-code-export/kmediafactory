// **************************************************************************
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
// **************************************************************************

#ifndef KMFAPPLICATION_H
#define KMFAPPLICATION_H

#include <QtCore/QMap>

#include <KApplication>
#include <KUrl>

#include <kmediafactorysettings.h>
#include <kmediafactory/plugin.h>
#include "kmfplugininterface.h"
#include "kmfproject.h"

class KMediaFactory;
class QObject;
class KMFPluginInterface;
class KCmdLineArgs;
class KMFDbusInterface;

/**
 * @short KMediaFactory application class
 * @author Petri Damsten <petri.damsten@iki.fi>
 */

#define kmfApp ((KMFApplication *)qApp)
#define KMFWINID (kmfApp->mainWidget()->winId())
#define CHECK_IF_STOPPED(result) { \
        kapp->processEvents(); \
        if (kmfApp->interface()->stopped()) { \
            return result; \
        }; \
}

class KMFApplication : public KApplication
{
    public:
        KMFApplication();
        ~KMFApplication();

        void init();
        void loadPlugins();
        KMFProject*newProject();
        KMFProject*project()
        {
            return m_project;
        };
        QObject*pluginInterface()
        {
            return m_pluginInterface;
        };
        KMFPluginInterface*interface()
        {
            return m_interface;
        };
        KMediaFactory*mainWindow()
        {
            return m_mainWin;
        };
        QWidget*widget()
        {
            return (QWidget *)m_mainWin;
        };
        const KUrl&url()
        {
            return m_url;
        };
        KMF::PluginList plugins();
        void finalize();
        KConfigBase*config()
        {
            return KMediaFactorySettings::self()->config();
        };
        const QStringList&supportedProjects()
        {
            return m_supportedProjects;
        };
        int startServiceByDesktopPath(const QString &_name, const QString &URL, QStringList &envs,
            QString *error = 0, QString *serviceName = 0, int *pid = 0,
            const QByteArray &startup_id = "",
            bool noWait = false);

    private:
        KMediaFactory *m_mainWin;
        KMFProject *m_project;
        QObject *m_pluginInterface;
        KMFPluginInterface *m_interface;
        KUrl m_url;
        QStringList m_supportedProjects;
        KMFDbusInterface *iface;
};

#endif // KMFAPPLICATION_H
