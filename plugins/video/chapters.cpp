//**************************************************************************
//   Copyright (C) 2004, 2005 by Petri Damst�
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

#include "chapters.h"
#include "videoplugin.h"
#include "kmfchapterlistview.h"
#include "kmftime.h"
#include "autochapterslayout.h"
#include "addchapterlayout.h"
#include "kmfimageview.h"
#include <kmediafactory/plugin.h>
#include <kapplication.h>
#include <kfiledialog.h>
#include <kmainwindow.h>
#include <kstdaction.h>
#include <kinputdialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <ktimewidget.h>
#include <qpopupmenu.h>
#include <qslider.h>
#include <qimage.h>
#include <qlabel.h>
#include <qdir.h>
#include <qlineedit.h>
#include <qdatetimeedit.h>

Chapters::Chapters(QWidget *parent, const char *name)
  : ChaptersLayout(parent, name), m_obj(0)
{
  video->setScaled(true);
}

Chapters::~Chapters()
{
}

void Chapters::getData(QDVD::CellList& cells, QString* preview) const
{
  KMFChapterListViewItem* item = 0;

  cells.clear();

  for(QListViewItemIterator it(chaptersListView); *it != 0; ++it)
  {
    item = static_cast<KMFChapterListViewItem*>(*it);
    cells.append(QDVD::Cell(item->pos(), item->length(), item->name()));
  }
  cells.last().setLength(QTime());
  *preview = m_preview;
}

void Chapters::setData(const QDVD::CellList& cells,
                       const VideoObject* obj)
{
  for(QDVD::CellList::ConstIterator it = cells.begin();
      it != cells.end(); ++it)
  {
    new KMFChapterListViewItem(chaptersListView, chaptersListView->lastItem(),
                               (*it).name(), (*it).start());
  }
  m_obj = obj;
  chaptersListView->setDuration(KMF::Time(m_obj->duration()));
  timeSlider->setMaxValue((int)KMF::Time(m_obj->duration()));
  m_duration = KMF::Time(m_obj->duration()).toString();
  m_pos = 0.0;
  chaptersListView->setSelected(chaptersListView->firstChild(), true);
  updateVideo();

  /* TODO: Why this DOESN'T work ?????????????
  KMF::Plugin* plugin = m_obj->plugin();
  new KAction(i18n("&Delete all"), "editdelete", CTRL+Key_Delete,
      this, SLOT(deleteAll()), plugin->actionCollection(), "delete_all");
  new KAction(i18n("&Rename all"), "", KShortcut(), this,
      SLOT(renameAll()), plugin->actionCollection(), "rename_all" );
  new KAction(i18n("&Auto cells"), "", KShortcut(), this,
      SLOT(autoCells()), plugin->actionCollection(), "auto_cells");
  new KAction(i18n("&Import"), "", KShortcut(), this,
      SLOT(import()), plugin->actionCollection(), "import" );
  */
}

void Chapters::updateVideo()
{
  QImage img = m_obj->getFrame(m_pos);
  video->setImage(img);

  QString s = QString("%1: %2 / %3").
      arg(m_obj->text()).
      arg(m_pos.toString()).
      arg(m_duration);
  timeLabel->setText(s);

  timeSlider->setValue((int)m_pos);
}

void Chapters::slotSliderMoved(int value)
{
  m_pos = value;
  updateVideo();
}

void Chapters::moveFrames(int direction)
{
  double move = (double)((double)direction * (1.0 / m_obj->frameRate()));
  m_pos += move;
  if(m_pos < KMF::Time(0.0))
    m_pos = 0.0;
  else if(m_pos > m_obj->duration())
    m_pos = m_obj->duration();
  updateVideo();
}

void Chapters::slotNextFrame()
{
  moveFrames(1);
}

void Chapters::slotPrevFrame()
{
  moveFrames(-1);
}

void Chapters::slotForward()
{
  moveFrames(120);
}

void Chapters::slotRewind()
{
  moveFrames(-120);
}

void Chapters::slotStart()
{
  m_pos = 0.0;
  updateVideo();
}

void Chapters::slotEnd()
{
  m_pos = m_obj->duration();
  updateVideo();
}

void Chapters::slotSelectionChanged()
{
  if(chaptersListView->selectedItem())
  {
    m_pos = chaptersListView->selectedItem()->pos();
    updateVideo();
  }
}

void Chapters::slotRemove()
{
  delete chaptersListView->selectedItem();
  QListViewItem* item = chaptersListView->currentItem();
  if(item)
    chaptersListView->setSelected(item, true);
}

void Chapters::slotAdd()
{
  AddChapterLayout dlg(this);

  dlg.chapterTime->setMaxValue(m_obj->duration());
  dlg.chapterTime->setTime(m_pos);
  if(dlg.exec() == QDialog::Accepted)
  {
    KMFChapterListViewItem* item = 0;
    QTime pos = dlg.chapterTime->time();
    QString text = dlg.nameEdit->text();

    // Don't lose milliseconds
    if(pos.hour() == m_pos.hour() && pos.minute() == m_pos.minute() &&
       pos.second() == m_pos.second())
      pos = m_pos;
    for(QListViewItemIterator it(chaptersListView); *it != 0; ++it)
    {
      item = static_cast<KMFChapterListViewItem*>(*it);
      if(pos < item->pos())
        break;
    }
    if(item && pos < item->pos())
      item = static_cast<KMFChapterListViewItem*>(item->itemAbove());
    new KMFChapterListViewItem(chaptersListView, item, text, pos);
  }
}

void Chapters::slotContextMenu(KListView*, QListViewItem*, const QPoint& p)
{
  /* TODO: Why this DOESN'T work ?????????????
  KMF::Plugin* plugin = m_obj->plugin();
  KXMLGUIFactory* factory = plugin->factory();
  QPopupMenu *popup =
      static_cast<QPopupMenu*>(factory->container("cell_popup", plugin));
  if(popup)
    popup->exec(p);
  */
  QPopupMenu *popup = new QPopupMenu( this );
  popup->insertItem(i18n("&Delete all"), this, SLOT(deleteAll()));
  popup->insertItem(i18n("&Rename all"), this, SLOT(renameAll()));
  popup->insertItem(i18n("&Auto chapters"), this, SLOT(autoChapters()));
  popup->insertItem(i18n("Import chapter file", "&Import"),
                    this, SLOT(import()));
  popup->exec(p);
}

void Chapters::renameAll()
{
  AutoChaptersLayout dlg(kapp->activeWindow());
  dlg.intervalLabel->hide();
  dlg.intervalTime->hide();
  dlg.resize(dlg.minimumSize());
  if (dlg.exec())
  {
    QString text = dlg.nameEdit->text();
    int i = 1;

    for(QListViewItemIterator it(chaptersListView); *it != 0; ++it)
    {
      KMFChapterListViewItem* item = static_cast<KMFChapterListViewItem*>(*it);
      item->setName(QString(text).arg(i++));
    }
  }
}

void Chapters::deleteAll()
{
  chaptersListView->clear();
}

void Chapters::autoChapters()
{
  AutoChaptersLayout dlg(kapp->activeWindow());
  if (dlg.exec())
  {
    QString text = dlg.nameEdit->text();
    double interval = (double)dlg.intervalTime->time().msecsTo(QTime())/-1000.0;
    KMF::Time time;
    int i = 1;

    if(interval < 1.0)
      return;
    chaptersListView->clear();
    while(time < m_obj->duration())
    {
      QString s;

      if(!text.isEmpty())
        s = QString(text).arg(i);
      else
        s = time.toString("h:mm:ss");
      new KMFChapterListViewItem(chaptersListView,
                                chaptersListView->lastItem(),
                                s, time);
      time += interval;
      ++i;
    }
  }
}

void Chapters::import()
{
  QString chapterFile = KFileDialog::getOpenFileName(":Chapters",
      "*.*|All files", kapp->mainWidget());

  if(!chapterFile.isEmpty())
  {
    KSimpleConfig chapters(chapterFile, true);
    int i = 1;
    QString entry;

    chaptersListView->clear();
    while((entry = chapters.readEntry(
           "CHAPTER" + QString("%1").arg(i).rightJustify(2, '0'))) !=
           QString::null)
    {
      KMF::Time time(entry);
      QString s = chapters.readEntry(
          "CHAPTER" + QString("%1").arg(i).rightJustify(2, '0') + "NAME",
          QString("%1").arg(i));
      new KMFChapterListViewItem(chaptersListView,
                                 chaptersListView->lastItem(),
                                 s, time);
      ++i;
    }
  }
}

void Chapters::saveCustomPreview( )
{
  int serial = m_obj->projectInterface()->serial();
  QDir dir = m_obj->projectInterface()->projectDir("media");

  m_preview.sprintf("%3.3d_preview.png", serial);
  m_preview = dir.filePath(m_preview);
  video->image().save(m_preview, "PNG");
}

void Chapters::okClicked()
{
  if(chaptersListView->childCount() > 0)
    accept();
  else
  {
    KMessageBox::sorry(this,
                       i18n("You should have atleast one chapter."));
    return;
  }
}

#include "chapters.moc"
