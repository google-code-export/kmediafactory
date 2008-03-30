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
#ifndef KMEDIAFACTORY_PLUGIN_H
#define KMEDIAFACTORY_PLUGIN_H

#include "uiinterface.h"
#include "projectinterface.h"
#include <kservice.h>
#include <kxmlguiclient.h>
#include <kpluginfactory.h>
#include <QObject>

class QDomElement;
class KConfigSkeleton;

namespace KMF
{
  struct ConfigPage
  {
    ConfigPage() : page(0), config(0) {};
    QWidget* page;
    KConfigSkeleton* config;
    QString itemName;
    QString pixmapName;
  };

  class KDE_EXPORT Plugin : public QObject, public KXMLGUIClient
  {
      Q_OBJECT
    public:
      explicit Plugin(QObject *parent = 0);
      ~Plugin();

      UiInterface* uiInterface() const;
      ProjectInterface* projectInterface() const;

    public slots:
      virtual void init(const QString &type) = 0;

    public:
      virtual QStringList supportedProjectTypes() const = 0;
      virtual MediaObject* createMediaObject(const QDomElement&);
      virtual const ConfigPage* configPage() const;

    protected:
      void deleteChildren();
  };

  typedef QList<Plugin*> PluginList;
} // KMF namespace

Q_DECLARE_METATYPE(KService::Ptr)

#define K_EXPORT_KMEDIAFACTORY_PLUGIN(libname, classname) \
K_PLUGIN_FACTORY(factory, registerPlugin<classname>();)   \
K_EXPORT_PLUGIN(factory("kmediafactory_plugin_" #libname))

#endif // KMEDIAFACTORY_PLUGIN_H
