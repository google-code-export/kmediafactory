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

#ifndef SIZEWIDGET_H
#define SIZEWIDGET_H

#include <kcapacitybar.h>

class SizeWidget : public QWidget
{
    Q_OBJECT

    public:
        SizeWidget(QWidget *parent = 0);
        virtual ~SizeWidget();

        virtual void paintEvent(QPaintEvent *ev);
        virtual QSize minimumSizeHint() const;

        public Q_SLOTS :
        void setSizes(quint64 max, quint64 size);

    private:
        void updateLabel(quint64 max, quint64 size);

        KCapacityBar m_bar;
};

#endif
