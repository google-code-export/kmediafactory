//**************************************************************************
//   Copyright (C) 2004-2006 by Petri Damsten
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
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************
#ifndef KMFOBJECT_H
#define KMFOBJECT_H

#include <kdemacros.h>
#include <QAction>
#include <QObject>
#include <QPixmap>
#include <QDomElement>

namespace KMF
{
  class UiInterface;
  class ProjectInterface;
  class Plugin;

  class KDE_EXPORT Object : public QObject
  {
    public:
      Object(QObject* parent):
          QObject(parent), m_plg(0), m_uiIf(0), m_prjIf(0) {};
      virtual void toXML(QDomElement&) const {};
      virtual bool fromXML(const QDomElement&) { return false; };
      virtual QPixmap pixmap() const = 0;
      virtual void actions(QList<QAction*>&) const {};
      virtual bool make(QString) { return false; };
      virtual QMap<QString, QString> subTypes() const
          { return QMap<QString, QString>(); };
      virtual int timeEstimate() const { return 0; };

      const QString& title() const { return m_title; };
      void setTitle(const QString& title) { m_title = title; };
      Plugin* plugin() const;
      UiInterface* uiInterface() const;
      ProjectInterface* projectInterface() const;
      QVariant call(const QString& func, QVariantList args = QVariantList());

    public slots:
      virtual void clean() {};

    private:
      QString m_title;
      mutable Plugin* m_plg;
      mutable UiInterface* m_uiIf;
      mutable ProjectInterface* m_prjIf;
  };
};

#endif // KMFOBJECT_H
