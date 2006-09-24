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
#include "kmfproject.h"
#include "kmfapplication.h"
#include "kmediafactorysettings.h"
#include <kmftools.h>
#include <ktempfile.h>
#include <kmessagebox.h>
#include <kio/netaccess.h>
#include <kio/job.h>
#include <klocale.h>
#include <ksavefile.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <Q3ValueList>
#include <Q3PtrList>

template <class T>
Q3PtrList<T> deepCopy(Q3PtrList<T> list)
{
  T* item;
  Q3PtrList<T> newList;

  for(item = list.first(); item; item = list.next())
    newList.append(new T(*item));
  return newList;
}

template <class S, class T>
void addMap(QMap<S, T>* map, const QMap<S, T>& add)
{
  QList<S> keys = add.keys();

  for(int i = 0; i < keys.size(); ++i)
    map->insert(keys[i], add[keys[i]]);
}

KMFProject::KMFProject(QObject *parent) :
  QObject(parent), m_template(0), m_output(0), m_modified(false),
  m_loading(false), m_initializing(false), m_serial(0)
{
  m_lastModified[ModMedia].setTime_t(0);
  m_lastModified[ModTemplate].setTime_t(0);
  m_lastModified[ModOutput].setTime_t(0);

  m_type = KMediaFactorySettings::defaultProjectType();
  m_title = i18n("Untitled");

  QDir dir(KMediaFactorySettings::defaultProjectDirectory());
  int i = 1;

  while(1)
  {
    QString file = QString(i18n("Project%1")).arg(i);

    QDir projectDir(dir.filePath(file));
    if(!projectDir.exists())
    {
      m_directory = projectDir.path();
      break;
    }
    ++i;
  }
  //kdDebug() << m_directory << endl;
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

void KMFProject::addItem(KMF::MediaObject *mob)
{
  m_list.append(mob);
  emit newItem(mob);
  setDirty(KMF::ProjectInterface::DirtyMedia);
}

void KMFProject::removeItem(KMF::MediaObject *mob)
{
  m_list.removeAll(mob);
  emit itemRemoved(mob);
  setDirty(KMF::ProjectInterface::DirtyMedia);
}

void KMFProject::setType(const QString& type)
{
  if(type != m_type)
  {
    m_type = type;
    emit init(type);
    m_list.clear();
  }
}

void KMFProject::setDirectory(const QString& directory)
{
  m_directory = KMF::Tools::addSlash(directory);
  setDirty(KMF::ProjectInterface::DirtyAny);
}

void KMFProject::setTitle(const QString& title)
{
  m_title = title;
  setDirty(KMF::ProjectInterface::DirtyTemplate);
}

void KMFProject::setTemplateObj(KMF::TemplateObject* tmplate)
{
  m_template = tmplate;
  setDirty(KMF::ProjectInterface::DirtyTemplate);
}

void KMFProject::setOutput(KMF::OutputObject* output)
{
  m_output = output;
  setDirty(KMF::ProjectInterface::DirtyOutput);
}

void KMFProject::init()
{
  m_initializing = true;
  emit init(m_type);
  m_initializing = false;
  setDirty(KMF::ProjectInterface::DirtyAny, false);
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
  QDomDocument doc("kmf_project");
  doc.setContent(xml);
  QDomElement element = doc.documentElement();
  m_type = element.attribute("type");
  m_directory = element.attribute("dir");
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

  emit init(m_type);

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
    ob->toXML(e);
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

  foreach(obj, m_list)
    saveObj(doc, root, "media", obj);
  saveObj(doc, root, "template", m_template);
  saveObj(doc, root, "output", m_output);
  return doc.toString();
}

// From QtEz by Jan Prokop - GPL-2
bool KMFProject::mkdir(const QString &path)
{
  QDir dir;
  int pos = 1;

  while((pos = path.indexOf(QDir::separator(), pos)) != -1)
    dir.mkdir(path.left(pos++));

  return(dir.mkdir(path));
}

QString KMFProject::directory(const QString& subDir) const
{
  QString result = m_directory;
  if(!subDir.isEmpty())
  {
    result = KMF::Tools::joinPaths(result, subDir);
    QDir dir(result);
    if(!QDir(result).exists())
      mkdir(result);
  }
  return KMF::Tools::addSlash(result);
}

void KMFProject::cleanFiles(const QString& subDir,
                            const QStringList& files) const
{
  KUrl::List list;
  QString sub = KMF::Tools::addSlash(subDir);
  QDir dir(KMF::Tools::joinPaths(m_directory, sub));

  for(QStringList::ConstIterator it=files.begin(); it!=files.end(); ++it)
  {
    dir.setNameFilters(QStringList(*it));
    QStringList files2 = dir.entryList(QDir::Files | QDir::NoSymLinks);
    for(QStringList::Iterator jt=files2.begin(); jt!=files2.end(); ++jt)
    {
      QFile file(dir.filePath(*jt));
      file.remove();
    }
  }
  // Remove dirs if they are empty
  int pos = -1;

  while((pos = sub.lastIndexOf(QDir::separator(), pos)) != -1)
  {
    QString s = KMF::Tools::joinPaths(m_directory, sub.left(pos--));
    if(dir.rmdir(s) == false)
      break;
  }
}

bool KMFProject::make(QString type)
{
  KMF::MediaObject *obj;
  bool result = true;

  if(!validProject())
    return false;

  QDir dir(m_directory);
  if(!dir.exists())
    mkdir(dir.path());

  foreach(obj, m_list)
  {
    if(!obj->make(type))
    {
      result = false;
      break;
    }
  }
  if(result && !m_template->make(type))
    result = false;
  if(result && !m_output->make(type))
    result = false;
  m_subType = type;
  return result;
}

int KMFProject::timeEstimate() const
{
  int result = 0;

  if(!validProject())
    return result;
  foreach(KMF::MediaObject* obj, m_list)
    result += obj->timeEstimate();
  result += m_template->timeEstimate();
  result += m_output->timeEstimate();
  return result;
}

bool KMFProject::open(const KUrl &url)
{
  bool result = false;
  QString tmpFile;

  m_loading = true;
  if(KIO::NetAccess::download(url, tmpFile, kmfApp->widget()))
  {
    QFile file(tmpFile);
    if(file.open(QIODevice::ReadOnly))
    {
      QTextStream stream(&file);
      fromXML(stream.readAll());
      file.close();
      m_url = url;
      setDirty(KMF::ProjectInterface::DirtyAny, false);
      result = true;
    }
    KIO::NetAccess::removeTempFile(tmpFile);
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

  if(KIO::NetAccess::exists(url, false, kmfApp->widget()) && saveas)
  {
    if(KMessageBox::questionYesNo(kmfApp->activeWindow(),
        i18n("Do you want to overwrite %1").arg(url.prettyUrl()))
      == KMessageBox::No)
    {
      return true;
    }
  }

  if(url.isLocalFile())
  {
    KSaveFile saveFile(url.path());

    if(saveFile.status() == 0)
      *(saveFile.textStream()) << toXML();
    else
      return false;
  }
  else
  {
    KTempFile tempFile;
    tempFile.setAutoDelete(true);
    if(tempFile.status() == 0)
    {
      *(tempFile.textStream()) << toXML();
      if(tempFile.close())
      {
        if (!KIO::NetAccess::upload(tempFile.name(), url, kmfApp->widget()))
          return false;
      }
      else
        return false;
    }
    else
      return false;
  }
  m_url = url;
  setDirty(KMF::ProjectInterface::DirtyAny, false);
  return true;
}

void KMFProject::setDirty(KMF::ProjectInterface::DirtyType type, bool dirty)
{
  m_modified = dirty;
  if(dirty && !m_loading && !m_initializing)
  {
    if(type & KMF::ProjectInterface::DirtyMedia)
      m_lastModified[ModMedia] = QDateTime::currentDateTime();
    if(type & KMF::ProjectInterface::DirtyTemplate)
      m_lastModified[ModTemplate] = QDateTime::currentDateTime();
    if(type & KMF::ProjectInterface::DirtyOutput)
      m_lastModified[ModOutput] = QDateTime::currentDateTime();
  }
  if(!m_initializing)
    emit modified(m_title, dirty);
}

QDateTime KMFProject::lastModified(KMF::ProjectInterface::DirtyType type) const
{
  QDateTime d;

  if(type & KMF::ProjectInterface::DirtyMedia &&
     d < m_lastModified[ModTemplate])
    d = m_lastModified[ModMedia];
  if(type & KMF::ProjectInterface::DirtyTemplate &&
     d < m_lastModified[ModTemplate])
    d = m_lastModified[ModTemplate];
  if(type & KMF::ProjectInterface::DirtyOutput &&
     d < m_lastModified[ModTemplate])
    d = m_lastModified[ModOutput];
  return d;
}

QMap<QString, QString> KMFProject::subTypes() const
{
  QMap<QString, QString> result;

  foreach(KMF::MediaObject* obj, m_list)
    addMap(&result, obj->subTypes());
  if(m_template)
    addMap(&result, m_template->subTypes());
  if(m_output)
    addMap(&result, m_output->subTypes());
  return result;
}

#include "kmfproject.moc"
