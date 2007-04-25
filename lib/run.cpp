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

Run::~Run()
{
}
void Run::setCommand(QString command)
{
  bool in = false;
  QChar previous = ' ';
  QString arg;

  m_program = "";
  m_arguments.clear();
  for(int i=0; i <= command.length(); ++i)
  {
    bool end = (i == command.length());

    if(!end && command[i] == '\"')
    {
      if(i == 0 || command[i-1] != '\\' )
        in = (in)?false:true;
      else
        arg[arg.length() - 1] = command[i];
      continue;
    }
    if(end || (command[i] == ' ' && !in))
    {
      kDebug() << k_funcinfo << "<" << arg << ">" << endl;
      if(m_program.isEmpty())
        m_program = arg;
      else
        m_arguments.append(arg);
      arg = "";
      continue;
    }
    arg += command[i];
  }
}

bool Run::run()
{
  setProcessChannelMode(QProcess::MergedChannels);

  if(m_program[0] != '/')
  {
    QFileInfo file;
    file = KStandardDirs::locate("data", "kmediafactory/scripts/" + m_program);
    if(file.exists())
      m_program = file.filePath();
  }
  kDebug() << "Running: " << m_program << m_arguments << endl;
  connect(this, SIGNAL(readyRead()),
          this, SLOT(stdout()));
  connect(this, SIGNAL(finished(int, QProcess::ExitStatus)),
          this, SLOT(exit(int, QProcess::ExitStatus)));
  QStringList env = QProcess::systemEnvironment();
  env << QString("KMF_DBUS=org.kde.kmediafactory_%1/KMediaFactory")
      .arg(getpid());
  env << QString("KMF_WINID=%1")
      .arg(QApplication::topLevelWidgets()[0]->winId());
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
