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

#ifndef KROSSPLUGIN_H
#define KROSSPLUGIN_H

#include <kmediafactory/plugin.h>
#include <kross/core/object.h>

namespace Kross {
  class Action;
}
class KrossUiInterface;
class KrossProjectInterface;

class KrossPlugin : public KMF::Plugin
{
  Q_OBJECT
  public:
    KrossPlugin(QObject *parent, const QVariantList&);
    ~KrossPlugin();

  public slots:
    virtual void init(const QString &type);

  public:
    virtual QStringList supportedProjectTypes() const;
    virtual KMF::MediaObject* createMediaObject(const QDomElement&);
    virtual const KMF::ConfigPage* configPage() const;

  public slots: // script functions
    void registerPlugin(Kross::Object::Ptr plugin);
    QObject* uiInterface();
    QObject* projectInterface();

  private:
    Kross::Action *m_action;
    mutable Kross::Object::Ptr m_plugin;
    KrossUiInterface* m_uiIf;
    KrossProjectInterface* m_projectIf;
};

K_EXPORT_KMEDIAFACTORY_PLUGIN(kross, KrossPlugin);

#endif // KROSSPLUGIN_H

