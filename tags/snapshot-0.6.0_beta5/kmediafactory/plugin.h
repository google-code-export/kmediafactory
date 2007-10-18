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
#include <kxmlguiclient.h>
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
      Plugin(QObject *parent = 0);
      virtual ~Plugin();
      virtual MediaObject* createMediaObject(const QDomElement&)
        { return 0; };
      virtual const ConfigPage* configPage() const { return 0; };

      UiInterface* uiInterface();
      ProjectInterface* projectInterface();

    public slots:
      virtual void init(const QString &type) { m_type = type; };
      virtual QStringList supportedProjectTypes() = 0;

    protected:
      void deleteChildren();

    private:
      QString m_type;
  };

  typedef QList<Plugin*> PluginList;
};

#endif // KMEDIAFACTORY_PLUGIN_H
