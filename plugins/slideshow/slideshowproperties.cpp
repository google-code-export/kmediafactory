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
#include "slideshowproperties.h"
#include "slideshowobject.h"
#include "subtitleoptions.h"
#include <kmftime.h>
#include <kmfmultiurldialog.h>
#include <kmfmediafile.h>
#include <KUrlRequester>
#include <KMessageBox>
#include <kio/previewjob.h>
#include <KFileDialog>
#include <KLocale>
#include <KApplication>
#include <KDebug>
#include <KIconLoader>
#include <QPixmap>
#include <QSpinBox>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QFileInfo>
#include <QMimeData>
#include <QBoxLayout>

enum Columns
{
    COL_IMG,
    COL_CHAPTER,
    COL_COMMENT,

    COL_COUNT
};

int SlideListModel::columnCount(const QModelIndex&) const
{
  return COL_COUNT;
}

Qt::ItemFlags SlideListModel::flags(const QModelIndex& index) const
{
  Qt::ItemFlags flags = QAbstractItemModel::flags(index);
  if(!isValid(index))
    return flags;

  if(COL_CHAPTER==index.column())
    flags |= Qt::ItemIsUserCheckable;
  else if(COL_COMMENT==index.column())
    flags |= Qt::ItemIsEditable;
  return flags;
}

QVariant SlideListModel::data(const QModelIndex &index, int role) const
{
  if (!isValid(index))
    return QVariant();

  switch(role)
  {
    case Qt::DisplayRole:
      switch(index.column())
      {
        case COL_IMG:
        {
          QFileInfo fi(at(index).picture);
          return fi.fileName();
        }
        case COL_COMMENT:
          return at(index).comment;
        default:
          break;
      }
      break;
    case Qt::DecorationRole:
      if(COL_IMG==index.column())
      {
        if(m_previews.keys().indexOf(at(index).picture) >= 0)
          return m_previews[at(index).picture];
        else
          return QPixmap();
      }
      break;
    case Qt::CheckStateRole:
      if(COL_CHAPTER==index.column())
        return ((at(index).chapter) ? Qt::Checked : Qt::Unchecked);
      break;
    case Qt::EditRole:
      if(COL_COMMENT==index.column())
        return at(index).comment;
    default:
      break;
  }
  return QVariant();
}

bool SlideListModel::setData(const QModelIndex &index, const QVariant &value,
                             int role)
{
  kDebug();
  if(!isValid(index))
    return false;

  Slide slide = at(index);

  if (role == Qt::EditRole)
  {
    if(COL_COMMENT==index.column())
    {
      slide.comment = value.toString();
    }
  }
  else if (role == Qt::CheckStateRole)
  {
    if(COL_CHAPTER==index.column())
    {
      slide.chapter = value.toBool();
    }
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
    case COL_IMG:
      return i18n("Image");
    case COL_CHAPTER:
      return i18n("Chapter");
    case COL_COMMENT:
      return i18n("Comment");
  }
  return "";
}

void SlideListModel::setPreview(const QString& file, const QPixmap& pixmap)
{
  int i;

  for(i = 0; i < m_lst.count(); ++i)
  {
    if(m_lst[i].picture == file)
      break;
  }
  m_previews[file] = pixmap;
  emit dataChanged(index(i), index(i));
}

SlideshowProperties::SlideshowProperties(QWidget *parent, bool showSubPage)
  : KDialog(parent)
{
  setupUi(mainWidget());
  setButtons(KDialog::Ok | KDialog::Cancel);
  setCaption(i18n("Slideshow Properties"));

  if(showSubPage)
  {
      QWidget *widget=new QWidget(tabWidget);
      QBoxLayout *layout=new QBoxLayout(QBoxLayout::LeftToRight, widget);
      layout->addWidget(m_subtitleWidget=new SubtitleOptionsWidget(tabWidget, false));
      tabWidget->insertTab(2, widget, i18n("Subtitles"));
  }
  slideListView->setModel(&m_model);
  slideListView->setRootIsDecorated(false);
  slideListView->setDragEnabled(true);
  slideListView->setAcceptDrops(true);
  slideListView->setDragDropMode(QAbstractItemView::DragDrop);
  slideListView->setDropIndicatorShown(true);
  slideListView->setDragDropOverwriteMode(false);
  audioListView->setModel(&m_audioModel);
  audioListView->setWordWrap(true);

  addButton->setIcon(KIcon("list-add"));
  removeButton->setIcon(KIcon("list-remove"));
  upButton->setIcon(KIcon("arrow-up"));
  downButton->setIcon(KIcon("arrow-down"));
  addAudioButton->setIcon(KIcon("list-add"));
  removeAudioButton->setIcon(KIcon("list-remove"));
  upAudioButton->setIcon(KIcon("arrow-up"));
  downAudioButton->setIcon(KIcon("arrow-down"));

  connect(downButton, SIGNAL(clicked()), this, SLOT(moveDown()));
  connect(upButton, SIGNAL(clicked()), this, SLOT(moveUp()));
  connect(addButton, SIGNAL(clicked()), this, SLOT(add()));
  connect(removeButton, SIGNAL(clicked()), this, SLOT(remove()));
  connect(downAudioButton, SIGNAL(clicked()), this, SLOT(moveDownAudio()));
  connect(upAudioButton, SIGNAL(clicked()), this, SLOT(moveUpAudio()));
  connect(addAudioButton, SIGNAL(clicked()), this, SLOT(addAudio()));
  connect(removeAudioButton, SIGNAL(clicked()), this, SLOT(removeAudio()));
  connect(durationSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateInfo()));
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
  obj.setAudioFiles(m_audioModel.list());
  obj.setSlides(m_model.list());
  if(m_subtitleWidget)
  {
      QDVD::Subtitle sub;
      m_subtitleWidget->getData(sub);
      obj.setSubtitleSettings(sub);
  }
}

void SlideshowProperties::setData(const SlideshowObject& obj)
{
  durationSpinBox->setValue((int)obj.slideDuration());
  addOriginalsCheckBox->setChecked(obj.includeOriginals());
  loopCheckBox->setChecked(obj.loop());
  addAudio(obj.audioFiles());
  titleEdit->setText(obj.title());
  if(m_subtitleWidget)
      m_subtitleWidget->setData(obj.subtitleSettings());

  addSlides(obj.slides());
  m_sob = &obj;
}

void SlideshowProperties::addSlides(const SlideList& slides)
{
  QModelIndex current = slideListView->currentIndex();
  QList<KFileItem> list;

  m_model.insert(current, slides);

  foreach(Slide slide, slides)
  {
    KFileItem item(KFileItem::Unknown, KFileItem::Unknown, slide.picture);
    list.append(item);
  }
  KIO::PreviewJob* job =  KIO::filePreview(list, 80, 60);
  connect(job, SIGNAL(gotPreview(const KFileItem&, const QPixmap&)),
          this, SLOT(gotPreview(const KFileItem&, const QPixmap&)));
  updateInfo();
}

void SlideshowProperties::addAudio(const QStringList& files)
{
  for(QStringList::ConstIterator it = files.begin();
      it != files.end(); ++it)
  {
    QFileInfo fi(*it);

    if(fi.isDir())
    {
      KMessageBox::error(kapp->activeWindow(),
                         i18n("Cannot add folder."));
      continue;
    }
    m_audioModel.append(*it);
  }
  audioListView->setCurrentIndex(m_audioModel.index(0));
}

void SlideshowProperties::gotPreview(const KFileItem& item,
                                     const QPixmap& pixmap)
{
  m_model.setPreview(item.url().path(), pixmap);
}

void SlideshowProperties::moveDown()
{
  if(m_model.count()>1)
  {
    QModelIndex item1 = slideListView->currentIndex();
    QModelIndex item2 = m_model.index(item1.row() + 1);
    m_model.swap(item1, item2);
    slideListView->setCurrentIndex(item2);
    slideListView->scrollTo(item2);
  }
}

void SlideshowProperties::moveUp()
{
  if(m_model.count()>1)
  {
    QModelIndex item1 = slideListView->currentIndex();
    QModelIndex item2 = m_model.index(item1.row() - 1);
    m_model.swap(item1, item2);
    slideListView->setCurrentIndex(item2);
    slideListView->scrollTo(item2);
  }
}

void SlideshowProperties::moveDownAudio()
{
  if(m_audioModel.count()>1)
  {
    QModelIndex item1 = audioListView->currentIndex();
    QModelIndex item2 = m_audioModel.index(item1.row() + 1);
    m_audioModel.swap(item1, item2);
    audioListView->setCurrentIndex(item2);
    audioListView->scrollTo(item2);
  }
}

void SlideshowProperties::moveUpAudio()
{
  if(m_audioModel.count()>1)
  {
    QModelIndex item1 = audioListView->currentIndex();
    QModelIndex item2 = m_audioModel.index(item1.row() - 1);
    m_audioModel.swap(item1, item2);
    audioListView->setCurrentIndex(item2);
    audioListView->scrollTo(item2);
  }
}

void SlideshowProperties::updateInfo()
{
  KMF::Time duration = (double)(durationSpinBox->value()*m_model.count()) ;
  KMF::Time audioDuration = 0.0;
  QStringList audioFiles=m_audioModel.list();

  foreach(QString file, audioFiles)
  {
    KMFMediaFile audio = KMFMediaFile::mediaFile(file);
    audioDuration += audio.duration();
  }

  infoLabel->setText(i18n("<i>Images: %1, Duration: %2, Audio duration: %3</i>", m_model.count(), duration.toString("h:mm:ss"), audioDuration.toString("h:mm:ss")));
}

void SlideshowProperties::remove()
{
  QModelIndexList selected = slideListView->selectionModel()->selectedIndexes();
  m_model.removeAt(selected);
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
    addSlides(m_sob->slideList(pics, this));
  }
}

void SlideshowProperties::removeAudio()
{
  QModelIndexList selected = audioListView->selectionModel()->selectedIndexes();
  m_audioModel.removeAt(selected);
  audioListView->setCurrentIndex(m_audioModel.index(0));
  updateInfo();
}

void SlideshowProperties::addAudio()
{
  QStringList files = KFileDialog::getOpenFileNames(
      KUrl("kfiledialog:///<SlideshowAudioFiles>"),
      i18n("*.mp3 *.wav *.ogg|Audio Files"),
      this);

  if(files.count() > 0)
  {
    addAudio(files);
    updateInfo();
  }
}

void SlideshowProperties::okClicked()
{
  SlideList slides = m_model.list();

  foreach(Slide slide, slides)
  {
    if(slide.chapter)
    {
      accept();
      return;
    }
  }
  KMessageBox::sorry(this,
                      i18n("You should have atleast one chapter."));
}

#include "slideshowproperties.moc"
