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
#include <QColorGroup>

#include "kmfmultiurldialog.h"
#include <KFileDialog>
#include <KApplication>
#include <KMessageBox>
#include <KLocale>
#include <KDebug>
#include <QStringList>

KMFMultiURLDialog::KMFMultiURLDialog(const QString& startDir,
                                     const QString& filter,
                                     QWidget* parent,
                                     const QString& title)
  : KDialog(parent), m_dir(startDir), m_filter(filter)
{
  setupUi(mainWidget());
  setButtons(KDialog::Ok | KDialog::Cancel);
  setCaption(title);
  fileListView->setModel(&m_model);

  addButton->setIcon(KIcon("list-add"));
  removeButton->setIcon(KIcon("list-remove"));
  upButton->setIcon(KIcon("arrow-up"));
  downButton->setIcon(KIcon("arrow-down"));

  connect(downButton, SIGNAL(clicked()), this, SLOT(moveDown()));
  connect(upButton, SIGNAL(clicked()), this, SLOT(moveUp()));
  connect(addButton, SIGNAL(clicked()), this, SLOT(add()));
  connect(removeButton, SIGNAL(clicked()), this, SLOT(remove()));
}

KMFMultiURLDialog::~KMFMultiURLDialog()
{
}

void KMFMultiURLDialog::moveDown()
{
  QModelIndex item1 = fileListView->currentIndex();
  QModelIndex item2 = m_model.index(item1.row() + 1);
  m_model.swap(item1, item2);
  fileListView->setCurrentIndex(item2);
  fileListView->scrollTo(item2);
}

void KMFMultiURLDialog::moveUp()
{
  QModelIndex item1 = fileListView->currentIndex();
  QModelIndex item2 = m_model.index(item1.row() - 1);
  m_model.swap(item1, item2);
  fileListView->setCurrentIndex(item2);
  fileListView->scrollTo(item2);
}

void KMFMultiURLDialog::remove()
{
  QModelIndexList selected = fileListView->selectionModel()->selectedIndexes();
  m_model.removeAt(selected);
  fileListView->setCurrentIndex(m_model.index(0));
}

void KMFMultiURLDialog::add()
{
  QStringList files = KFileDialog::getOpenFileNames(m_dir,
      m_filter + "\n*.*|All files", this);

  if(files.count() > 0)
  {
    addFiles(files);
  }
}

void KMFMultiURLDialog::addFiles(const QStringList& files)
{
  for(QStringList::ConstIterator it = files.begin();
      it != files.end(); ++it)
  {
    QFileInfo fi(*it);

    if(fi.isDir())
    {
      KMessageBox::error(kapp->activeWindow(),
                         i18n("Cannot add directory."));
      continue;
    }
    m_model.append(*it);
  }
  fileListView->setCurrentIndex(m_model.index(0));
}

QStringList KMFMultiURLDialog::files()
{
  return m_model.list();
}

#include "kmfmultiurldialog.moc"
