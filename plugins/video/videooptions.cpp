// **************************************************************************
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
// **************************************************************************

#include "videooptions.h"

#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

#include <KApplication>
#include <KDebug>
#include <KLocale>
#include <KUrlRequester>
#include <KFileDialog>

#include <kmflanguagewidgets.h>
#include <kmftools.h>
#include <qdvdinfo.h>
#include <ui_languageselection.h>
#include <videopluginsettings.h>
#include "chapters.h"
#include "subtitleoptions.h"

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
        void setLanguage(const QString &lang)
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
    m_chapters = new Chapters(this);
    tabWidget->setCurrentIndex(tabWidget->insertTab(0, m_chapters, i18n("Chapters")));

    setButtons(KDialog::Ok | KDialog::Cancel);
    setCaption(i18n("Video Properties"));
    connect(subtitleAddButton, SIGNAL(clicked()),
            this, SLOT(subtitleAddClicked()));
    connect(subtitlePropertiesButton, SIGNAL(clicked()),
            this, SLOT(subtitlePropertiesClicked()));
    connect(subtitleRemoveButton, SIGNAL(clicked()),
            this, SLOT(subtitleRemoveClicked()));
    connect(audioPropertiesButton, SIGNAL(clicked()),
            this, SLOT(audioPropertiesClicked()));
    previewUrl->setFilter("image/jpeg image/png");
    previewUrl->fileDialog()->setCaption(i18n("Select Preview File"));
    restoreDialogSize(KConfigGroup(KGlobal::config(), metaObject()->className()));
}

VideoOptions::~VideoOptions()
{
    KConfigGroup cg(KGlobal::config(), metaObject()->className());
    KDialog::saveDialogSize(cg);
}

void VideoOptions::accept()
{
    if (m_chapters->ok()) {
        KDialog::accept();
    }

//     else
//         KDialog::reject();
}

void VideoOptions::setData(const VideoObject &obj)
{
    titleEdit->setText(obj.title());
    previewUrl->setUrl(obj.previewUrl().prettyUrl());
    aspectComboBox->setCurrentIndex((int)obj.aspect());

    m_obj = &obj;
    m_chapters->setData(obj.cellList(), m_obj, this);
    m_audioTracks = obj.audioTracks();
    m_audioModel.setLanguages(&m_audioTracks);
    audioListBox->setModel(&m_audioModel);

    m_subtitles = obj.subtitles();
    m_subtitleModel.setLanguages(&m_subtitles);
    subtitleListBox->setModel(&m_subtitleModel);
    subtitleListBox->setCurrentIndex(m_subtitleModel.index(0));

    connect(audioListBox->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(enableButtons()));
    connect(subtitleListBox->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(enableButtons()));
    enableButtons();
}

void VideoOptions::setPreviewUrl(const QString& url)
{
    previewUrl->setUrl(url);
}

void VideoOptions::getData(VideoObject &obj) const
{
    obj.setTitle(titleEdit->text());
    obj.setCellList(m_chapters->cells());
    obj.setPreviewUrl(KUrl(previewUrl->url()));
    obj.setAspect((QDVD::VideoTrack::AspectRatio)aspectComboBox->currentIndex());
    obj.setSubtitles(m_subtitles);
    obj.setAudioTracks(m_audioTracks);
}

void VideoOptions::audioPropertiesClicked()
{
    QPointer<LanguageSelection> dlg = new LanguageSelection(this);
    int n = audioListBox->selectionModel()->selectedIndexes().first().row();

    dlg->setLanguage(m_audioTracks[n].language());

    if (dlg->exec()) {
        QString newLanguage = dlg->language();
        m_audioTracks[n].setLanguage(newLanguage);
    }
    delete dlg;
}

void VideoOptions::subtitleAddClicked()
{
    QDVD::Subtitle subtitle(VideoPluginSettings::defaultSubtitleLanguage());
    QPointer<SubtitleOptions> dlg = new SubtitleOptions(this);

    dlg->setData(subtitle);

    if (dlg->exec()) {
        dlg->getData(subtitle);
        m_subtitles.append(subtitle);
    }

    m_subtitleModel.setLanguages(&m_subtitles);
    enableButtons();
    delete dlg;
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

    if (isSelectedSubtitleInVideo()) {
        QPointer<LanguageSelection> dlg = new LanguageSelection(this);
        int n = subtitleListBox->currentIndex().row();
        dlg->setLanguage(m_subtitles[n].language());

        if (dlg->exec()) {
            QString lang = dlg->language();
            m_subtitles[n].setLanguage(lang);
        }
        delete dlg;
    } else   {
        QPointer<SubtitleOptions> dlg = new SubtitleOptions(this);
        dlg->setData(m_subtitles[n]);

        if (dlg->exec()) {
            dlg->getData(m_subtitles[n]);
        }
        delete dlg;
    }
}

// TODO update preview!!!
// void VideoOptions::chaptersClicked()
// {
//   Chapters dlg(this);
//   dlg.setData(m_cells, m_obj);
//   if (dlg.exec())
//   {
//     QString preview;
//     dlg.getData(m_cells, &preview);
//     if(!preview.isEmpty())
//       previewUrl->setUrl(preview);
//     updateTexts();
//   }
// }

void VideoOptions::enableButtons()
{
    int a = audioListBox->selectionModel()->selectedIndexes().count();
    int s = subtitleListBox->selectionModel()->selectedIndexes().count();

    audioPropertiesButton->setEnabled(m_audioTracks.count() > 0 && a > 0);
    subtitleRemoveButton->setEnabled(m_subtitles.count() > 0 && s > 0 &&
            !isSelectedSubtitleInVideo());
    subtitlePropertiesButton->setEnabled(m_subtitles.count() > 0 && s > 0);
}

bool VideoOptions::isSelectedSubtitleInVideo()
{
    if (m_subtitles.count() > 0) {
        int n = subtitleListBox->currentIndex().row();

        if ((n >= 0) && (n < m_subtitles.count())) {
            return m_subtitles[n].file().isEmpty();
        }
    }

    return false;
}

#include "videooptions.moc"
