//**************************************************************************
//   Copyright (C) 2004 by Petri Damst�
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
#include "projectoptions.h"
#include "kmfapplication.h"
#include "kmftools.h"
#include <kcombobox.h>
#include <kpushbutton.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>
#include <kurlrequester.h>
#include <kmessagebox.h>
#include <qcombobox.h>
#include <qlineedit.h>

ProjectOptions::ProjectOptions(QWidget* parent, const char* name,
                               bool modal, WFlags fl) :
  ProjectOptionsLayout(parent, name, modal, fl), titleChangesPath(false)
{
  QStringList list = kmfApp->supportedProjects();

  for(QStringList::Iterator it = list.begin(); it != list.end(); ++it)
  {
    typeComboBox->insertItem(*it);
  }
}

ProjectOptions::~ProjectOptions()
{
}

void ProjectOptions::getData(KMFProject& project) const
{
  project.setDirectory(directoryURL->url());
  project.setTitle(titleEdit->text());
  project.setType(typeComboBox->currentText());
}

void ProjectOptions::setData(const KMFProject& project)
{
  directoryURL->setURL(project.directory());
  titleEdit->setText(project.title());
  typeComboBox->setCurrentItem(0);
  for (int i=0; i < typeComboBox->count(); ++i)
  {
    if (typeComboBox->text(i) == project.type())
    {
      typeComboBox->setCurrentItem(i);
      break;
    }
  }
  QDir dir(directoryURL->url());
  titleChangesPath = dir.dirName().startsWith(i18n("Project"));
  m_saved = dir.dirName();
  m_type = project.type();
  m_count = project.mediaCount();
}

void ProjectOptions::titleChanged(const QString& title)
{
  if(titleChangesPath)
  {
    QDir dir(directoryURL->url());
    if(title != i18n("Untitled"))
    {
      int n = dir.path().findRev(dir.dirName());
      QString mainDir = dir.path().left(n);
      QString projectDir;
      
      if(!title.isEmpty())
        projectDir = KMF::Tools::simpleName(title);
      else
        projectDir = m_saved;

      directoryURL->setURL(mainDir + projectDir);
    }
  }
}

void ProjectOptions::okClicked()
{
  if(m_count == 0 || m_type == typeComboBox->currentText())
    accept();
  else
  {
    switch(KMessageBox::warningYesNo(this,
           i18n("Media files will be removed from the project"
                " when project type changes. Continue?")))
    {
      case KMessageBox::Yes :
        accept();
        break;
      case KMessageBox::No :
      default:
        return;
        break;
    }
  }
}

#include "projectoptions.moc"

