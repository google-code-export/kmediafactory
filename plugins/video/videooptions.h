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
#ifndef VIDEOOPTIONS_H
#define VIDEOOPTIONS_H

#include <kmf_stddef.h>
#include "videooptionslayout.h"
#include "videoobject.h"
#include <qdict.h>

/**
*/
class VideoOptions : public VideoOptionsLayout
{
    Q_OBJECT
  public:
    VideoOptions(QWidget *parent = 0, const char *name = 0);
    ~VideoOptions();

    void getData(VideoObject& obj) const;
    void setData(const VideoObject& obj);

  protected slots:
    virtual void chaptersClicked();
    virtual void audioPropertiesClicked();
    virtual void subtitleAddClicked();
    virtual void subtitleRemoveClicked();
    virtual void subtitlePropertiesClicked();
    virtual void enableButtons();
    virtual void updateTexts();

  private:
    const VideoObject* m_obj;
    QDVD::CellList m_cells;
    QDVD::SubtitleList m_subtitles;
    QDVD::AudioList m_audioTracks;

    bool isSelectedSubtitleInVideo();
};

#endif
