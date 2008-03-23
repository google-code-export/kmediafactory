//**************************************************************************
//   Copyright (C) 2004, 2005 by Petri Damstén
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
#include <qffmpeg.h>
#include <kurlrequester.h>
#include <kmessagebox.h>
#include <kio/previewjob.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qlistview.h>
#include <qlineedit.h>

SlideshowProperties::SlideshowProperties(QWidget *parent, const char *name)
  : SlideshowPropertiesLayout(parent, name)
{
  slideListView->setDefaultRenameAction(QListView::Accept);
  // User sorting
  slideListView->setSorting(1000);
  audioButton->setIconSet(
      KGlobal::iconLoader()->loadIconSet(("arts"), KIcon::Small,
      KIcon::SizeMedium));
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

  SlideList list;
  for(QListViewItemIterator it(slideListView); *it != 0; ++it)
  {
    QCheckListItem* litem = static_cast<QCheckListItem*>(*it);
    Slide slide;

    slide.chapter = litem->isOn();
    slide.picture = litem->text(4);
    slide.comment = litem->text(3);
    list.append(slide);
  }
  obj.setSlides(list);
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
  QCheckListItem* prev =
      static_cast<QCheckListItem*>(slideListView->currentItem());
  QCheckListItem* first = 0;
  KFileItemList list;

  for(SlideList::ConstIterator it = slides.begin();
      it != slides.end(); ++it)
  {
    KFileItem* item = new KFileItem(KFileItem::Unknown, KFileItem::Unknown,
                                    (*it).picture);
    prev = new QCheckListItem(slideListView, prev, "",
                              QCheckListItem::CheckBox);
    if(!first)
      first = prev;
    QFileInfo fi((*it).picture);
    prev->setText(2, fi.fileName());
    prev->setText(3, (*it).comment);
    prev->setText(4, (*it).picture);
    prev->setRenameEnabled(3, true);
    prev->setState(((*it).chapter) ? QCheckListItem::On : QCheckListItem::Off);
    list.append(item);
  }
  KIO::PreviewJob* job =  KIO::filePreview(list, 80, 60);
  connect(job, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
          this, SLOT(gotPreview(const KFileItem*, const QPixmap&)));
  select(first);
  updateInfo();
}

void SlideshowProperties::select(QListViewItem* item)
{
  for(QListViewItemIterator it(slideListView); *it != 0; ++it)
    (*it)->setSelected(false);
  if(item)
  {
    slideListView->setSelected(item, true);
    slideListView->setCurrentItem(item);
    slideListView->ensureItemVisible(item);
  }
}

void SlideshowProperties::gotPreview(const KFileItem* item,
                                     const QPixmap& pixmap)
{

  for(QListViewItemIterator it(slideListView); *it != 0; ++it)
  {
    QCheckListItem* litem = static_cast<QCheckListItem*>(*it);
    if(item->url() == litem->text(4))
    {
      litem->setPixmap(1, pixmap);
      break;
    }
  }
}

void SlideshowProperties::moveDown()
{
  QListViewItem* item = slideListView->currentItem();
  if(item->itemBelow())
    item->moveItem(item->itemBelow());
  slideListView->ensureItemVisible(item);
}

void SlideshowProperties::moveUp()
{
  QListViewItem* item = slideListView->currentItem();
  if(item->itemAbove())
    item->itemAbove()->moveItem(item);
  slideListView->ensureItemVisible(item);
}

void SlideshowProperties::updateInfo()
{
  QString info(i18n("Info: "));
  int count = slideListView->childCount();
  KMF::Time duration = (double)durationSpinBox->value();
  KMF::Time audioDuration = 0.0;

  for(QStringList::ConstIterator it = m_audioFiles.begin();
      it != m_audioFiles.end(); ++it)
  {
    QFFMpeg audio(*it);
    audioDuration += audio.duration();
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
}

void SlideshowProperties::remove()
{
  QListViewItemIterator it(slideListView);
  QListViewItem* first = 0;

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
}

void SlideshowProperties::add()
{
  QStringList pics = KFileDialog::getOpenFileNames(":AddSlideshow",
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
  KMFMultiURLDialog dlg(":SlideshowAudioFiles",
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
  for(QListViewItemIterator it(slideListView); *it != 0; ++it)
  {
    QCheckListItem* litem = static_cast<QCheckListItem*>(*it);

    if(litem->isOn())
    {
      accept();
      return;
    }
  }
  KMessageBox::sorry(this,
                      i18n("You should have atleast one chapter."));
}

#include "slideshowproperties.moc"
