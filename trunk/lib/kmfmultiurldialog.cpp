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
}

KMFMultiURLDialog::~KMFMultiURLDialog()
{
}

void KMFMultiURLDialog::moveDown()
{
#warning TODO
/*
  Q3ListViewItem* item = fileListView->currentItem();
  if(item->itemBelow())
    item->moveItem(item->itemBelow());
  fileListView->ensureItemVisible(item);
*/
}

void KMFMultiURLDialog::moveUp()
{
#warning TODO
/*
  Q3ListViewItem* item = fileListView->currentItem();
  if(item->itemAbove())
    item->itemAbove()->moveItem(item);
  fileListView->ensureItemVisible(item);
*/
}

void KMFMultiURLDialog::remove()
{
#warning TODO
/*
  Q3ListViewItemIterator it(fileListView);
  Q3ListViewItem* first = 0;

  while(*it != 0)
  {
    if((*it)->isSelected())
    {
      if(!first)
        first = (*it)->itemAbove();
      delete *it;
    }
    else
      ++it;
  }
  if(!first)
    first = fileListView->firstChild();
*/
  fileListView->setCurrentIndex(m_model.index(0));
}

void KMFMultiURLDialog::add()
{
  QStringList files = KFileDialog::getOpenFileNames(m_dir,
      m_filter + "\n*.*|All files", this);

  if(files.count() > 0)
  {
    QStringList l = m_model.stringList();
     l << files;
     m_model.setStringList(l);
  }
}

void KMFMultiURLDialog::addFiles(const QStringList& files)
{
  QStringList l = m_model.stringList();

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
    l << files;
  }
  m_model.setStringList(l);
  fileListView->setCurrentIndex(m_model.index(0));
}

QStringList KMFMultiURLDialog::files()
{
  return m_model.stringList();
}

#include "kmfmultiurldialog.moc"
