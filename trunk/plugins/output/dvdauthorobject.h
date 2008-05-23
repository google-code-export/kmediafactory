//**************************************************************************
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
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************

#ifndef DVDAUTHOROBJECT_H
#define DVDAUTHOROBJECT_H

#include <kmediafactory/plugin.h>
#include <QPixmap>
#include <QDomElement>

class DvdAuthorObject : public KMF::OutputObject
{
    Q_OBJECT
  public:
    enum { TotalPoints = 10 };
    DvdAuthorObject(QObject *parent = 0);
    virtual ~DvdAuthorObject();
    virtual void toXML(QDomElement* element) const;
    virtual bool fromXML(const QDomElement& element);
    virtual void actions(QList<QAction*>* actionList) const;
    virtual bool prepare(const QString& type);

  public slots:
    // Help Kross plugin declaring these as slots
    virtual QPixmap pixmap() const;

  private:
    //KAction* dvdAuthorProperties;
    QDomElement toElement(const QVariant& element);
};

#endif // DVDAUTHOROBJECT_H
