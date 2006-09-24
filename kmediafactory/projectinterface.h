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
#ifndef PROJECTINTERFACE_H
#define PROJECTINTERFACE_H

#include "kmfobject.h"
#include <qdatetime.h>

class QDomElement;
class MediaObject;

namespace KMF
{
  class ProjectInterface :public QObject
  {
      Q_OBJECT
    public:
      enum DirtyType { DirtyMedia = 1, DirtyTemplate = 2, DirtyOutput = 4,
        DirtyMediaOrTemplate = 3, DirtyAny = 7 };

    public:
      ProjectInterface(QObject* parent);

      virtual QList<MediaObject*>* mediaObjects() = 0;
      virtual QString title() = 0;
      virtual void setTitle(QString title) = 0;
      virtual QString projectDir(const QString& subDir = "") = 0;
      virtual void cleanFiles(const QString& subDir,
                              const QStringList& files) = 0;
      virtual void setDirty(DirtyType type) = 0;
      virtual QString type() = 0;
      virtual QString lastSubType() = 0;
      virtual QDateTime lastModified(DirtyType type) = 0;
      virtual int serial() = 0;
  };
};

#endif
