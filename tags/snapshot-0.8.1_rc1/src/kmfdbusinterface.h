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

#ifndef KMFDBUSINTERFACE_H
#define KMFDBUSINTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QStringList>

class KProgressDialog;

class KMFDbusInterface : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kmediafactory.KMediaFactory")

    public : KMFDbusInterface(QObject *parent);
        virtual ~KMFDbusInterface();

        public Q_SLOTS :

        virtual void openProject(QString file);
        virtual void newProject();
        virtual void quit();

        virtual void setProjectTitle(const QString &title);
        virtual void setProjectType(const QString &type);
        virtual void setProjectAspectRatio(const QString &aspect);
        virtual void setProjectDirectory(const QString &dir);
        virtual QString projectTitle();
        virtual QString projectType();
        virtual QString projectDirectory(const QString &subdir);
        virtual QString projectAspectRatio();
        virtual int projectDuration();

        virtual bool saveThumbnail(int title, const QString &fileName);

        virtual void addMediaObject(const QString &xml);
        virtual void selectTemplate(const QString &xml);
        virtual void selectOutput(const QString &xml);

        virtual void start();

        // Script helpers
        virtual QStringList getOpenFileNames(const QString &startDir, const QString &filter,
            const QString &caption);
        virtual void debug(const QString &txt);
        virtual int  message(const QString &caption, const QString &txt, int type);

        // Progress dialog
        virtual void progressDialog(const QString &caption, const QString &label, int maximum);
        virtual void pdlgSetMaximum(int maximum);
        virtual void pdlgSetValue(int value);
        virtual void pdlgSetLabel(const QString &label);
        virtual void pdlgShowCancelButton(bool show);
        virtual bool pdlgWasCancelled();
        virtual void pdlgClose();
};

#endif
