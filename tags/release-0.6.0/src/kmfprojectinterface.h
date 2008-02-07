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
#ifndef KMFPROJECTINTERFACE_H
#define KMFPROJECTINTERFACE_H

#include <kmediafactory/uiinterface.h>
#include <kmediafactory/projectinterface.h>

/**
@author Petri Damsten
*/
class KMFProjectInterface : public KMF::ProjectInterface
{
    Q_OBJECT
  public:
    KMFProjectInterface(QObject *parent = 0);
    virtual ~KMFProjectInterface();

    virtual QList<KMF::MediaObject*> mediaObjects();
    virtual QString title();
    virtual void setTitle(QString title);
    virtual QString projectDir(const QString& subDir = "");
    virtual void cleanFiles(const QString& subDir,
                            const QStringList& files);
    virtual void setDirty(KMF::ProjectInterface::DirtyType type);
    virtual QString type();
    virtual QString lastSubType();
    virtual QDateTime lastModified(KMF::ProjectInterface::DirtyType type);
    virtual int serial();
};

#endif
