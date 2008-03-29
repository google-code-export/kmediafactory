//**************************************************************************
//   Copyright (C) 2004 by Petri Damstén
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
#include "languageselectionlayout.h"
#include "kmflanguagewidgets.h"
#include "videopluginsettings.h"
#include "subtitleoptions.h"
#include "conversion.h"
#include <qdvdinfo.h>
#include <kapplication.h>
#include <klocale.h>
#include <kdebug.h>
#include <kurlrequester.h>
#include <ktimewidget.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlabel.h>

VideoOptions::VideoOptions(QWidget *parent, const char *name)
  : VideoOptionsLayout(parent, name)
{
}

VideoOptions::~VideoOptions()
{
}

void VideoOptions::setData(const VideoObject& obj)
{
  titleEdit->setText(obj.title());
  previewURL->setURL(obj.previewURL().prettyURL());
  aspectComboBox->setCurrentItem((int)obj.aspect());

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
}

void VideoOptions::getData(VideoObject& obj) const
{
  obj.setTitle(titleEdit->text());
  obj.setCellList(m_cells);
  obj.setPreviewURL(KURL(previewURL->url()));
  obj.setAspect((QDVD::VideoTrack::AspectRatio)aspectComboBox->currentItem());
  obj.setSubtitles(m_subtitles);
  obj.setAudioTracks(m_audioTracks);
  obj.setConversion(m_conversionParams);
}

void VideoOptions::audioPropertiesClicked()
{
  LanguageSelectionLayout dlg(this);
  dlg.languageListBox->setLanguage(audioListBox->language());
  if (dlg.exec())
  {
    QString lang = dlg.languageListBox->language();
    int n = audioListBox->index(audioListBox->selectedItem());
    m_audioTracks[n].setLanguage(lang);
    audioListBox->setItemLanguage(lang);
  }
}

void VideoOptions::subtitleAddClicked()
{
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
}

void VideoOptions::subtitleRemoveClicked()
{
  int n = subtitleListBox->index(subtitleListBox->selectedItem());

  m_subtitles.remove(m_subtitles.at(n));
  subtitleListBox->removeItem(n);
  enableButtons();
}

void VideoOptions::subtitlePropertiesClicked()
{
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
      previewURL->setURL(preview);
    updateTexts();
  }
}

void VideoOptions::conversionPropertiesClicked()
{
  Conversion dlg(this);
  dlg.setData(m_conversionParams);
  if (dlg.exec())
  {
    dlg.getData(m_conversionParams);
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
      i18n("Video has %1 chapter", "Video has %1 chapters", chapters)
      .arg(chapters));
  conversionLabel->setText(i18n("Video %1kb/s, Audio %2kb/s")
      .arg(m_conversionParams.m_videoBitrate)
      .arg(m_conversionParams.m_audioBitrate));
}

bool VideoOptions::isSelectedSubtitleInVideo()
{
  if(m_subtitles.count() > 0 && subtitleListBox->selectedItem())
  {
    int n = subtitleListBox->index(subtitleListBox->selectedItem());
    return m_subtitles[n].file().isEmpty();
  }
  return false;
}

#include "videooptions.moc"
