// **************************************************************************
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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
// **************************************************************************

#include "krossmediaobject.h"

#include <kmftime.h>
#include <kmftools.h>
#include "krossplugin.h"

KrossMediaObject::KrossMediaObject(QObject *parent, Kross::Object::Ptr object)
    : KMF::MediaObject(parent)
    , m_object(object)
{
}

KrossMediaObject::~KrossMediaObject()
{
}

QVariant KrossMediaObject::call(const QString &func, QVariantList args)
{
    return m_object->callMethod(func, args);
}

void KrossMediaObject::toXML(QDomElement *elem) const
{
    elem->appendChild(KMF::Tools::string2XmlElement(m_object->callMethod("toXML").toString()));
}

bool KrossMediaObject::fromXML(const QDomElement &elem)
{
    return m_object->callMethod("fromXML", QVariantList() <<
            KMF::Tools::xmlElement2String(elem)).toBool();
}

QPixmap KrossMediaObject::pixmap() const
{
    return QPixmap::fromImage(KMF::Tools::variantList2Image(m_object->callMethod("pixmap")));
}

void KrossMediaObject::actions(QList<QAction *> *actions) const
{
    KrossPlugin *p = qobject_cast<KrossPlugin *>(plugin());

    p->addActions(actions, m_object->callMethod("actions").toStringList());
}

bool KrossMediaObject::prepare(const QString &type)
{
    return m_object->callMethod("prepare", QVariantList() << type).toBool();
}

void KrossMediaObject::finished()
{
    m_object->callMethod("finished");
}

QMap<QString, QString> KrossMediaObject::subTypes() const
{
    return KMF::Tools::variantMap2StringMap(m_object->callMethod("subTypes").toMap());
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
    return KMF::Tools::variantList2Image(m_object->callMethod("preview", QVariantList() << chapter));
}

QString KrossMediaObject::text(int chapter) const
{
    return m_object->callMethod("text", QVariantList() << chapter).toString();
}

int KrossMediaObject::chapters() const
{
    return m_object->callMethod("chapters").toInt();
}

uint64_t KrossMediaObject::size() const
{
    return m_object->callMethod("size").toULongLong();
}

QTime KrossMediaObject::duration() const
{
    return KMF::Time(m_object->callMethod("duration").toDouble());
}

QTime KrossMediaObject::chapterTime(int chapter) const
{
    return KMF::Time(m_object->callMethod("chapterTime", QVariantList() << chapter).toDouble());
}

#include "krossmediaobject.moc"
