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

#include "dvddirectoryobject.h"
#include "dvdauthorparser.h"
#include "outputplugin.h"
#include <kmediafactory/kmfobject.h>
#include <kmediafactory/logger.h>
#include <KApplication>
#include <KMimeType>
#include <KIconLoader>
#include <KLocale>
#include <KDebug>
#include <KMessageBox>
#include <KIcon>
#include <KActionCollection>
#include <KAction>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QEventLoop>
#include <QRegExp>
#include <QStringList>
#include <QPixmap>
#include <limits.h>

DvdDirectoryObject::DvdDirectoryObject(QObject* parent)
  : DvdAuthorObject(parent), m_points(0), m_filePoints(0)
{
  setObjectName("dvddir");
  setTitle(i18n("DVD Directory"));
  dvdCleanDirectory = new KAction(KIcon("editdelete"),
                                  i18n("&Clean directory"),this);
  plugin()->actionCollection()->addAction("ddob_cleandir", dvdCleanDirectory);
  connect(dvdCleanDirectory, SIGNAL(triggered()), SLOT(clean()));
}

DvdDirectoryObject::~DvdDirectoryObject()
{
}

void DvdDirectoryObject::actions(QList<QAction*>* actionList) const
{
  actionList->append(dvdCleanDirectory);
}

bool DvdDirectoryObject::fromXML(const QDomElement&)
{
  return true;
}

int DvdDirectoryObject::timeEstimate() const
{
  return TotalPoints + DvdAuthorObject::timeEstimate();
}

void DvdDirectoryObject::output(const QString& line)
{
  //kDebug() << line;
  bool stopped = false;

  if(line.startsWith("\t") &&
      (m_lastLine == Warning || m_lastLine == Error))
  {
    stopped = interface()->message((KMF::PluginInterface::MsgType)m_lastLine, line.mid(1));
  }
  else if(line.startsWith("ERR:"))
  {
    m_lastLine = Error;
    m_error = true;
    stopped = interface()->message(KMF::PluginInterface::Error, line.mid(6));
  }
  else if(line.startsWith("WARN:"))
  {
    m_lastLine = Warning;
    QString temp = line.mid(6);
    // Don't print multiple similar warnings. They can be found from the log.
    if(temp != m_warning)
    {
      stopped = interface()->message(KMF::PluginInterface::Warning, temp);
      m_warning = temp;
    }
  }
  else if(line.startsWith("STAT: Processing"))
  {
    QString previousFile = m_currentFile.filePath();
    m_lastLine = Processing;
    m_lastSize += m_currentFile.size() / 1024;
    m_currentFile.setFile(line.mid(17, line.length() - 20));

    stopped = interface()->message(KMF::PluginInterface::Info,
        i18n("  Processing: %1", m_currentFile.fileName()));
    stopped = interface()->setItemTotalSteps(m_currentFile.size()/1024);
    if(!m_first)
    {
      if(previousFile != m_currentFile.filePath())
        progress(m_filePoints);
    }
    else
      m_first = false;
    m_vobu = m_lastVobu;
  }
  else if(line.startsWith("STAT: VOBU"))
  {
    QRegExp reVobu("VOBU (\\d+) at (\\d+)MB, .*");

    if(m_lastLine != Vobu && m_lastLine != Processing)
    {
      stopped = interface()->message(KMF::PluginInterface::Info,
          i18n("  Processing: %1",m_currentFile.fileName()));
      stopped = interface()->setItemTotalSteps(m_currentFile.size()/1024);
    }
    m_lastLine = Vobu;
    if(reVobu.indexIn(line) > -1)
    {
      m_lastVobu = reVobu.cap(1).toInt();
      if(m_vobu != 0)
      {
        if(m_lastVobu < m_vobu)
          m_lastSize = 0;
        m_vobu = 0;
      }
      stopped = interface()->setItemProgress(
          reVobu.cap(2).toInt()*1024 - m_lastSize);
    }
  }
  else if(line.startsWith("STAT: fixing VOBU"))
  {
    QRegExp reFix(".* (\\d+)%\\)");

    if(m_lastLine != FixingVobu)
    {
      stopped = interface()->message(KMF::PluginInterface::Info,
          i18n("  Fixing: %1", m_currentFile.fileName()));
      stopped = interface()->setItemTotalSteps(100);
    }
    m_lastLine = FixingVobu;
    if(reFix.indexIn(line) > -1)
      stopped = interface()->setItemProgress(reFix.cap(1).toInt());
  }
  else
  {
    if(m_lastLine == Warning || m_lastLine == Error)
      m_lastLine = None;
  }
  if(m_lastLine != Warning)
    m_warning = "";
  if(stopped)
    m_run.kill();
}

void DvdDirectoryObject::clean()
{
  DvdAuthorObject::clean();

  QStringList list;

  interface()->cleanFiles("DVD/AUDIO_TS", list);
  list.append("*.VOB");
  list.append("*.BUP");
  list.append("*.IFO");
  interface()->cleanFiles("DVD/VIDEO_TS", list);
}

bool DvdDirectoryObject::isUpToDate(QString type)
{
  if(type != interface()->lastSubType())
    return false;

  QDateTime lastModified = interface()->lastModified(
      KMF::PluginInterface::DirtyAny);
  QDir dir(interface()->projectDir("DVD/VIDEO_TS"));
  if(dir.exists() == false)
    return false;
  dir.nameFilters().append("*.VOB");
  dir.nameFilters().append("*.BUP");
  dir.nameFilters().append("*.IFO");
  QStringList files = dir.entryList();

  files.removeAll(".");
  files.removeAll("..");
  if(files.count() <= 0)
    return false;

  for(QStringList::Iterator it=files.begin(); it!=files.end(); ++it )
  {
    QFileInfo fi(dir.filePath(*it));

    if(fi.exists() == false || lastModified > fi.lastModified())
      return false;
  }
  return true;
}

void DvdDirectoryObject::progress(int points)
{
  if(points > m_points)
    points = m_points;
  m_points -= points;
  //kDebug() << m_points;
  interface()->progress(points);
}

bool DvdDirectoryObject::make(QString type)
{
  if(DvdAuthorObject::make(type) == false)
    return false;

  if(isUpToDate(type))
  {
    interface()->message(KMF::PluginInterface::OK, i18n("DVD Directory is up to date"));
    interface()->progress(TotalPoints);
    return true;
  }
  m_error = false;
  m_lastLine = None;
  m_first = true;
  m_warning = "";
  m_currentFile.setFile("");
  m_points = TotalPoints;
  m_lastVobu = 0;
  m_vobu = 0;
  m_lastSize = 0;

  KMF::DVDAuthorParser da;
  da.setFile(interface()->projectDir() +"dvdauthor.xml");
  int count = da.files().count();
  if(count > 0)
    m_filePoints = m_points / count;
  else
    m_filePoints = 0;

  clean();
  interface()->message(KMF::PluginInterface::Info, i18n("Making DVD Directory"));

  m_run.setCommand("dvdauthor -x dvdauthor.xml");
  m_run.setWorkingDirectory(interface()->projectDir());
  connect(&m_run, SIGNAL(line(const QString&)),
          this, SLOT(output(const QString&)));
  m_run.run();
  interface()->logger()->message(m_run.output());
  if(!m_error)
  {
    interface()->message(KMF::PluginInterface::OK, i18n("DVD Directory ready"));
    if(type == "dummy")
      static_cast<OutputPlugin*>(plugin())->play();
  }
  else
    clean();
  progress(TotalPoints);
  return !m_error;
}

void DvdDirectoryObject::toXML(QDomElement*) const
{
}

QPixmap DvdDirectoryObject::pixmap() const
{
  return KIconLoader::global()->loadIcon("folder-video", KIconLoader::NoGroup,
                                         KIconLoader::SizeLarge);
}

#include "dvddirectoryobject.moc"
