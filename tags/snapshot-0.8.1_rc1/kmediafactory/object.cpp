// **************************************************************************
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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
// **************************************************************************

#include "object.h"

#include "plugin.h"
#include "plugininterface.h"

class KMF::Object::Private
{
    public:
        Private() : plugin(0), interface(0)
        {
            msgid = KMF::PluginInterface::messageId();
        };

        QString title;
        Plugin *plugin;
        PluginInterface *interface;
        uint msgid;
};

KMF::Object::Object(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
}

KMF::Object::~Object()
{
    delete d;
}

void KMF::Object::toXML(QDomElement *) const
{
}

bool KMF::Object::fromXML(const QDomElement &)
{
    return false;
}

void KMF::Object::actions(QList<QAction *> *) const
{
}

bool KMF::Object::prepare(const QString &)
{
    return false;
}

void KMF::Object::finished()
{
}

QMap<QString, QString> KMF::Object::subTypes() const
{
    return QMap<QString, QString>();
}

QString KMF::Object::title() const
{
    return d->title;
}

void KMF::Object::setTitle(const QString &title)
{
    d->title = title;
}

KMF::Plugin *KMF::Object::plugin() const
{
    if (!d->plugin) {
        d->plugin = static_cast<KMF::Plugin *>(parent());
    }

    return d->plugin;
}

KMF::PluginInterface *KMF::Object::interface() const
{
    if (!d->interface) {
        if (plugin()) {
            d->interface = plugin()->interface();
        }
    }

    return d->interface;
}

QVariant KMF::Object::call(const QString &func, QVariantList args)
{
    QVariant result;

    // kDebug() << func << args;
    QMetaObject::invokeMethod(this, func.toAscii(), Qt::DirectConnection,
            Q_RETURN_ARG(QVariant, result),
            Q_ARG(QVariantList, args));
    return result;
}

uint KMF::Object::msgId()
{
    return d->msgid;
}

uint KMF::Object::newMsgId()
{
    return (d->msgid = KMF::PluginInterface::messageId());
}

void KMF::Object::clean()
{
}

#include "object.moc"
