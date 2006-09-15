//**************************************************************************
//   Copyright (C) 2004 by Petri Damst�
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
#include "videooptions.h"
#include "chapters.h"
#include "ui_languageselection.h"
#include "kmflanguagewidgets.h"
#include "videopluginsettings.h"
#include "subtitleoptions.h"
#include <qdvdinfo.h>
#include <kapplication.h>
#include <klocale.h>
#include <kdebug.h>
#include <kurlrequester.h>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>

class LanguageSelection : public KDialog, public Ui::LanguageSelection
{
  public:
    LanguageSelection(QWidget *parent) : KDialog(parent)
    {
      setupUi(mainWidget());
      setButtons(KDialog::Ok | KDialog::Cancel);
    };
};

VideoOptions::VideoOptions(QWidget *parent)
  : KDialog(parent)
{
  setupUi(mainWidget());
  setButtons(KDialog::Ok | KDialog::Cancel);
}

VideoOptions::~VideoOptions()
{
}

void VideoOptions::setData(const VideoObject& obj)
{
#warning TODO
#if 0
  titleEdit->setText(obj.title());
  previewUrl->setUrl(obj.previewUrl().prettyUrl());
  aspectComboBox->setCurrentIndex((int)obj.aspect());

  m_cells = obj.cellList();

  m_audioTracks = obj.audioTracks();
  for(QDVD::AudioList::ConstIterator it = m_audioTracks.begin();
      it != m_audioTracks.end(); ++it)
    new KMFLanguageItem(audioListBox, (*it).language());
  audioListBox->setSelected(0, true);

  m_subtitles = obj.subtitles();
  for(QDVD::SubtitleList::ConstIterator it = m_subtitles.begin();
      it != m_subtitles.end(); ++it)
    new KMFLanguageItem(subtitleListBox, (*it).language());
  subtitleListBox->setSelected(0, true);

  m_conversionParams = obj.conversion();
  m_obj = &obj;
  if(!obj.isDVDCompatible())
  {
    conversionLabel->show();
    conversionPropertiesButton->show();
  }
  else
  {
    conversionLabel->hide();
    conversionPropertiesButton->hide();
  }
  enableButtons();
  updateTexts();
#endif
}

void VideoOptions::getData(VideoObject& obj) const
{
#warning TODO
#if 0
  obj.setTitle(titleEdit->text());
  obj.setCellList(m_cells);
  obj.setPreviewUrl(KUrl(previewUrl->url()));
  obj.setAspect((QDVD::VideoTrack::AspectRatio)aspectComboBox->currentIndex());
  obj.setSubtitles(m_subtitles);
  obj.setAudioTracks(m_audioTracks);
  obj.setConversion(m_conversionParams);
#endif
}

void VideoOptions::audioPropertiesClicked()
{
#warning TODO
#if 0
  LanguageSelection dlg(this);
  dlg.languageListBox->setLanguage(audioListBox->language());
  if (dlg.exec())
  {
    QString lang = dlg.languageListBox->language();
    int n = audioListBox->index(audioListBox->selectedItem());
    m_audioTracks[n].setLanguage(lang);
    audioListBox->setItemLanguage(lang);
  }
#endif
}

void VideoOptions::subtitleAddClicked()
{
#warning TODO
#if 0
  QDVD::Subtitle subtitle;
  SubtitleOptions dlg(this);

  subtitle.setLanguage(VideoPluginSettings::defaultSubtitleLanguage());
  dlg.setData(subtitle);
  if (dlg.exec())
  {
    dlg.getData(subtitle);
    KMFLanguageItem* item = new KMFLanguageItem(subtitleListBox,
                                                subtitle.language());
    subtitleListBox->setSelected(item, true);
    m_subtitles.append(subtitle);
  }
  enableButtons();
#endif
}

void VideoOptions::subtitleRemoveClicked()
{
#warning TODO
#if 0
  int n = subtitleListBox->index(subtitleListBox->selectedItem());

  m_subtitles.remove(m_subtitles.at(n));
  subtitleListBox->removeItem(n);
  enableButtons();
#endif
}

void VideoOptions::subtitlePropertiesClicked()
{
#warning TODO
#if 0
  int n = subtitleListBox->index(subtitleListBox->selectedItem());

  if(isSelectedSubtitleInVideo())
  {
    LanguageSelectionLayout dlg(this);
    dlg.languageListBox->setLanguage(subtitleListBox->language());
    if (dlg.exec())
    {
      QString lang = dlg.languageListBox->language();
      m_subtitles[n].setLanguage(lang);
      subtitleListBox->setItemLanguage(lang);
    }
  }
  else
  {
    SubtitleOptions dlg(this);
    dlg.setData(m_subtitles[n]);
    if (dlg.exec())
    {
      dlg.getData(m_subtitles[n]);
      subtitleListBox->setItemLanguage(m_subtitles[n].language());
    }
  }
#endif
}

void VideoOptions::chaptersClicked()
{
  Chapters dlg(this);
  dlg.setData(m_cells, m_obj);
  if (dlg.exec())
  {
    QString preview;
    dlg.getData(m_cells, &preview);
    if(!preview.isEmpty())
      previewUrl->setUrl(preview);
    updateTexts();
  }
}

void VideoOptions::enableButtons()
{
  audioPropertiesButton->setEnabled(m_audioTracks.count() > 0);
  subtitleRemoveButton->setEnabled(m_subtitles.count() > 0 &&
                                   !isSelectedSubtitleInVideo());
  subtitlePropertiesButton->setEnabled(m_subtitles.count() > 0);
}

void VideoOptions::updateTexts()
{
  int chapters = m_cells.count();
  chapterLabel->setText(
      i18np("Video has %1 chapter", "Video has %1 chapters", chapters)
      .arg(chapters));
}

bool VideoOptions::isSelectedSubtitleInVideo()
{
#warning TODO
#if 0
  if(m_subtitles.count() > 0 && subtitleListBox->selectedItem())
  {
    int n = subtitleListBox->index(subtitleListBox->selectedItem());
    return m_subtitles[n].file().isEmpty();
  }
#endif
  return false;
}

#include "videooptions.moc"
