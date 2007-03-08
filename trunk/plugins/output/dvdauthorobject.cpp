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
#include "dvdauthorobject.h"
#include "outputplugin.h"
#include <kmimetype.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kdebug.h>
#include <QPixmap>

DvdAuthorObject::DvdAuthorObject(QObject* parent)
  : OutputObject(parent)
{
  setObjectName("dvdauthor");
  setTitle(i18n("DVDAuthor project"));
}

DvdAuthorObject::~DvdAuthorObject()
{
  KMF::UiInterface *ui = uiInterface();
  if(ui)
    ui->removeOutputObject(this);
}

void DvdAuthorObject::actions(QList<KAction*>&) const
{
  //actionList.append(dvdAuthorProperties);
}

void DvdAuthorObject::fromXML(const QDomElement&)
{
}

int DvdAuthorObject::timeEstimate() const
{
  return TotalPoints;
}

bool DvdAuthorObject::make(QString)
{
  uiInterface()->message(KMF::OK, i18n("DVDAuthor project ready"));
  uiInterface()->progress(TotalPoints);
  return true;
}

void DvdAuthorObject::toXML(QDomElement&) const
{
}

QPixmap DvdAuthorObject::pixmap() const
{
  return KIconLoader::global()->loadIcon("dvd_unmount", K3Icon::NoGroup,
                                                        K3Icon::SizeLarge);
}

#include "dvdauthorobject.moc"
