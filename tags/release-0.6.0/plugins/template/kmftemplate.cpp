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
#include "kmftemplate.h"
#include "kmftools.h"
#include <kstore/KoStore.h>
#include <kzip.h>
#include <kdebug.h>
#include <QFileInfo>
#include <QDir>

KMFTemplate::KMFTemplate(const QString file) : m_store(0), m_language("en")
{
  if(!file.isEmpty())
    setStore(file);
}

KMFTemplate::~KMFTemplate()
{
  if(m_domain.data)
    kmf_nl_unload_domain ((struct loaded_domain *)m_domain.data);
  delete m_store;
}

bool KMFTemplate::setStore(const QString& store)
{
  if(m_store)
    delete m_store;

  QFileInfo fi(store);
  QString s(store);

  if(fi.isDir())
    s = KMF::Tools::addSlash(store);

  m_store = KoStore::createStore(s, KoStore::Read);
  if(m_store->bad())
  {
    delete m_store;
    m_store = 0;
  }
  else
    m_storeName = store;
  return (m_store != 0);
}

bool KMFTemplate::hasFile(const QString& file) const
{
  if(m_store)
  {
    return m_store->hasFile(file);
  }
  return false;
}

QByteArray KMFTemplate::readFile(const QString& file) const
{
  QByteArray result;

  if(m_store)
  {
    if(m_store->open(file))
    {
      QIODevice* device = m_store->device();

      if(device->isOpen() || device->open(QIODevice::ReadOnly))
      {
        result = device->readAll();
        device->close();
      }
      m_store->close();
    }
  }
  return result;
}

void KMFTemplate::setLanguage(const QString& file, const QString& lang)
{
  if(m_language == lang && m_file == file)
    return;

  QString zfile = QString("locale/%1/LC_MESSAGES/%2.mo").arg(lang).arg(file);

  if(m_store)
  {
    if(m_store->open(zfile))
    {
      if(m_domain.data)
        kmf_nl_unload_domain((struct loaded_domain *)m_domain.data);
      kmf_nl_load_domain(m_store->device(), m_store->size(), &m_domain);
      m_store->close();
      m_language = lang;
      m_file = file;
    }
  }
  //kDebug() << m_language << ", " <<  file << ", "
  //    << zfile;
}

QStringList KMFTemplate::languages() const
{
  QStringList result("en");
  QFileInfo fi(m_storeName);

  if(fi.isDir())
  {
    QDir dir(KMF::Tools::joinPaths(m_storeName, "locale/"));
    result += dir.entryList();
  }
  else
  {
    KZip zip(m_storeName);

    if(zip.open(QIODevice::ReadOnly))
    {
      const KArchiveEntry* entry = zip.directory()->entry("locale");
      if(entry && entry->isDirectory())
      {
        const KArchiveDirectory* dir =
            static_cast<const KArchiveDirectory*>(entry);
        result += dir->entries();
      }
      zip.close();
    }
  }
  return result;
}

QString KMFTemplate::translate(const char* text) const
{
  //kDebug() << text;
  QString result = QString::fromUtf8(kmf_nl_find_msg(&m_domain, text));
  //kDebug() << text << "=" << result;
  if(result.isEmpty())
    return text;
  else
    return result;
}

QIODevice* KMFTemplate::device(const QString& file)
{
  m_store->open(file);
  return m_store->device();
}

void KMFTemplate::close()
{
  m_store->close();
}
