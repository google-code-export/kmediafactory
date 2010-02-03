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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//**************************************************************************

#include "kmfproject.h"

#include <QtCore/QDir>
#include <QtXml/QDomElement>

#include <KDebug>
#include <KLocale>
#include <KMessageBox>
#include <KSaveFile>
#include <KStandardDirs>
#include <KTemporaryFile>
#include <KIO/Job>
#include <KIO/NetAccess>

#include <kmediafactorysettings.h>
#include <kmftools.h>
#include "kmfapplication.h"

template <class S, class T>
void addMap(QMap<S, T>* map, const QMap<S, T>& add)
{
  QList<S> keys = add.keys();

  for(int i = 0; i < keys.size(); ++i)
    map->insert(keys[i], add[keys[i]]);
}

QDVD::VideoTrack::AspectRatio KMFProject::toAspectRatio(const QString &ar)
{
    // As detailed below, KConfigXT only saves combobox index!
    if (QString::fromLatin1("0")==ar)
        return QDVD::VideoTrack::Aspect_4_3;
    else if (QString::fromLatin1("1")==ar)
        return QDVD::VideoTrack::Aspect_16_9;
    else
        return ar==QDVD::VideoTrack::aspectRatioString(QDVD::VideoTrack::Aspect_16_9)
                    ? QDVD::VideoTrack::Aspect_16_9 : QDVD::VideoTrack::Aspect_4_3;
}

// KConfigXT when used with a combobox, only saves the combos current *index*
// to the config file - not the current combo text. This causes KMediaFactory
// headaches, as when the template is initialised (via the preinit signal), it
// checks if the string starts with "DVD", if not it calls the NewStuff plugin.
//
// Therefore, convert '0' to 'DVD-NTSC' (as its the first entry in the combo), and
// '1' to 'DVD-PAL'
static QString fixType(const QString &type)
{
    if (QString::fromLatin1("0")==type)
        return QString::fromLatin1("DVD-NTSC");
    else if (QString::fromLatin1("1")==type)
        return QString::fromLatin1("DVD-PAL");
    return type;
}

static QString fixDirectory(const QString &directory)
{
  QString dir = KMF::Tools::addSlash(directory);
  if(dir.startsWith("file://"))
      dir = dir.mid(7);
  return dir;
}

KMFProject::KMFProject(QObject *parent) :
  QObject(parent), m_aspectRatio(toAspectRatio(KMediaFactorySettings::defaultAspectRatio())), m_template(0), m_output(0), m_dirty(false),
  m_loading(false), m_initializing(false), m_serial(0)
{
  m_lastModified[ModMedia].setTime_t(0);
  m_lastModified[ModTemplate].setTime_t(0);
  m_lastModified[ModOutput].setTime_t(0);

  m_type = fixType(KMediaFactorySettings::defaultProjectType());
  m_title = i18n("Untitled");

  QDir dir(KMediaFactorySettings::defaultProjectDirectory().path());
  int i = 1;

  m_list.setDragable();

  while(1)
  {
    QString file = i18n("Project%1", i);

    QDir projectDir(dir.filePath(file));
    if(!projectDir.exists())
    {
      m_directory = fixDirectory(projectDir.path());
      break;
    }
    ++i;
  }
  //kDebug() << m_directory;
}

KMFProject::~KMFProject()
{
}

bool KMFProject::validProject() const
{
  setError();
  if(m_list.count() == 0)
  {
    setError(i18n("No media files."));
    return false;
  }
  if(!m_template)
  {
    setError(i18n("No template selected."));
    return false;
  }
  if(!m_output)
  {
    setError(i18n("No output type selected."));
    return false;
  }
  return true;
}

bool KMFProject::addItem(KMF::MediaObject *mob)
{
  if(m_list.count()<99)
  {
    m_list.append(mob);
    setDirty(KMF::Media);
    return true;
  }
  return false;
}

void KMFProject::removeItem(KMF::MediaObject *mob)
{
  m_list.removeAll(mob);
  setDirty(KMF::Media);
}

void KMFProject::setType(const QString& type)
{
  if(type != m_type)
  {
    m_type = fixType(type);
    emit preinit(m_type);
    m_list.clear();
    emit init(type);
  }
}

void KMFProject::setAspectRatio(QDVD::VideoTrack::AspectRatio ar)
{
  if(ar!=m_aspectRatio)
  {
    m_aspectRatio=ar;
    setDirty(KMF::Any);
  }
}

void KMFProject::setDirectory(const QString& directory)
{
  QString dir = fixDirectory(directory);
  if(dir!=m_directory)
  {
    m_directory=dir;
    setDirty(KMF::Any);
  }
}

void KMFProject::setTitle(const QString& title)
{
  if(title!=m_title)
  {
    m_title = title;
    setDirty(KMF::Template);
  }
}

void KMFProject::setTemplateObj(KMF::TemplateObject* tmplate)
{
  if(tmplate!=m_template)
  {
    m_template = tmplate;
    setDirty(KMF::Template);
  }
}

void KMFProject::setOutput(KMF::OutputObject* output)
{
  if(output!=m_output)
  {
    m_output = output;
    setDirty(KMF::Output);
  }
}

void KMFProject::init()
{
  m_initializing = true;
  kDebug() << "preinit";
  emit preinit(m_type);
  kDebug() << "init" << m_type;
  emit init(m_type);
  m_initializing = false;
  setDirty(KMF::Any, false);
}

void KMFProject::mediaObjFromXML(const QDomElement& e)
{
  KMF::Plugin *plugin =
      kmfApp->pluginInterface()->findChild<KMF::Plugin*>(e.attribute("plugin"));

  if(plugin)
  {
    KMF::MediaObject *mob = plugin->createMediaObject(e);
    if(mob)
      addItem(mob);
  }
}

void KMFProject::templateFromXML(const QDomElement& e)
{
  KMF::Plugin *plugin =
      kmfApp->pluginInterface()->findChild<KMF::Plugin*>(e.attribute("plugin"));
  if(plugin)
  {
    m_template =
        plugin->findChild<KMF::TemplateObject*>(e.attribute("object"));
    if(m_template)
      m_template->fromXML(e);
  }
}

void KMFProject::outputFromXML(const QDomElement& e)
{
  KMF::Plugin *plugin =
      kmfApp->pluginInterface()->findChild<KMF::Plugin*>(e.attribute("plugin"));
  if(plugin)
  {
    m_output =
        plugin->findChild<KMF::OutputObject*>(e.attribute("object"));
    if(m_output)
      m_output->fromXML(e);
  }
}

void KMFProject::fromXML(QString xml)
{
  emit preinit(m_type);
  QDomDocument doc("kmf_project");
  doc.setContent(xml);
  QDomElement element = doc.documentElement();
  m_type = fixType(element.attribute("type"));
  m_aspectRatio = toAspectRatio(element.attribute("aspect"));
  m_directory = fixDirectory(element.attribute("dir"));
  m_title = element.attribute("title");
  m_serial = element.attribute("serial").toInt();

  if(element.hasAttribute("last_mod_media"))
    m_lastModified[ModMedia] =
    QDateTime::fromString(element.attribute("last_mod_media"), Qt::ISODate);
  if(element.hasAttribute("last_mod_template"))
    m_lastModified[ModTemplate] =
    QDateTime::fromString(element.attribute("last_mod_template"), Qt::ISODate);
  if(element.hasAttribute("last_mod_output"))
    m_lastModified[ModOutput] =
    QDateTime::fromString(element.attribute("last_mod_output"), Qt::ISODate);
  if(element.hasAttribute("last_subtype"))
    m_subType = element.attribute("last_subtype");

  QDomNode n = element.firstChild();
  while(!n.isNull())
  {
    QDomElement e = n.toElement();
    if(!e.isNull())
    {
      if(e.tagName() == "media")
      {
        mediaObjFromXML(e);
      }
      if(e.tagName() == "template")
      {
        templateFromXML(e);
      }
      if(e.tagName() == "output")
      {
        outputFromXML(e);
      }
    }
    n = n.nextSibling();
  }
  emit init(m_type);
}

void KMFProject::saveObj(QDomDocument& doc, QDomElement& root,
                         const char* name, KMF::Object* ob)
{
  if(ob)
  {
    QDomElement e = doc.createElement(name);
    if(ob->parent())
      e.setAttribute("plugin", ob->parent()->objectName());
    e.setAttribute("object", ob->objectName());
    ob->toXML(&e);
    root.appendChild(e);
  }
}

QString KMFProject::toXML()
{
  KMF::MediaObject *obj;
  QDomDocument doc("kmf_project");
  doc.appendChild(doc.createProcessingInstruction("xml",
                  "version=\"1.0\" encoding=\"UTF-8\""));

  QDomElement root=doc.createElement("project");
  root.setAttribute("type", m_type);
  root.setAttribute("aspect", QDVD::VideoTrack::aspectRatioString(m_aspectRatio));
  root.setAttribute("dir", m_directory);
  root.setAttribute("title", m_title);
  root.setAttribute("serial", m_serial);
  root.setAttribute("last_mod_media",
                    m_lastModified[ModMedia].toString(Qt::ISODate));
  root.setAttribute("last_mod_template",
                    m_lastModified[ModTemplate].toString(Qt::ISODate));
  root.setAttribute("last_mod_output",
                    m_lastModified[ModOutput].toString(Qt::ISODate));
  root.setAttribute("last_subtype", m_subType);

  doc.appendChild(root);

  foreach(obj, m_list.list())
    saveObj(doc, root, "media", obj);
  saveObj(doc, root, "template", m_template);
  saveObj(doc, root, "output", m_output);
  return doc.toString();
}

QString KMFProject::directory(const QString& subDir, bool create) const
{
  QString result = m_directory;
  if(!subDir.isEmpty())
    result = KMF::Tools::joinPaths(result, subDir);

  QDir dir(result);
  if(create && !QDir(result).exists())
    dir.mkpath(result);
  return KMF::Tools::addSlash(result);
}

bool KMFProject::prepare(const QString& type)
{
  if(!validProject())
    return false;

  foreach(KMF::MediaObject *obj, m_list.list())
  {
    if(!obj->prepare(type))
    {
      return false;
      break;
    }
    CHECK_IF_STOPPED(false);
  }
  if (!m_template->prepare(type))
    return false;

  CHECK_IF_STOPPED(false);

  if (!m_output->prepare(type))
    return false;

  CHECK_IF_STOPPED(false);

  m_subType = type;
  return true;
}

void KMFProject::finished()
{
  foreach(KMF::MediaObject* obj, m_list.list())
    obj->finished();
  m_template->finished();
  m_output->finished();
}

bool KMFProject::open(const KUrl &url)
{
  bool result = false;
  QString tmp;

  m_loading = true;
  if (KMF::Tools::loadStringFromFile(url, &tmp))
  {
    fromXML(tmp);
    m_url = url;
    setDirty(KMF::Any, false);
    result = true;
  }
  m_loading = false;
  return result;
}

bool KMFProject::save(KUrl url)
{
  bool saveas = !url.isEmpty();

  if(url.isEmpty())
  {
    if(m_url.isEmpty())
      return false;
    else
      url = m_url;
  }

  if(KIO::NetAccess::exists(url, KIO::NetAccess::DestinationSide,
                            kmfApp->widget()) && saveas)
  {
    if(KMessageBox::questionYesNo(kmfApp->activeWindow(),
        i18n("Do you want to overwrite %1", url.prettyUrl()))
      == KMessageBox::No)
    {
      return true;
    }
  }

  KMF::Tools::saveString2File(url, toXML());

  m_url = url;
  setDirty(KMF::Any, false);
  return true;
}

void KMFProject::setModified(KMF::DirtyType type)
{
  if(type & KMF::Media)
    m_lastModified[ModMedia] = QDateTime::currentDateTime();
  if(type & KMF::Template)
    m_lastModified[ModTemplate] = QDateTime::currentDateTime();
  if(type & KMF::Output)
    m_lastModified[ModOutput] = QDateTime::currentDateTime();
}

void KMFProject::setDirty(KMF::DirtyType type, bool dirty)
{
  m_dirty = dirty;
  if(dirty && !m_loading && !m_initializing)
  {
    setModified(type);
  }
  if(!m_initializing)
    emit modified(m_title, dirty);
  if(dirty && !m_loading && !m_initializing)
  {
    if(type & KMF::Media)
    {
      kDebug() << "media modified";
      emit mediaModified();
    }
    if(type & KMF::Template)
      emit templatesModified();
    if(type & KMF::Output)
      emit outputsModified();
  }
}

QDateTime KMFProject::lastModified(KMF::DirtyType type) const
{
  QDateTime d;

  if((type & KMF::Media) && d < m_lastModified[ModTemplate])
    d = m_lastModified[ModMedia];
  if((type & KMF::Template) && d < m_lastModified[ModTemplate])
    d = m_lastModified[ModTemplate];
  if((type & KMF::Output) && d < m_lastModified[ModTemplate])
    d = m_lastModified[ModOutput];
  return d;
}

QMap<QString, QString> KMFProject::subTypes() const
{
  QMap<QString, QString> result;
  KMF::MediaObject* obj;

  foreach(obj, m_list.list())
  {
    if(obj)
      addMap(&result, obj->subTypes());
  }
  if(m_template)
    addMap(&result, m_template->subTypes());
  if(m_output)
    addMap(&result, m_output->subTypes());
  return result;
}

#include "kmfproject.moc"



