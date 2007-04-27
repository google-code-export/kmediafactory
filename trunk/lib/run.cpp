//**************************************************************************
//   Copyright (C) 2006 by Petri Damsten
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
#include "run.h"
#include <kdebug.h>
#include <kshell.h>
#include <kapplication.h>
#include <kstandarddirs.h>
#include <QStringList>
#include <QFileInfo>
#include <QWidget>
#include <sys/types.h>
#include <unistd.h>

Run::Run(QString command, QString dir)
{
  setCommand(command);
  setWorkingDirectory(dir);
  if(!command.isEmpty())
    run();
}

Run::Run(QStringList command, QString dir)
{
  setCommand(command);
  setWorkingDirectory(dir);
  if(!command.isEmpty())
    run();
}

Run::~Run()
{
}

void Run::setCommand(QString command)
{
  m_arguments = KShell::splitArgs(command);
  if(m_arguments.count() > 0)
    m_program = m_arguments.takeFirst();
}

void Run::setCommand(QStringList command)
{
  m_arguments = command;
  if(m_arguments.count() > 0)
    m_program = m_arguments.takeFirst();
}

bool Run::run()
{
  setProcessChannelMode(QProcess::MergedChannels);

  kDebug() << "Running: " << m_program << m_arguments << endl;
  connect(this, SIGNAL(readyRead()),
          this, SLOT(stdout()));
  connect(this, SIGNAL(finished(int, QProcess::ExitStatus)),
          this, SLOT(exit(int, QProcess::ExitStatus)));

  QStringList env = QProcess::systemEnvironment();
  QStringList kmfPaths;
  kmfPaths << KGlobal::dirs()->findDirs("apps", "kmediafactory/scripts/");
  kmfPaths << KGlobal::dirs()->findDirs("apps", "kmediafactory/tools/");
  env << QString("KMF_DBUS=org.kde.kmediafactory_%1/KMediaFactory")
      .arg(getpid());
  env << QString("KMF_WINID=%1")
      .arg(QApplication::topLevelWidgets()[0]->winId());
  foreach(QString path, kmfPaths)
    env.replaceInStrings(QRegExp("^PATH=(.*)", Qt::CaseInsensitive),
                        QString("PATH=%1;\\1").arg(path));
  setEnvironment(env);

  m_outputIndex = 0;
  start(m_program, m_arguments);
  while(!waitForFinished(250))
  {
    if(state() == QProcess::NotRunning)
      break;
    kapp->processEvents();
  }
  kDebug() << "Output: " << m_output << endl;
  if(exitStatus() == QProcess::NormalExit || exitCode() == 0)
  {
    return true;
  }
  return false;
}

void Run::stdout()
{
  int found;
  QRegExp re("[\n\r]");
  QByteArray ba = readAllStandardOutput();

  m_output += QString::fromLocal8Bit(ba.data(), ba.count());
  while((found = m_output.indexOf(re, m_outputIndex)) != -1)
  {
    emit line(m_output.mid(m_outputIndex, found - m_outputIndex));
    m_outputIndex = found + 1;
  }
}

void Run::exit(int, QProcess::ExitStatus)
{
  if(m_outputIndex < m_output.length())
    emit line(m_output.mid(m_outputIndex));

}

#include "run.moc"
