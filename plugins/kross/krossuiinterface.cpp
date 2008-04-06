//**************************************************************************
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
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************

#include "krossuiinterface.h"

KrossUiInterface::KrossUiInterface(QObject *parent, KMF::UiInterface* uiIf)
 : QObject(parent), m_uiIf(uiIf)
{
}

KrossUiInterface::~KrossUiInterface()
{
}

//bool KrossUiInterface::addMediaAction(QAction* action, const QString& group = "") const

bool KrossUiInterface::addMediaObject(Kross::Object::Ptr media) const
{
  return false;
}

bool KrossUiInterface::addTemplateObject(Kross::Object::Ptr tob)
{
  return false;
}

bool KrossUiInterface::addOutputObject(Kross::Object::Ptr oob)
{
  return false;
}

bool KrossUiInterface::removeMediaObject(Kross::Object::Ptr media) const
{
  return false;
}

bool KrossUiInterface::removeTemplateObject(Kross::Object::Ptr tob)
{
  return false;
}

bool KrossUiInterface::removeOutputObject(Kross::Object::Ptr oob)
{
  return false;
}

bool KrossUiInterface::message(KMF::MsgType type, const QString& msg)
{
  return m_uiIf->message(type, msg);
}

bool KrossUiInterface::progress(int advance)
{
  return m_uiIf->progress(advance);
}

bool KrossUiInterface::setItemTotalSteps(int totalSteps)
{
  return m_uiIf->setItemTotalSteps(totalSteps);
}

bool KrossUiInterface::setItemProgress(int progress)
{
  return m_uiIf->setItemProgress(progress);
}

QObject* KrossUiInterface::logger()
{
  return m_uiIf->logger();
}

#include "krossuiinterface.moc"
