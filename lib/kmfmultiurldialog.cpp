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
#include <Q3ListView>
#include <QStringList>
#include <Q3Header>

KMFMultiURLDialog::KMFMultiURLDialog(const QString& startDir,
                                     const QString& filter,
                                     QWidget* parent,
                                     const QString& title)
  : KDialog(parent), m_dir(startDir), m_filter(filter)
{
  setCaption(title);
  fileListView->header()->hide();
  // User sorting
  fileListView->setSorting(1000);
}

KMFMultiURLDialog::~KMFMultiURLDialog()
{
}

void KMFMultiURLDialog::moveDown()
{
  Q3ListViewItem* item = fileListView->currentItem();
  if(item->itemBelow())
    item->moveItem(item->itemBelow());
  fileListView->ensureItemVisible(item);
}

void KMFMultiURLDialog::moveUp()
{
  Q3ListViewItem* item = fileListView->currentItem();
  if(item->itemAbove())
    item->itemAbove()->moveItem(item);
  fileListView->ensureItemVisible(item);
}

void KMFMultiURLDialog::remove()
{
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
  select(first);
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
  Q3ListViewItem* prev = fileListView->currentItem();
  Q3ListViewItem* first = 0;

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
    prev = new Q3ListViewItem(fileListView, prev, *it);
    if(!first)
      first = prev;
  }
  /*
  KIO::PreviewJob* job =  KIO::filePreview(list, 80, 60);
  connect(job, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
          this, SLOT(gotPreview(const KFileItem*, const QPixmap&)));
  */
  select(first);
}

void KMFMultiURLDialog::select(Q3ListViewItem* item)
{
  for(Q3ListViewItemIterator it(fileListView); *it != 0; ++it)
    (*it)->setSelected(false);
  if(item)
  {
    fileListView->setSelected(item, true);
    fileListView->setCurrentItem(item);
    fileListView->ensureItemVisible(item);
  }
}

QStringList KMFMultiURLDialog::files()
{
  QStringList list;

  for(Q3ListViewItemIterator it(fileListView); *it != 0; ++it)
  {
    list.append((*it)->text(0));
  }
  return list;
}

#include "kmfmultiurldialog.moc"
