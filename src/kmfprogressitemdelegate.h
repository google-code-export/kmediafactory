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

#ifndef KMFPROGRESSITEMDELEGATE_H
#define KMFPROGRESSITEMDELEGATE_H

#include <QtGui/QStyledItemDelegate>
#include <QtGui/QPixmap>

class KMFProgressItemDelegate : public QStyledItemDelegate
{
    public:
        Q_ENUMS(ProgressRoles)
        enum ProgressRoles { ValueRole = Qt::UserRole, MaxRole, LogRole, ResultRole, ColorRole};

        virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
            const QModelIndex &index) const;
            
    private:
        mutable QPixmap pixmap;
};

#endif // KMFPROGRESSITEMDELEGATE_H
