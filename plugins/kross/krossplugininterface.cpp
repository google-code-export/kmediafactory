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

#include "krossplugininterface.h"
#include "krossplugin.h"
#include <KDebug>
#include <KAction>
#include <KActionCollection>

KrossPluginInterface::KrossPluginInterface(QObject *parent, KMF::PluginInterface* interface)
 : QObject(parent), m_interface(interface)
{
}

KrossPluginInterface::~KrossPluginInterface()
{
}

QList<KMF::MediaObject*> KrossPluginInterface::mediaObjects()
{
  return m_interface->mediaObjects();
}

QString KrossPluginInterface::title()
{
  return m_interface->title();
}

void KrossPluginInterface::setTitle(QString title)
{
  m_interface->setTitle(title);
}

QString KrossPluginInterface::projectDir(const QString& subDir)
{
  return m_interface->projectDir(subDir);
}

void KrossPluginInterface::setDirty(KMF::PluginInterface::DirtyType type)
{
  return m_interface->setDirty(type);
}

QString KrossPluginInterface::projectType()
{
  return m_interface->projectType();
}

QString KrossPluginInterface::lastSubType()
{
  return m_interface->lastSubType();
}

QDateTime KrossPluginInterface::lastModified(KMF::PluginInterface::DirtyType type)
{
  return m_interface->lastModified(type);
}

int KrossPluginInterface::serial()
{
  return m_interface->serial();
}

bool KrossPluginInterface::addMediaAction(const QString& icon, const QString& text,
                                      const QString& shortcut, const QString& name,
                                      Kross::Object::Ptr obj, const QString& method)
{
  KrossPlugin* plugin = qobject_cast<KrossPlugin*>(parent());
  QAction* act = new KAction(KIcon(icon), text, plugin->parent());
  act->setShortcut(QKeySequence::fromString(shortcut));
  //kDebug() << "addAction" << name << act;
  plugin->actionCollection()->addAction(name, act);
  connect(act, SIGNAL(triggered()), plugin, SLOT(actionTriggered()));
  QVariant arg;
  arg.setValue(obj);
  plugin->actionMap()->insert(act, QVariantList() << arg << method);
  return m_interface->addMediaAction(act);
}

void KrossPluginInterface::setActionEnabled(const QString& name, bool enabled)
{
  KrossPlugin* plugin = qobject_cast<KrossPlugin*>(parent());
  QAction* action = plugin->actionCollection()->action(name);
  if(!action)
    return;
  action->setEnabled(enabled);
}

bool KrossPluginInterface::addMediaObject(Kross::Object::Ptr media) const
{
  Q_UNUSED(media)
  return false;
}

bool KrossPluginInterface::addTemplateObject(Kross::Object::Ptr tob)
{
  Q_UNUSED(tob)
  return false;
}

bool KrossPluginInterface::addOutputObject(Kross::Object::Ptr oob)
{
  Q_UNUSED(oob)
  return false;
}

bool KrossPluginInterface::removeMediaObject(Kross::Object::Ptr media) const
{
  Q_UNUSED(media)
  return false;
}

bool KrossPluginInterface::removeTemplateObject(Kross::Object::Ptr tob)
{
  Q_UNUSED(tob)
  return false;
}

bool KrossPluginInterface::removeOutputObject(Kross::Object::Ptr oob)
{
  Q_UNUSED(oob)
  return false;
}

bool KrossPluginInterface::message(KMF::PluginInterface::MsgType type, const QString& msg)
{
  return m_interface->message(type, msg);
}

bool KrossPluginInterface::progress(int advance)
{
  return m_interface->progress(advance);
}

bool KrossPluginInterface::setItemTotalSteps(int totalSteps)
{
  return m_interface->setItemTotalSteps(totalSteps);
}

bool KrossPluginInterface::setItemProgress(int progress)
{
  return m_interface->setItemProgress(progress);
}

QObject* KrossPluginInterface::logger()
{
  return m_interface->logger();
}

void KrossPluginInterface::addMediaObjectFromXML(const QString& xml)
{
  kDebug() << xml;
  m_interface->addMediaObject(xml);
}

void KrossPluginInterface::setTemplateFromXML(const QString& xml)
{
  m_interface->selectTemplate(xml);
}

void KrossPluginInterface::setOutputFromXML(const QString& xml)
{
  m_interface->selectOutput(xml);
}

QStringList KrossPluginInterface::getOpenFileNames(const QString &startDir, const QString &filter, 
                                               const QString &caption)
{
  return m_interface->getOpenFileNames(startDir, filter, caption);
}

void KrossPluginInterface::debug(const QString &txt)
{
  return m_interface->debug(txt);
}

int  KrossPluginInterface::messageBox(const QString &caption, const QString &txt, int type)
{
  return m_interface->messageBox(caption, txt, type);
}

QObject* KrossPluginInterface::progressDialog(const QString &caption, const QString &label, 
                                          int maximum)
{
  return m_interface->progressDialog(caption, label, maximum);
}

QObject* KrossPluginInterface::progressDialog()
{
  return m_interface->progressDialog();
}

#include "krossplugininterface.moc"
