//**************************************************************************
//   Copyright (C) 2008 by Petri Damsten <damu@iki.fi>
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

#include "krossoutputobject.h"

KrossOutputObject::KrossOutputObject(QObject* parent, Kross::Object::Ptr object)
 : KMF::OutputObject(parent), ObjectMapper(object, this), m_object(object)
{
}

KrossOutputObject::~KrossOutputObject()
{
}

QVariant KrossOutputObject::call(const QString & func, QVariantList args)
{
}

void KrossOutputObject::toXML(QDomElement *) const
{
}

bool KrossOutputObject::fromXML(const QDomElement &)
{
}

QPixmap KrossOutputObject::pixmap() const
{
}

void KrossOutputObject::actions(QList< QAction * > *) const
{
}

bool KrossOutputObject::prepare(QString )
{
}

void KrossOutputObject::finished()
{
}

QMap< QString, QString > KrossOutputObject::subTypes() const
{
}
#include "krossoutputobject.moc"
