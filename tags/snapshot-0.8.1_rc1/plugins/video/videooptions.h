// **************************************************************************
//   Copyright (C) 2004-2008 by Petri Damsten
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

#ifndef VIDEOOPTIONS_H
#define VIDEOOPTIONS_H

#include <kmf_stddef.h>
#include <kmflanguagewidgets.h>
#include <ui_videooptions.h>
#include "videoobject.h"

class Chapters;

class VideoOptions : public KDialog, public Ui::VideoOptions
{
    Q_OBJECT

    public:
        VideoOptions(QWidget *parent = 0);
        ~VideoOptions();

        void getData(VideoObject &obj) const;
        void setData(const VideoObject &obj);
        void setPreviewUrl(const QString& url);

    protected slots:
        virtual void accept();
        virtual void audioPropertiesClicked();
        virtual void subtitleAddClicked();
        virtual void subtitleRemoveClicked();
        virtual void subtitlePropertiesClicked();
        virtual void enableButtons();

    private:
        const VideoObject *m_obj;
        Chapters *m_chapters;
        QDVD::SubtitleList m_subtitles;
        LanguageListModel m_subtitleModel;
        QDVD::AudioList m_audioTracks;
        LanguageListModel m_audioModel;

        bool isSelectedSubtitleInVideo();
};

#endif
