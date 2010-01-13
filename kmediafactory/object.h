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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//**************************************************************************

#ifndef KMFOBJECT_H
#define KMFOBJECT_H

#include <kdemacros.h>
#include <QObject>
#include <QPixmap>
#include <QVariantList>
#include <QDomElement>

class QAction;

namespace KMF
{
  class PluginInterface;
  class Plugin;

  class KDE_EXPORT Object : public QObject
  {
      Q_OBJECT
      Q_PROPERTY(QString title READ title WRITE setTitle)
      Q_PROPERTY(QPixmap pixmap READ pixmap)
    public:
      explicit Object(QObject* parent);
      ~Object();

      virtual void toXML(QDomElement*) const;
      virtual bool fromXML(const QDomElement&);
      virtual void actions(QList<QAction*>*) const;
      virtual bool prepare(const QString&);
      virtual void finished();

      void setTitle(const QString& title);
      Plugin* plugin() const;
      PluginInterface* interface() const;
      uint msgId();
      uint newMsgId();

    public slots:
      // Help Kross plugin declaring these as slots
      virtual QVariant call(const QString& func, QVariantList args = QVariantList());
      virtual QString title() const;
      virtual QPixmap pixmap() const = 0;
      virtual QString information() const { return QString(); }
      virtual QMap<QString, QString> subTypes() const;
      virtual void clean();

    private:
      class Private;
      Private *const d;
  };
}

Q_DECLARE_METATYPE(QDomElement)

#endif // KMFOBJECT_H
