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
#include "videooptions.h"
#include "chapters.h"
#include "ui_languageselection.h"
#include "kmflanguagewidgets.h"
#include "videopluginsettings.h"
#include "subtitleoptions.h"
#include "kmftools.h"
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
      setCaption(i18n("Select Language"));
      m_languages.useAllLanguages();
      languageListBox->setModel(&m_languages);
    };
    QString language()
    {
      return m_languages.at(languageListBox->currentIndex().row());
    };
    void setLanguage(const QString& lang)
    {
      languageListBox->setCurrentIndex(m_languages.index(lang));
    };

  private:
    LanguageListModel m_languages;
};

VideoOptions::VideoOptions(QWidget *parent)
  : KDialog(parent)
{
  setupUi(mainWidget());
  setButtons(KDialog::Ok | KDialog::Cancel);
  setCaption(i18n("Video Options"));
  connect(subtitleAddButton, SIGNAL(clicked()),
          this, SLOT(subtitleAddClicked()));
  connect(subtitlePropertiesButton, SIGNAL(clicked()),
          this, SLOT(subtitlePropertiesClicked()));
  connect(subtitleRemoveButton, SIGNAL(clicked()),
          this, SLOT(subtitleRemoveClicked()));
  connect(audioPropertiesButton, SIGNAL(clicked()),
          this, SLOT(audioPropertiesClicked()));
  connect(chapterPropertiesButton, SIGNAL(clicked()),
          this, SLOT(chaptersClicked()));
}

VideoOptions::~VideoOptions()
{
}

void VideoOptions::setData(const VideoObject& obj)
{
  titleEdit->setText(obj.title());
  previewUrl->setUrl(obj.previewUrl().prettyUrl());
  aspectComboBox->setCurrentIndex((int)obj.aspect());

  m_cells = obj.cellList();
  m_obj = &obj;
  m_audioTracks = obj.audioTracks();
  m_audioModel.setLanguages(&m_audioTracks);
  audioListBox->setModel(&m_audioModel);

  m_subtitles = obj.subtitles();
  m_subtitleModel.setLanguages(&m_subtitles);
  subtitleListBox->setModel(&m_subtitleModel);
  subtitleListBox->setCurrentIndex(m_subtitleModel.index(0));

  connect(audioListBox->selectionModel(),
      SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
      this, SLOT(enableButtons()));
  connect(subtitleListBox->selectionModel(),
      SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
      this, SLOT(enableButtons()));
  enableButtons();
  updateTexts();
}

void VideoOptions::getData(VideoObject& obj) const
{
  obj.setTitle(titleEdit->text());
  obj.setCellList(m_cells);
  obj.setPreviewUrl(KUrl(previewUrl->url()));
  obj.setAspect((QDVD::VideoTrack::AspectRatio)aspectComboBox->currentIndex());
  obj.setSubtitles(m_subtitles);
  obj.setAudioTracks(m_audioTracks);
}

void VideoOptions::audioPropertiesClicked()
{
  LanguageSelection dlg(this);
  int n = audioListBox->selectionModel()->selectedIndexes().first().row();
  dlg.setLanguage(m_audioTracks[n].language());
  if (dlg.exec())
  {
    QString newLanguage = dlg.language();
    m_audioTracks[n].setLanguage(newLanguage);
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
    m_subtitles.append(subtitle);
  }
  m_subtitleModel.setLanguages(&m_subtitles);
  enableButtons();
}

void VideoOptions::subtitleRemoveClicked()
{
  int n = subtitleListBox->currentIndex().row();

  m_subtitles.removeAt(n);
  m_subtitleModel.setLanguages(&m_subtitles);
  enableButtons();
}

void VideoOptions::subtitlePropertiesClicked()
{
  int n = subtitleListBox->currentIndex().row();

  if(isSelectedSubtitleInVideo())
  {
    LanguageSelection dlg(this);
    int n = subtitleListBox->currentIndex().row();
    dlg.setLanguage(m_subtitles[n].language());
    if (dlg.exec())
    {
      QString lang = dlg.language();
      m_subtitles[n].setLanguage(lang);
    }
  }
  else
  {
    SubtitleOptions dlg(this);
    dlg.setData(m_subtitles[n]);
    if (dlg.exec())
    {
      dlg.getData(m_subtitles[n]);
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
      previewUrl->setUrl(preview);
    updateTexts();
  }
}

void VideoOptions::enableButtons()
{
  int a = audioListBox->selectionModel()->selectedIndexes().count();
  int s = subtitleListBox->selectionModel()->selectedIndexes().count();
  audioPropertiesButton->setEnabled(m_audioTracks.count() > 0 && a > 0);
  subtitleRemoveButton->setEnabled(m_subtitles.count() > 0 && s > 0 &&
                                   !isSelectedSubtitleInVideo());
  subtitlePropertiesButton->setEnabled(m_subtitles.count() > 0 && s > 0);
}

void VideoOptions::updateTexts()
{
  int chapters = m_cells.count();
  chapterLabel->setText(
      i18np("Video has 1 chapter", "Video has %1 chapters", chapters));
}

bool VideoOptions::isSelectedSubtitleInVideo()
{
  if(m_subtitles.count() > 0)
  {
    int n = subtitleListBox->currentIndex().row();
    if(n >= 0 && n < m_subtitles.count())
      return m_subtitles[n].file().isEmpty();
  }
  return false;
}

#include "videooptions.moc"
