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

#ifndef TEMPLATEPAGE_H
#define TEMPLATEPAGE_H

#include <QtCore/QDateTime>

#include <ui_templatepage.h>
#include "kmfplugininterface.h"

class KPageWidgetItem;

class TemplatePage : public QWidget, public Ui::TemplatePage
{
    Q_OBJECT

    struct MenuIndex
    {
        MenuIndex()  { reset(); }
        MenuIndex(int p, int t, int c) : page(p), title(t), chapter(c) { }
        MenuIndex(int idx) : page(idx&0xFF), title((idx&0xFF00)>>8), chapter((idx&0xFF0000)>>16) { }
        int  toInt() const { return (page&0xFF)+((title&0xFF)<<8)+((chapter&0xFF)<<16); }
        void reset() { page=title=chapter=0; }
        bool operator==(MenuIndex &o) const { return page==o.page && title==o.title && chapter==o.chapter; }
        int page,
            title,
            chapter;
    };
    
    public:
        TemplatePage(QWidget *parent = 0);
        virtual ~TemplatePage();

    public slots:
        void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
        void currentPageChanged(KPageWidgetItem *, KPageWidgetItem *);
        void updatePreview(int n = -1);
        void contextMenuRequested(const QPoint &pos);
        void imageContextMenuRequested(const QPoint &pos);
        void projectInit();

    protected slots:
        virtual void previewClicked();

    private:
        MenuIndex m_menu;
        QDateTime m_lastUpdate;
};

#endif // TEMPLATEPAGE_H
