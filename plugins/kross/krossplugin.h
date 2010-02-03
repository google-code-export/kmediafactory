// **************************************************************************
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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
// **************************************************************************

#ifndef KROSSPLUGIN_H
#define KROSSPLUGIN_H

#include <Kross/Object>

#include <kmediafactory/plugin.h>

namespace Kross
{
class Action;
}
class KrossUiInterface;
class KrossProjectInterface;

class KrossPlugin : public KMF::Plugin
{
    Q_ENUMS(DialogType)
    Q_OBJECT

    public:
        // From KMessageBox
        enum DialogType {
            Information = 5,
            Sorry = 7,
            Error = 8
        };

        KrossPlugin(QObject *parent, const QVariantList &);
        ~KrossPlugin();

    public slots:
        virtual void init(const QString &type);

    public:
        virtual QStringList supportedProjectTypes() const;
        virtual KMF::MediaObject*createMediaObject(const QDomElement &);
        virtual const KMF::ConfigPage*configPage() const;
        QMap<QObject *, QVariantList>*actionMap()
        {
            return &m_actionMap;
        };
        void addActions(QList<QAction *> *actionList, QStringList actions);

    public slots:
        // script functions
        void registerPlugin(Kross::Object::Ptr plugin);
        QObject*interface();

    protected slots:
        void actionTriggered();

    private:
        Kross::Action *m_action;
        mutable Kross::Object::Ptr m_plugin;
        QMap<QObject *, QVariantList> m_actionMap;
};

#endif // KROSSPLUGIN_H
