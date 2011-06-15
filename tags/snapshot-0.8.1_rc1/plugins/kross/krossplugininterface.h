// **************************************************************************
//   Copyright (C) 2008 Petri Damsten <damu@iki.fi>
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

#ifndef KROSSPLUGININTERFACE_H
#define KROSSPLUGININTERFACE_H

#include <QtCore/QObject>

#include <kdeversion.h>

#if KDE_IS_VERSION(4, 4, 0)
#include <Kross/Object>
#else
#include <kross/core/object.h>
#endif

#include <kmediafactory/plugininterface.h>

class KrossPluginInterface : public QObject
{
    Q_OBJECT

    public:
        KrossPluginInterface(QObject *parent, KMF::PluginInterface *interface);
        ~KrossPluginInterface();

    public slots:
        QList<KMF::MediaObject *> mediaObjects();
        KMF::TemplateObject*templateObject();
        KMF::OutputObject*outputObject();
        QString title();
        void setTitle(QString title);
        QString projectDir(const QString &subDir = "");
        QString projectType();
        void setDirty(KMF::DirtyType type);
        QString lastSubType();
        QDateTime lastModified(KMF::DirtyType type);
        int serial();
        QString newAction(const QString &name, const QString &icon, const QString &text,
            const QString &shortcut, Kross::Object::Ptr obj,
            const QString &method);
        bool addMediaAction(const QString &name);
        void setActionEnabled(const QString &name, bool enabled);
        uintptr_t addMediaObject(Kross::Object::Ptr media);
        uintptr_t addTemplateObject(Kross::Object::Ptr tob);
        uintptr_t addOutputObject(Kross::Object::Ptr oob);
        bool removeMediaObject(uintptr_t mediaId);
        bool removeTemplateObject(uintptr_t templateId);
        bool removeOutputObject(uintptr_t outputId);
        void addMediaObjectFromXML(const QString &xml);
        void setTemplateFromXML(const QString &xml);
        void setOutputFromXML(const QString &xml);

        uintptr_t addJob(Kross::Object::Ptr job, KMF::JobDependency dependency = KMF::None);
        uintptr_t addJob(Kross::Object::Ptr job, uintptr_t dependency);

        uint messageId();
        void message(uint id, KMF::MsgType type, const QString &msg = QString());
        void setMaximum(uint id, int maximum);
        void setValue(uint id, int value);
        void log(uint id, const QString &msg);
        // sub id hack
        uint subId(uint id);
        uint parent(uint id);

        // Plugin helpers
        QStringList getOpenFileNames(const QString &startDir, const QString &filter,
            const QString &caption);
        void debug(const QString &txt);
        int  messageBox(const QString &caption, const QString &txt, int type);
        QObject*progressDialog(const QString &caption, const QString &label, int maximum);
        QObject*progressDialog();

    private:
        KMF::PluginInterface *m_interface;
};

#endif // KROSSPLUGININTERFACE_H
