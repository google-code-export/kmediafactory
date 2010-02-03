// **************************************************************************
//   Copyright (C) 2008 Petri Damsten <damu@iki.fi>
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

#ifndef KROSSMEDIAOBJECT_H
#define KROSSMEDIAOBJECT_H

#include <Kross/Object>

#include <kmediafactory/plugininterface.h>

class KrossMediaObject : public KMF::MediaObject
{
    Q_OBJECT

    public:
        KrossMediaObject(QObject *parent, Kross::Object::Ptr mediaObject);
        ~KrossMediaObject();

        virtual void toXML(QDomElement *) const;
        virtual bool fromXML(const QDomElement &);
        virtual void actions(QList<QAction *> *) const;
        virtual bool prepare(const QString &);
        virtual void finished();

    public slots:
        virtual QVariant call(const QString &func, QVariantList args = QVariantList());
        virtual QString title() const;
        virtual QPixmap pixmap() const;
        virtual QMap<QString, QString> subTypes() const;
        virtual void clean();
        virtual QImage preview(int chapter = KMF::MediaObject::MainPreview) const;
        virtual QString text(int chapter = KMF::MediaObject::MainTitle) const;
        virtual int chapters() const;
        virtual uint64_t size() const;
        virtual QTime duration() const;
        virtual QTime chapterTime(int chapter) const;

    private:
        mutable Kross::Object::Ptr m_object;
};

#endif // KROSSMEDIAOBJECT_H
