//**************************************************************************
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
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************

#ifndef KROSSPROJECTINTERFACE_H
#define KROSSPROJECTINTERFACE_H

#include <QObject>
#include "kmediafactory/projectinterface.h"

class KrossProjectInterface : public QObject
{
  Q_OBJECT
  public:
    KrossProjectInterface(QObject *parent, KMF::ProjectInterface* projectIf);
    ~KrossProjectInterface();

  public slots:
    QList<MediaObject*> mediaObjects();
    QString title();
    void setTitle(QString title);
    QString projectDir(const QString& subDir = "");
    void cleanFiles(const QString& subDir, const QStringList& files);
    void setDirty(KMF::ProjectInterface::DirtyType type);
    QString type();
    QString lastSubType();
    QDateTime lastModified(KMF::ProjectInterface::DirtyType type);
    int serial();

  private:
    KMF::ProjectInterface* m_projectIf;
};

#endif // KROSSPROJECTINTERFACE_H
