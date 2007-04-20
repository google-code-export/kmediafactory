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
#include "slideshowproperties.h"
#include "slideshowobject.h"
#include <kmftime.h>
#include <kmfmultiurldialog.h>
#include <kurlrequester.h>
#include <kmessagebox.h>
#include <kio/previewjob.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <QPixmap>
#include <QSpinBox>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QFileInfo>

#warning TODO drag & drop support

int SlideListModel::columnCount(const QModelIndex&) const
{
  return 2;
};

Qt::ItemFlags SlideListModel::flags(const QModelIndex& index) const
{
  Qt::ItemFlags flags = QAbstractItemModel::flags(index);
  if(!isValid(index))
    return flags | Qt::ItemIsDropEnabled;

  flags |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
  if(index.column() == 0)
    flags |= Qt::ItemIsUserCheckable;
  else if(index.column() == 1)
    flags |= Qt::ItemIsEditable;
  return flags;
}

QVariant SlideListModel::data(const QModelIndex &index, int role) const
{
  if (!isValid(index))
    return QVariant();

  if (role == Qt::DisplayRole)
  {
    switch(index.column())
    {
      case 0:
      {
        QFileInfo fi(at(index).picture);
        return fi.fileName();
      }
      case 1:
        return at(index).comment;
    }
  }
  if (role == Qt::DecorationRole)
  {
    if(index.column() == 0)
      return QPixmap();
  }
  if (role == Qt::CheckStateRole)
  {
    if(index.column() == 0)
      return ((at(index).chapter) ? Qt::Checked : Qt::Unchecked);
  }
  return QVariant();
};

bool SlideListModel::setData(const QModelIndex &index, const QVariant &value,
                             int role)
{
  if(!isValid(index))
    return false;

  Slide slide = at(index);

  if (role == Qt::EditRole)
  {
    if(index.column() == 2)
      slide.comment = value.toString();
  }
  else if (role == Qt::CheckStateRole)
  {
    slide.chapter = value.toBool();
  }
  replace(index, slide);
  return true;
}

QVariant SlideListModel::headerData(int column, Qt::Orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  switch(column)
  {
    case 0:
      return i18n("Picture");
    case 1:
      return i18n("Comment");
  }
  return "";
};

SlideshowProperties::SlideshowProperties(QWidget *parent)
  : KDialog(parent)
{
  setupUi(mainWidget());
  setButtons(KDialog::Ok | KDialog::Cancel);

  slideListView->setModel(&m_model);
  slideListView->setRootIsDecorated(false);
  audioButton->setIcon(KIcon("arts"));
  audioButton->setIconSize(QSize(K3Icon::SizeLarge, K3Icon::SizeLarge));
  addButton->setIcon(KIcon("list-add"));
  removeButton->setIcon(KIcon("list-remove"));
  upButton->setIcon(KIcon("arrow-up"));
  downButton->setIcon(KIcon("arrow-down"));
}

SlideshowProperties::~SlideshowProperties()
{
}

void SlideshowProperties::getData(SlideshowObject& obj) const
{
  obj.setSlideDuration(durationSpinBox->value());
  obj.setIncludeOriginals(addOriginalsCheckBox->isChecked());
  obj.setLoop(loopCheckBox->isChecked());
  obj.setTitle(titleEdit->text());
  obj.setAudioFile(m_audioFiles);
  obj.setSlides(m_model.list());
}

void SlideshowProperties::setData(const SlideshowObject& obj)
{
  durationSpinBox->setValue((int)obj.slideDuration());
  addOriginalsCheckBox->setChecked(obj.includeOriginals());
  loopCheckBox->setChecked(obj.loop());
  m_audioFiles = obj.audioFile();
  titleEdit->setText(obj.title());

  addSlides(obj.slides());
  m_sob = &obj;
}

void SlideshowProperties::addSlides(const SlideList& slides)
{
  m_model.setList(slides);
#warning TODO
/*
  Q3CheckListItem* prev =
      static_cast<Q3CheckListItem*>(slideListView->currentItem());
  Q3CheckListItem* first = 0;
  KFileItemList list;

  for(SlideList::ConstIterator it = slides.begin();
      it != slides.end(); ++it)
  {
    KFileItem* item = new KFileItem(KFileItem::Unknown, KFileItem::Unknown,
                                    (*it).picture);
    prev = new Q3CheckListItem(slideListView, prev, "",
                              Q3CheckListItem::CheckBox);
    if(!first)
      first = prev;
    QFileInfo fi((*it).picture);
    prev->setText(2, fi.fileName());
    prev->setText(3, (*it).comment);
    prev->setText(4, (*it).picture);
    prev->setRenameEnabled(3, true);
    prev->setState(((*it).chapter) ? Q3CheckListItem::On : Q3CheckListItem::Off);
    list.append(item);
  }
  KIO::PreviewJob* job =  KIO::filePreview(list, 80, 60);
  connect(job, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
          this, SLOT(gotPreview(const KFileItem*, const QPixmap&)));
  select(first);
  updateInfo();
  */
}

void SlideshowProperties::gotPreview(const KFileItem* item,
                                     const QPixmap& pixmap)
{
#warning TODO
/*
  for(Q3ListViewItemIterator it(slideListView); *it != 0; ++it)
  {
    Q3CheckListItem* litem = static_cast<Q3CheckListItem*>(*it);
    if(item->url() == litem->text(4))
    {
      litem->setPixmap(1, pixmap);
      break;
    }
  }
  */
}

void SlideshowProperties::moveDown()
{
#warning TODO
/*
  Q3ListViewItem* item = slideListView->currentItem();
  if(item->itemBelow())
    item->moveItem(item->itemBelow());
  slideListView->ensureItemVisible(item);
  */
}

void SlideshowProperties::moveUp()
{
#warning TODO
/*
  Q3ListViewItem* item = slideListView->currentItem();
  if(item->itemAbove())
    item->itemAbove()->moveItem(item);
  slideListView->ensureItemVisible(item);
  */
}

void SlideshowProperties::updateInfo()
{
#warning TODO
/*

  QString info(i18n("Info: "));
  int count = slideListView->childCount();
  KMF::Time duration = (double)durationSpinBox->value();
  KMF::Time audioDuration = 0.0;

  for(QStringList::ConstIterator it = m_audioFiles.begin();
      it != m_audioFiles.end(); ++it)
  {
# warning TODO: Get audio duration using tools that dvdslideshow depends
    //QFFMpeg audio(*it);
    //audioDuration += audio.duration();
  }
  info += i18n("%1 images").arg(count);
  if(duration < KMF::Time(1.0))
    info += i18n(", Duration %1").arg(audioDuration.toString("h:mm:ss"));
  else
  {
    duration = (1.0 + duration.toSeconds()) * count + 1.0;
    info += i18n(", Duration %1").arg(duration.toString("h:mm:ss"));
  }
  if(!audioDuration.isNull())
    info += i18n(", Audio duration %1").arg(audioDuration.toString("h:mm:ss"));

  infoLabel->setText(info);
  */
}

void SlideshowProperties::remove()
{
#warning TODO
/*
  Q3ListViewItemIterator it(slideListView);
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
    first = slideListView->firstChild();
  select(first);
  updateInfo();
  */
}

void SlideshowProperties::add()
{
  QStringList pics = KFileDialog::getOpenFileNames(
      KUrl("kfiledialog:///<AddSlideshow>"),
      i18n("*.jpg *.png *.pdf *.odp *.odt *.ods *.odx *.sxw *.sxc *.sxi \
            *.ppt *.xls *.doc|Pictures, Presentations\n*.*|All files"),
      this);

  if(pics.count() > 0)
  {
    addSlides(m_sob->slideList(pics));
  }
}

void SlideshowProperties::audioClicked()
{
  KMFMultiURLDialog dlg("kfiledialog:///<SlideshowAudioFiles>",
                        i18n("*.mp3 *.wav *.ogg|Audio Files"),
                        this, i18n("Audio files"));

  dlg.addFiles(m_audioFiles);
  if(dlg.exec())
  {
    m_audioFiles = dlg.files();
    updateInfo();
  }
}

void SlideshowProperties::okClicked()
{
#warning TODO
/*
  for(Q3ListViewItemIterator it(slideListView); *it != 0; ++it)
  {
    Q3CheckListItem* litem = static_cast<Q3CheckListItem*>(*it);

    if(litem->isOn())
    {
      accept();
      return;
    }
  }
  KMessageBox::sorry(this,
                      i18n("You should have atleast one chapter."));
                      */
}

#include "slideshowproperties.moc"
