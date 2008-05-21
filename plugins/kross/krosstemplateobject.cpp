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

#include "krosstemplateobject.h"

KrossTemplateObject::KrossTemplateObject(QObject* parent, Kross::Object::Ptr object)
 : KMF::TemplateObject(parent), ObjectMapper(object, this), m_object(object)
{
}

KrossTemplateObject::~KrossTemplateObject()
{
}

QVariant KrossTemplateObject::call(const QString & func, QVariantList args)
{
}

void KrossTemplateObject::toXML(QDomElement *) const
{
}

bool KrossTemplateObject::fromXML(const QDomElement &)
{
}

QPixmap KrossTemplateObject::pixmap() const
{
}

void KrossTemplateObject::actions(QList< QAction * > *) const
{
}

bool KrossTemplateObject::prepare(QString )
{
}

void KrossTemplateObject::finished()
{
}

QMap< QString, QString > KrossTemplateObject::subTypes() const
{
}

QImage KrossTemplateObject::preview(const QString &)
{
}

QStringList KrossTemplateObject::menus()
{
}

bool KrossTemplateObject::clicked()
{
}

#include "krosstemplateobject.moc"

