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
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************

#include "kmfobject.h"
#include "plugin.h"
#include "projectinterface.h"
#include "uiinterface.h"

class Object::Private
{
  public:
    Object(QObject* parent): 
      QObject(parent), 
      m_plg(0), 
      m_uiIf(0), 
      m_prjIf(0) {};

    QString title;
    Plugin* plugin;
    PluginInterface* interface;
};

Object(QObject* parent): QObject(parent), m_plg(0), m_uiIf(0), m_prjIf(0) 
{
}

void KMF::Object::toXML(QDomElement*) const 
{
}

bool KMF::Object::fromXML(const QDomElement&) 
{ 
  return false; 
}

void KMF::Object::actions(QList<QAction*>&) const 
{
}

bool KMF::Object::make(QString) 
{ 
  return false; 
}

QMap<QString, QString> KMF::Object::subTypes() const
{ 
  return QMap<QString, QString>(); 
}

int KMF::Object::timeEstimate() const 
{ 
  return 0; 
}

const QString& KMF::Object::title() const 
{ 
  return m_title; 
}

void KMF::Object::setTitle(const QString& title) 
{ 
  m_title = title; 
}

KMF::Plugin* KMF::Object::plugin() const
{
  if(!m_plg)
    m_plg = static_cast<KMF::Plugin*>(parent());
  return m_plg;
}

KMF::UiInterface* KMF::Object::uiInterface() const
{
  if(!m_uiIf)
    if(plugin())
      m_uiIf = plugin()->uiInterface();
  return m_uiIf;
}

KMF::ProjectInterface* KMF::Object::projectInterface() const
{
  if(!m_prjIf)
    if(plugin())
      m_prjIf = plugin()->projectInterface();
  return m_prjIf;
}

QVariant KMF::Object::call(const QString& func, QVariantList args)
{
  QVariant result;

  //kDebug() << func << args;
  QMetaObject::invokeMethod(this, func.toAscii(), Qt::DirectConnection,
                            Q_RETURN_ARG(QVariant, result),
                            Q_ARG(QVariantList, args));
  return result;
}
