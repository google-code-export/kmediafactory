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
#include "projectoptions.h"
#include "kmfapplication.h"
#include "kmftools.h"
#include <KComboBox>
#include <KPushButton>
#include <KIconLoader>
#include <KDebug>
#include <KLocale>
#include <KUrlRequester>
#include <KMessageBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPainter>
#include <qdvdinfo.h>

ProjectOptions::ProjectOptions(QWidget* parent) :
  QWidget(parent), titleChangesPath(false)
{
  setupUi(this);
}

ProjectOptions::~ProjectOptions()
{
}

void ProjectOptions::init()
{
  QStringList list = kmfApp->supportedProjects();
  directoryUrl->setMode(KFile::Directory);

  for(QStringList::Iterator it = list.begin(); it != list.end(); ++it)
  {
    typeComboBox->addItem(*it);
  }
  connect(titleEdit, SIGNAL(textChanged(const QString&)),
          this, SLOT(titleChanged(const QString&)));
  connect(directoryUrl, SIGNAL(textChanged(const QString&)),
          this, SLOT(directoryChanged(const QString&)));
  connect(typeComboBox, SIGNAL(currentIndexChanged(int)),
          this, SLOT(typeChanged(int)));
  connect(aspectComboBox, SIGNAL(currentIndexChanged(int)),
          this, SLOT(aspectChanged(int)));
}

void ProjectOptions::setData(const KMFProject& project)
{
  setProjectDirectory(project.directory("", false));
  setProjectTitle(project.title());
  setProjectType(project.type());
  aspectComboBox->setCurrentIndex(project.aspectRatio());
  QDir dir(directoryUrl->url().prettyUrl());
  titleChangesPath = dir.dirName().startsWith(i18n("Project"));
  m_saved = dir.dirName();
  m_type = project.type();
  m_count = project.mediaObjects().count();
}

void ProjectOptions::setProjectTitle(const QString& title)
{
  titleEdit->setText(title);
}

void ProjectOptions::setProjectType(const QString& type)
{
  for (int i=0; i < typeComboBox->count(); ++i)
  {
    if (typeComboBox->itemText(i) == type)
    {
      typeComboBox->setCurrentIndex(i);
      break;
    }
  }
}

void ProjectOptions::setProjectAspectRatio(const QString& aspect)
{
  aspectComboBox->setCurrentIndex(KMFProject::toAspectRatio(aspect));
}

void ProjectOptions::setProjectDirectory(const QString& dir)
{
  directoryUrl->setUrl(dir);
}

void ProjectOptions::titleChanged(const QString& title)
{
  kDebug();
  kmfApp->project()->setTitle(titleEdit->text());

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

void ProjectOptions::directoryChanged(const QString &)
{
    kmfApp->project()->setDirectory(directoryUrl->url().prettyUrl());
}

void ProjectOptions::typeChanged(int)
{
    kmfApp->project()->setType(typeComboBox->currentText());
}

void ProjectOptions::aspectChanged(int idx)
{
    kmfApp->project()->setAspectRatio((QDVD::VideoTrack::AspectRatio)idx);
}

void ProjectOptions::paintEvent(QPaintEvent *ev)
{
    static const int constBorder=12;

    QWidget::paintEvent(ev);
    QRect    r(rect());
    QPainter painter(this);
    QPixmap  pix(KIcon("kmediafactory").pixmap(256, 256, QIcon::Disabled));

    painter.drawPixmap(Qt::RightToLeft==layoutDirection()
                        ? r.left()+constBorder
                        : r.right()-(pix.width()+constBorder),
                       r.bottom()-(pix.height()+constBorder),
                       pix);
}

#include "projectoptions.moc"

