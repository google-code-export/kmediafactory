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
#include <QColorGroup>

#include "kmfmultiurldialog.h"
#include <kfiledialog.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kdebug.h>
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
}

KMFMultiURLDialog::~KMFMultiURLDialog()
{
}

void KMFMultiURLDialog::moveDown()
{
  QModelIndex i1 = fileListView->selectionModel()->currentIndex();
  if(i1.row() < m_model.rowCount() - 1)
  {
    QModelIndex i2 = m_model.index(i1.row() + 1, 0, QModelIndex());
    m_model.swap(i1, i2);
    fileListView->scrollTo(i2);
  }
}

void KMFMultiURLDialog::moveUp()
{
  QModelIndex i1 = fileListView->selectionModel()->currentIndex();
  if(i1.row() > 0)
  {
    QModelIndex i2 = m_model.index(i1.row() - 1, 0, QModelIndex());
    m_model.swap(i1, i2);
    fileListView->scrollTo(i2);
  }
}

void KMFMultiURLDialog::remove()
{
  QModelIndexList list = fileListView->selectionModel()->selectedRows();

  m_model.removeAt(list);
  fileListView->setCurrentIndex(m_model.index(0));
}

void KMFMultiURLDialog::add()
{
  QStringList files = KFileDialog::getOpenFileNames(m_dir,
      m_filter + "\n*.*|All files", this);

  if(files.count() > 0)
  {
    m_model.append(files);
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
