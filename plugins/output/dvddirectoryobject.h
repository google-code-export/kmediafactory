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
#ifndef DVDDIRECTORYOBJECT_H
#define DVDDIRECTORYOBJECT_H

#include "dvdauthorobject.h"
#include <run.h>
#include <QFileInfo>
#include <QPixmap>

class KAction;

class DvdDirectoryObject : public DvdAuthorObject
{
  Q_OBJECT
  public:
    enum { TotalPoints = 1000 };

    DvdDirectoryObject(QObject *parent = 0);
    virtual ~DvdDirectoryObject();
    virtual void toXML(QDomElement* element) const;
    virtual bool fromXML(const QDomElement& element);
    virtual QPixmap pixmap() const;
    virtual void actions(QList<QAction*>* actionList) const;
    virtual int timeEstimate() const;
    virtual bool make(QString type);

  private slots:
    virtual void clean();

  private:
    QString m_buffer;
    KAction* dvdCleanDirectory;
    Run m_run;

    void progress(int points);
    bool isUpToDate(QString type);
};

#endif // DVDDIRECTORYOBJECT_H
