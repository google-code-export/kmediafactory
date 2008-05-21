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
#include "krossplugin.h"
#include <kmftools.h>

KrossMediaObject::KrossMediaObject(QObject* parent, Kross::Object::Ptr object)
 : KMF::MediaObject(parent), ObjectMapper(object, this), m_object(object)
{
}

KrossMediaObject::~KrossMediaObject()
{
}

QVariant KrossMediaObject::call(const QString & func, QVariantList args)
{
  return m_object->callMethod(func, args);
}

void KrossMediaObject::toXML(QDomElement *elem) const
{
  elem->appendChild(KMF::Tools::string2xmlElement(m_object->callMethod("toXML").toString()));
}

bool KrossMediaObject::fromXML(const QDomElement &elem)
{
  return m_object->callMethod("fromXML", QVariantList() <<
         KMF::Tools::xmlElement2String(elem)).toBool();
}

QPixmap KrossMediaObject::pixmap() const
{
  return KMF::Tools::variantList2Pixmap(m_object->callMethod("pixmap"));
}

void KrossMediaObject::actions(QList<QAction*>* actions) const
{
  KrossPlugin* p = qobject_cast<KrossPlugin*>(plugin());
  p->addActions(actions, m_object->callMethod("actions").toStringList());
}

bool KrossMediaObject::prepare(const QString& type)
{
  return m_object->callMethod("prepare", QVariantList() << type).toBool();
}

void KrossMediaObject::finished()
{
  m_object->callMethod("finished");
}

QMap<QString, QString> KrossMediaObject::subTypes() const
{
  return KMF::Tools::variantMap2stringMap(m_object->callMethod("subTypes").toMap());
}

QString KrossMediaObject::title() const
{
  return m_object->callMethod("title").toString();
}

void KrossMediaObject::clean()
{
  m_object->callMethod("clean");
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
