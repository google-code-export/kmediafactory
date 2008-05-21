//**************************************************************************
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
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************

#ifndef KROSSTEMPLATEOBJECT_H
#define KROSSTEMPLATEOBJECT_H

#include <kmediafactory/plugininterface.h>
#include <kross/core/object.h>
#include "objectmapper.h"

class KrossTemplateObject : public KMF::TemplateObject, public ObjectMapper
{
  Q_OBJECT
  public:
    KrossTemplateObject(QObject *parent, Kross::Object::Ptr templateObject);
    ~KrossTemplateObject();

    virtual QVariant call(const QString& func, QVariantList args = QVariantList());
    virtual void toXML(QDomElement*) const;
    virtual bool fromXML(const QDomElement&);
    virtual QPixmap pixmap() const;
    virtual void actions(QList<QAction*>*) const;
    virtual bool prepare(QString);
    virtual void finished();
    virtual QMap<QString, QString> subTypes() const;

    virtual QImage preview(const QString& = "");
    virtual QStringList menus();
    virtual bool clicked();

  private:
    Kross::Object::Ptr m_object;
};

#endif // KROSSTEMPLATEOBJECT_H
