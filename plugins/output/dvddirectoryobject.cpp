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
#include "dvddirectoryobject.h"
#include "dvdauthorparser.h"
#include "outputplugin.h"
#include <kmediafactory/kmfobject.h>
#include <kapplication.h>
#include <kmimetype.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kicon.h>
#include <kactioncollection.h>
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

void DvdDirectoryObject::actions(QList<QAction*>& actionList) const
{
  actionList.append(dvdCleanDirectory);
}

void DvdDirectoryObject::fromXML(const QDomElement&)
{
}

int DvdDirectoryObject::timeEstimate() const
{
  return TotalPoints + DvdAuthorObject::timeEstimate();
}

void DvdDirectoryObject::output(K3Process* process, char* buffer, int buflen)
{
  bool stopped = false;
  int find = 0, start = 0;
  QRegExp re("[\n\r]");
  m_buffer += QString::fromLocal8Bit(buffer, buflen);

  while((find = m_buffer.indexOf(re, find)) > -1)
  {
    QString line = m_buffer.mid(start, find - start);
    if(line.startsWith("\t") &&
       (m_lastLine == Warning || m_lastLine == Error))
    {
      stopped = uiInterface()->message((KMF::MsgType)m_lastLine, line.mid(1));
    }
    else if(line.startsWith("ERR:"))
    {
      m_lastLine = Error;
      m_error = true;
      stopped = uiInterface()->message(KMF::Error, line.mid(6));
    }
    else if(line.startsWith("WARN:"))
    {
      m_lastLine = Warning;
      QString temp = line.mid(6);
      // Don't print multiple similar warnings. They can be found from the log.
      if(temp != m_warning)
      {
        stopped = uiInterface()->message(KMF::Warning, temp);
        m_warning = temp;
      }
    }
    else if(line.startsWith("STAT: Processing"))
    {
      QString previousFile = m_currentFile.filePath();
      m_lastLine = Processing;
      m_lastSize += m_currentFile.size() / 1024;
      m_currentFile.setFile(line.mid(17, line.length() - 20));

      stopped = uiInterface()->message(KMF::Info,
          QString(i18n("  Processing: %1")).arg(m_currentFile.fileName()));
      stopped = uiInterface()->setItemTotalSteps(m_currentFile.size()/1024);
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
        stopped = uiInterface()->message(KMF::Info,
            QString(i18n("  Processing: %1")).arg(m_currentFile.fileName()));
        stopped = uiInterface()->setItemTotalSteps(m_currentFile.size()/1024);
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
        stopped = uiInterface()->setItemProgress(
            reVobu.cap(2).toInt()*1024 - m_lastSize);
      }
    }
    else if(line.startsWith("STAT: fixing VOBU"))
    {
      QRegExp reFix(".* (\\d+)%\\)");

      if(m_lastLine != FixingVobu)
      {
        stopped = uiInterface()->message(KMF::Info,
            QString(i18n("  Fixing: %1")).arg(m_currentFile.fileName()));
        stopped = uiInterface()->setItemTotalSteps(100);
      }
      m_lastLine = FixingVobu;
      if(reFix.indexIn(line) > -1)
        stopped = uiInterface()->setItemProgress(reFix.cap(1).toInt());
    }
    else
    {
      if(m_lastLine == Warning || m_lastLine == Error)
        m_lastLine = None;
    }
    if(m_lastLine != Warning)
      m_warning = "";
    ++find;
    start = find;
    if(stopped)
      process->kill();
  }
  m_buffer.remove(0, start);
}

void DvdDirectoryObject::clean()
{
  DvdAuthorObject::clean();

  QStringList list;

  plugin()->projectInterface()->cleanFiles("DVD/AUDIO_TS", list);
  list.append("*.VOB");
  list.append("*.BUP");
  list.append("*.IFO");
  plugin()->projectInterface()->cleanFiles("DVD/VIDEO_TS", list);
}

bool DvdDirectoryObject::isUpToDate(QString type)
{
  if(type != projectInterface()->lastSubType())
    return false;

  QDateTime lastModified = projectInterface()->lastModified(
      KMF::ProjectInterface::DirtyAny);
  QDir dir(projectInterface()->projectDir("DVD/VIDEO_TS"));
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
  //kdDebug() << k_funcinfo << m_points << endl;
  uiInterface()->progress(points);
}

bool DvdDirectoryObject::make(QString type)
{
  if(DvdAuthorObject::make(type) == false)
    return false;

  if(isUpToDate(type))
  {
    uiInterface()->message(KMF::OK, i18n("DVD Directory is up to date"));
    uiInterface()->progress(TotalPoints);
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
  da.setFile(projectInterface()->projectDir() +"/dvdauthor.xml");
  int count = da.files().count();
  if(count > 0)
    m_filePoints = m_points / count;
  else
    m_filePoints = 0;

  clean();
  uiInterface()->message(KMF::Info, i18n("Making DVD Directory"));
  dvdauthor << "dvdauthor" << "-x" << "dvdauthor.xml";
  dvdauthor.setWorkingDirectory(projectInterface()->projectDir());
  uiInterface()->logger()->connectProcess(&dvdauthor);
  connect(&dvdauthor, SIGNAL(receivedStdout(K3Process*, char*, int)),
          this, SLOT(output(K3Process*, char*, int)));
  connect(&dvdauthor, SIGNAL(receivedStderr(K3Process*, char*, int)),
          this, SLOT(output(K3Process*, char*, int)));
  dvdauthor.start(K3Process::Block, K3Process::AllOutput);
  if(!m_error)
  {
    uiInterface()->message(KMF::OK, i18n("DVD Directory ready"));
    if(type == "dummy")
      static_cast<OutputPlugin*>(plugin())->play();
  }
  else
    clean();
  progress(TotalPoints);
  return !m_error;
}

void DvdDirectoryObject::toXML(QDomElement&) const
{
}

QPixmap DvdDirectoryObject::pixmap() const
{
  return KIconLoader::global()->loadIcon("folder_video", K3Icon::NoGroup,
                                           K3Icon::SizeLarge);
}

#include "dvddirectoryobject.moc"
