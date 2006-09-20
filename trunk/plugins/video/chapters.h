//**************************************************************************
//   Copyright (C) 2004, 2005 by Petri Damst�
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
#ifndef CHAPTERS_H
#define CHAPTERS_H

#include <kmf_stddef.h>
#include <ui_chapters.h>
#include <videooptions.h>
#include <kmftime.h>
#include <Q3ListView>

/**
@author Petri Damsten
*/
class CellListModel;

class Chapters : public KDialog, public Ui::Chapters
{
    Q_OBJECT
  public:
    Chapters(QWidget *parent = 0);
    ~Chapters();

    void getData(QDVD::CellList& cells, QString* preview) const;
    void setData(const QDVD::CellList& cells, const VideoObject* obj);

  protected slots:
    virtual void slotNextFrame();
    virtual void slotPrevFrame();
    virtual void slotForward();
    virtual void slotRewind();
    virtual void slotStart();
    virtual void slotEnd();
    virtual void slotRemove();
    virtual void slotAdd();
    virtual void slotSelectionChanged();
    virtual void slotSliderMoved(int value);
    virtual void slotContextMenu(Q3ListView*, Q3ListViewItem*, const QPoint& p);
    virtual void renameAll();
    virtual void deleteAll();
    virtual void autoChapters();
    virtual void import();
    virtual void saveCustomPreview();
    virtual void accept();

  private:
    const VideoObject* m_obj;
    KMF::Time m_pos;
    QString m_duration;
    QString m_preview;
    QDVD::CellList m_cells;
    CellListModel* m_model;

    int selectedItemIndex(Q3ListView* lv);
    void moveFrames(int direction);
    void updateVideo();
};

#endif
