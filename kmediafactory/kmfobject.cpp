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
#include "kmfobject.h"
#include "plugin.h"
#include "projectinterface.h"
#include "uiinterface.h"


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
