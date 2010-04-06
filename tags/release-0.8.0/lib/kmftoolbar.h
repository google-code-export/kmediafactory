// **************************************************************************
//   Copyright (C) 2007 by Petri Damsten
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

#ifndef KMFTOOLBAR_H
#define KMFTOOLBAR_H

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtGui/QHBoxLayout>
#include <QtGui/QScrollArea>

#include <kdemacros.h>

class QToolButton;

class KDE_EXPORT KMFToolBar : public QScrollArea
{
    Q_OBJECT

    public:
        KMFToolBar(QWidget * parent = 0);
        ~KMFToolBar();

    public slots:
        void add(QAction *action, const QString &group = "");
        void addActions(QList<QAction *> actions, const QString &group = "");
        void removeActions(const QString &group);

    protected slots:
        void lateInit();

    protected:
        virtual void resizeEvent(QResizeEvent *event);

    private:
        QWidget m_grid;
        QHBoxLayout m_layout;
        QMap<QString, QList<QToolButton *> > m_actions;
};

#endif
