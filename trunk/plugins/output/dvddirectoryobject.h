// **************************************************************************
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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
// **************************************************************************

#ifndef DVDDIRECTORYOBJECT_H
#define DVDDIRECTORYOBJECT_H

#include <QtCore/QFileInfo>
#include <QtGui/QPixmap>

#include <run.h>
#include "dvdauthorobject.h"

class KAction;

class DvdDirectoryObject : public DvdAuthorObject
{
    Q_OBJECT

    public:
        enum { TotalPoints = 1000};

        DvdDirectoryObject(QObject *parent = 0);
        virtual ~DvdDirectoryObject();
        virtual void toXML(QDomElement *element) const;
        virtual bool fromXML(const QDomElement &element);
        virtual void actions(QList<QAction *> *actionList) const;
        virtual bool prepare(const QString &type);

    public slots:
        // Help Kross plugin declaring these as slots
        virtual QPixmap pixmap() const;
        virtual void clean();

    private:
        QString m_buffer;
        KAction *dvdCleanDirectory;
        Run m_run;

        void progress(int points);
        bool isUpToDate(QString type);
};

#endif // DVDDIRECTORYOBJECT_H
