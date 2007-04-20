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

#include "newstuffobject.h"
#include "templateplugin.h"
#include "templatepluginsettings.h"
#include <klocale.h>
#include <knewstuff2/engine.h>
#include <kapplication.h>
#include <kiconloader.h>

NewStuffObject::NewStuffObject(QObject* parent):
  KMF::TemplateObject(parent), m_newstuff(0)
{
  setObjectName("newstuff");
  setTitle(i18n("Get more..."));
  uiInterface()->addTemplateObject(this);
}

NewStuffObject::~NewStuffObject()
{
  KMF::UiInterface *ui = uiInterface();
  if(ui)
    uiInterface()->removeTemplateObject(this);
}

bool NewStuffObject::clicked()
{
  KNS::Engine *engine = new KNS::Engine();
  KNS::Entry::List entries = engine->downloadDialogModal();
  delete engine;
  return true;
}

QPixmap NewStuffObject::pixmap() const
{
  return KIconLoader::global()->loadIcon("get-hot-new-stuff", K3Icon::NoGroup,
      K3Icon::SizeLarge);
}

#include "newstuffobject.moc"
