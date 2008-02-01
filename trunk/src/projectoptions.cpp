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
#include <QComboBox>
#include <QLineEdit>

ProjectOptions::ProjectOptions(QWidget* parent) :
  KDialog(parent), titleChangesPath(false)
{
  setupUi(mainWidget());
  setButtons(KDialog::Ok | KDialog::Cancel);

  connect(titleEdit, SIGNAL(textChanged(const QString&)),
          this, SLOT(textChanged(const QString&)));

  QStringList list = kmfApp->supportedProjects();
  directoryUrl->setMode(KFile::Directory);

  for(QStringList::Iterator it = list.begin(); it != list.end(); ++it)
  {
    typeComboBox->addItem(*it);
  }
}

ProjectOptions::~ProjectOptions()
{
}

void ProjectOptions::getData(KMFProject& project) const
{
  project.setDirectory(directoryUrl->url().prettyUrl());
  project.setTitle(titleEdit->text());
  project.setType(typeComboBox->currentText());
}

void ProjectOptions::setData(const KMFProject& project)
{
  directoryUrl->setUrl(project.directory("", false));
  titleEdit->setText(project.title());
  typeComboBox->setCurrentIndex(0);
  for (int i=0; i < typeComboBox->count(); ++i)
  {
    if (typeComboBox->itemText(i) == project.type())
    {
      typeComboBox->setCurrentIndex(i);
      break;
    }
  }
  QDir dir(directoryUrl->url().prettyUrl());
  titleChangesPath = dir.dirName().startsWith(i18n("Project"));
  m_saved = dir.dirName();
  m_type = project.type();
  m_count = project.mediaObjects().count();
}

void ProjectOptions::textChanged(const QString& title)
{
  kDebug();
  if(titleChangesPath)
  {
    QDir dir(directoryUrl->url().prettyUrl());
    if(title != i18n("Untitled"))
    {
      int n = dir.path().lastIndexOf(dir.dirName());
      QString mainDir = dir.path().left(n);
      QString projectDir;

      if(!title.isEmpty())
        projectDir = KMF::Tools::simpleName(title);
      else
        projectDir = m_saved;

      directoryUrl->setUrl(mainDir + projectDir);
    }
  }
}

void ProjectOptions::slotOk()
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

void ProjectOptions::slotButtonClicked(int button)
{
  if(button == KDialog::Ok)
    slotOk();
  else
    KDialog::slotButtonClicked(button);
}

#include "projectoptions.moc"

