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

#include "krossmediaobject.h"

KrossMediaObject::KrossMediaObject(QObject* parent, Kross::Object::Ptr object)
 : KMF::MediaObject(parent), ObjectMapper(object, this), m_object(object)
{
}

KrossMediaObject::~KrossMediaObject()
{
}

QVariant KrossMediaObject::call(const QString & func, QVariantList args)
{
}

void KrossMediaObject::toXML(QDomElement *) const
{
}

bool KrossMediaObject::fromXML(const QDomElement &)
{
}

QPixmap KrossMediaObject::pixmap() const
{
}

void KrossMediaObject::actions(QList< QAction * > *) const
{
}

bool KrossMediaObject::prepare(const QString& type)
{
}

void KrossMediaObject::finished()
{
}

QMap< QString, QString > KrossMediaObject::subTypes() const
{
}

QImage KrossMediaObject::preview(int chapter) const
{
}

QString KrossMediaObject::text(int chapter) const
{
}

int KrossMediaObject::chapters() const
{
}

uint64_t KrossMediaObject::size() const
{
}

QTime KrossMediaObject::duration() const
{
}

QTime KrossMediaObject::chapterTime(int chapter) const
{
}

#include "krossmediaobject.moc"
