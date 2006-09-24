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
#include <Q3ListView>

SlideshowProperties::SlideshowProperties(QWidget *parent)
  : KDialog(parent)
{
  setupUi(mainWidget());
  setButtons(KDialog::Ok | KDialog::Cancel);

  slideListView->setDefaultRenameAction(Q3ListView::Accept);
  // User sorting
  slideListView->setSorting(1000);
  audioButton->setIcon(
      KGlobal::iconLoader()->loadIconSet(("arts"), K3Icon::Small,
      K3Icon::SizeMedium));
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
  for(Q3ListViewItemIterator it(slideListView); *it != 0; ++it)
  {
    Q3CheckListItem* litem = static_cast<Q3CheckListItem*>(*it);
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
}

void SlideshowProperties::select(Q3ListViewItem* item)
{
  for(Q3ListViewItemIterator it(slideListView); *it != 0; ++it)
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

  for(Q3ListViewItemIterator it(slideListView); *it != 0; ++it)
  {
    Q3CheckListItem* litem = static_cast<Q3CheckListItem*>(*it);
    if(item->url() == litem->text(4))
    {
      litem->setPixmap(1, pixmap);
      break;
    }
  }
}

void SlideshowProperties::moveDown()
{
  Q3ListViewItem* item = slideListView->currentItem();
  if(item->itemBelow())
    item->moveItem(item->itemBelow());
  slideListView->ensureItemVisible(item);
}

void SlideshowProperties::moveUp()
{
  Q3ListViewItem* item = slideListView->currentItem();
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
}

void SlideshowProperties::remove()
{
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
}

#include "slideshowproperties.moc"
