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
#include "krossplugin.h"
#include <kmftools.h>
#include <KActionCollection>
#include <QTextStream>

KrossOutputObject::KrossOutputObject(QObject* parent, Kross::Object::Ptr object)
 : KMF::OutputObject(parent), ObjectMapper(object, this), m_object(object)
{
}

KrossOutputObject::~KrossOutputObject()
{
}

QVariant KrossOutputObject::call(const QString & func, QVariantList args)
{
  return m_object->callMethod(func, args);
}

void KrossOutputObject::toXML(QDomElement *elem) const
{
  elem->appendChild(KMF::Tools::string2xmlElement(m_object->callMethod("toXML").toString()));
}

bool KrossOutputObject::fromXML(const QDomElement &elem)
{
  return m_object->callMethod("fromXML", QVariantList() <<
         KMF::Tools::xmlElement2String(elem)).toBool();
}

QPixmap KrossOutputObject::pixmap() const
{
  return KMF::Tools::variantList2Pixmap(m_object->callMethod("pixmap"));
}

void KrossOutputObject::actions(QList<QAction*>* actions) const
{
  KrossPlugin* p = qobject_cast<KrossPlugin*>(plugin());
  p->addActions(actions, m_object->callMethod("actions").toStringList());
}

bool KrossOutputObject::prepare(const QString& type)
{
  return m_object->callMethod("prepare", QVariantList() << type).toBool();
}

void KrossOutputObject::finished()
{
  m_object->callMethod("finished");
}

QMap<QString, QString> KrossOutputObject::subTypes() const
{
  return KMF::Tools::variantMap2stringMap(m_object->callMethod("subTypes").toMap());
}

QString KrossOutputObject::title() const
{
  return m_object->callMethod("title").toString();
}

void KrossOutputObject::clean()
{
  m_object->callMethod("clean");
}

#include "krossoutputobject.moc"
