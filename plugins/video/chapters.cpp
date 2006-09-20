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
#include "kmftools.h"
#include "ui_autochapters.h"
#include "ui_addchapter.h"
#include <qdvdinfo.h>
#include <kmfimageview.h>
#include <kmediafactory/plugin.h>
#include <kapplication.h>
#include <kfiledialog.h>
#include <kmainwindow.h>
#include <kstdaction.h>
#include <kinputdialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <QMenu>
#include <QSlider>
#include <QImage>
#include <QLabel>
#include <QDir>
#include <QLineEdit>

class CellListModel : public QAbstractListModel
{
  public:
    CellListModel(const QDVD::CellList& data) : m_data(data) {};

    virtual int rowCount(const QModelIndex&) const
    {
      return m_data.count();
    };

    virtual int columnCount(const QModelIndex&) const
    {
      return 3;
    };

    virtual QVariant data(const QModelIndex &index, int role) const
    {
      if (!index.isValid())
        return QVariant();

      if (index.row() >= rowCount(index))
        return QVariant();

      if (role == Qt::DisplayRole)
      {
        switch(index.column())
        {
          case 0:
            return m_data.at(index.row()).name();
          case 1:
            return KMF::Time(m_data.at(index.row()).start()).toString();
          case 2:
            return KMF::Time(m_data.at(index.row()).length()).toString();
        }
      }
      return QVariant();
    };

    virtual QVariant headerData(int column, Qt::Orientation, int role) const
    {
      if (role != Qt::DisplayRole)
        return QVariant();

      switch(column)
      {
        case 0:
          return i18n("Name");
        case 1:
          return i18n("Start");
        case 2:
          return i18n("Length");
      }
      return "";
    };

  private:
    const QDVD::CellList& m_data;
};

class AddChapter : public KDialog, public Ui::AddChapter
{
  public:
    AddChapter(QWidget *parent) : KDialog(parent)
    {
      setupUi(mainWidget());
      setButtons(KDialog::Ok | KDialog::Cancel);
      setCaption(i18n("Add chapters"));
    };
};

class AutoChapters : public KDialog, public Ui::AutoChapters
{
  public:
    AutoChapters(QWidget *parent) : KDialog(parent)
    {
      setupUi(mainWidget());
      setButtons(KDialog::Ok | KDialog::Cancel);
      setCaption(i18n("Auto chapters"));
    };
};

Chapters::Chapters(QWidget *parent)
  : KDialog(parent), m_obj(0), m_model(0)
{
  setupUi(mainWidget());
  setButtons(KDialog::Ok | KDialog::Cancel);
  setCaption(i18n("Chapters"));

  connect(startButton, SIGNAL(clicked()), this, SLOT(slotStart()));
  connect(endButton, SIGNAL(clicked()), this, SLOT(slotEnd()));
  connect(addButton, SIGNAL(clicked()), this, SLOT(slotAdd()));
  connect(removeButton, SIGNAL(clicked()), this, SLOT(slotRemove()));
  connect(chaptersListView, SIGNAL(selectionChanged()),
          this, SLOT(slotSelectionChanged()));
  connect(timeSlider, SIGNAL(sliderMoved(int)),
          this, SLOT(slotSliderMoved(int)));
  connect(fwdButton, SIGNAL(clicked()), this, SLOT(slotForward()));
  connect(rewButton, SIGNAL(clicked()), this, SLOT(slotRewind()));
  connect(nextButton, SIGNAL(clicked()), this, SLOT(slotNextFrame()));
  connect(prevButton, SIGNAL(clicked()), this, SLOT(slotPrevFrame()));
  connect(chaptersListView, SIGNAL(contextMenu(KListView*,QListViewItem*,const QPoint&)),
          this, SLOT( slotContextMenu(KListView*,QListViewItem*,const QPoint&)));
  connect(customPreviewButton, SIGNAL(clicked()),
          this, SLOT(saveCustomPreview()));

  video->setScaled(true);
}

Chapters::~Chapters()
{
}

void Chapters::getData(QDVD::CellList& cells, QString* preview) const
{
  cells = m_cells;
  *preview = m_preview;
  delete m_model;
}

void Chapters::setData(const QDVD::CellList& cells,
                       const VideoObject* obj)
{
  m_cells = cells;
  m_model = new CellListModel(m_cells);
  chaptersListView->setModel(m_model);
  m_obj = obj;
  //chaptersListView->setDuration(KMF::Time(m_obj->duration()));
  //timeSlider->setMaximum((int)KMF::Time(m_obj->duration()));
  //m_duration = KMF::Time(m_obj->duration()).toString();
  //m_pos = 0.0;
  Q_SELECT_FIRST(chaptersListView, m_model);
  //updateVideo();

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
#warning TODO
/*
  if(chaptersListView->selectedItem())
  {
    m_pos = chaptersListView->selectedItem()->pos();
    updateVideo();
  }
  */
}

void Chapters::slotRemove()
{
  int i = Q_SELECTED_INDEX(chaptersListView);
  m_cells.removeAt(i);
  if(i > 0)
    --i;
  Q_SELECT(chaptersListView, m_model, i);
  m_cells.last().setLength(QTime(0, 0, 0, 0));
  chaptersListView->viewport()->update();
}

void Chapters::slotAdd()
{
#warning TODO
/*
  AddChapter dlg(this);

  dlg.chapterTime->setMaximumTime(m_obj->duration());
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
    for(Q3ListViewItemIterator it(chaptersListView); *it != 0; ++it)
    {
      item = static_cast<KMFChapterListViewItem*>(*it);
      if(pos < item->pos())
        break;
    }
    if(item && pos < item->pos())
      item = static_cast<KMFChapterListViewItem*>(item->itemAbove());
    new KMFChapterListViewItem(chaptersListView, item, text, pos);
  }
  */
}

void Chapters::slotContextMenu(Q3ListView*, Q3ListViewItem*, const QPoint& p)
{
  /* TODO: Why this DOESN'T work ?????????????
  KMF::Plugin* plugin = m_obj->plugin();
  KXMLGUIFactory* factory = plugin->factory();
  QPopupMenu *popup =
      static_cast<QPopupMenu*>(factory->container("cell_popup", plugin));
  if(popup)
    popup->exec(p);
  */
  QMenu *popup = new QMenu( this );
  popup->insertItem(i18n("&Delete all"), this, SLOT(deleteAll()));
  popup->insertItem(i18n("&Rename all"), this, SLOT(renameAll()));
  popup->insertItem(i18n("&Auto chapters"), this, SLOT(autoChapters()));
  popup->insertItem(i18nc("Import chapter file", "&Import"),
                    this, SLOT(import()));
  popup->exec(p);
}

void Chapters::renameAll()
{
#warning TODO
/*
  AutoChapters dlg(kapp->activeWindow());
  dlg.intervalLabel->hide();
  dlg.intervalTime->hide();
  dlg.resize(dlg.minimumSize());
  if (dlg.exec())
  {
    QString text = dlg.nameEdit->text();
    int i = 1;

    for(Q3ListViewItemIterator it(chaptersListView); *it != 0; ++it)
    {
      KMFChapterListViewItem* item = static_cast<KMFChapterListViewItem*>(*it);
      item->setName(QString(text).arg(i++));
    }
  }
  */
}

void Chapters::deleteAll()
{
#warning TODO
/*
  chaptersListView->clear();
  */
}

void Chapters::autoChapters()
{
#warning TODO
/*
  AutoChapters dlg(kapp->activeWindow());
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
  */
}

void Chapters::import()
{
#warning TODO
/*

  QString chapterFile = KFileDialog::getOpenFileName(
      KUrl("kfiledialog:///<Chapters"), "*.*|All files");

  if(!chapterFile.isEmpty())
  {
    KSimpleConfig chapters(chapterFile, true);
    int i = 1;
    QString entry;

    chaptersListView->clear();
    while((entry = chapters.readEntry(
           QString("CHAPTER%1").arg(i).rightJustified(2, '0'),
           QString(""))) != QString(""))
    {
      KMF::Time time(entry);
      QString s = chapters.readEntry(
          QString("CHAPTER%1NAME").arg(i).rightJustified(2, '0'),
          QString("%1").arg(i));
      new KMFChapterListViewItem(chaptersListView,
                                 chaptersListView->lastItem(),
                                 s, time);
      ++i;
    }
  }
  */
}

void Chapters::saveCustomPreview( )
{
  int serial = m_obj->projectInterface()->serial();
  QDir dir = m_obj->projectInterface()->projectDir("media");

  m_preview.sprintf("%3.3d_preview.png", serial);
  m_preview = dir.filePath(m_preview);
  video->image().save(m_preview, "PNG");
}

void Chapters::accept()
{
  if(m_cells.count() > 0)
    KDialog::accept();
  else
  {
    KMessageBox::sorry(this,
                       i18n("You should have atleast one chapter."));
  }
}

#include "chapters.moc"
