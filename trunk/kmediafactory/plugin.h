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

#ifndef KMF_PLUGIN_H
#define KMF_PLUGIN_H

#include <KService>
#include <KXMLGUIClient>

#include "plugininterface.h"

class KConfigSkeleton;

namespace KMF
{
struct ConfigPage {
    ConfigPage() : page(0), config(0){};
    QWidget *page;
    KConfigSkeleton *config;
    QString itemName;
    QString itemDescription;
    QString pixmapName;
};

class KDE_EXPORT Plugin : public QObject, public KXMLGUIClient
{
    Q_OBJECT

    public:
        explicit Plugin(QObject *parent = 0);
        ~Plugin();

        PluginInterface*interface() const;
        virtual QStringList supportedProjectTypes() const = 0;
        virtual MediaObject*createMediaObject(const QDomElement &);
        virtual const ConfigPage*configPage() const;

    public slots:
        virtual void init(const QString &type) = 0;

    protected:
        void deleteChildren();
};

typedef QList<Plugin *> PluginList;
} // KMF namespace

Q_DECLARE_METATYPE(KService::Ptr)

#define K_EXPORT_KMEDIAFACTORY_PLUGIN(libname, classname) \
    K_PLUGIN_FACTORY(factory, registerPlugin < classname > ();)   \
    K_EXPORT_PLUGIN(factory("kmediafactory_plugin_" # libname))

#endif // KMEDIAFACTORY_PLUGIN_H
