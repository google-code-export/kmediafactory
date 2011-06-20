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

#include "kmfprogressitemdelegate.h"

#include <QtCore/QModelIndex>
#include <QtGui/QPainter>
#include <QtGui/QStyle>
#include <QtGui/QApplication>
#include <KDE/KLocale>

#define BARW 100

void KMFProgressItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    int value = index.data(ValueRole).toInt();
    int max = index.data(MaxRole).toInt();
    QRect rc = option.rect;
    QStyleOptionViewItemV2 v2 = option;

    painter->save();
    v2.features &= ~QStyleOptionViewItemV2::WrapText;
    QStyledItemDelegate::paint(painter, v2, index);

    if (value < max) {
        if(pixmap.isNull() || pixmap.width()!=BARW || pixmap.height()!=rc.height()) {
            pixmap=QPixmap(BARW, rc.height());
        }
        
        pixmap.fill(Qt::transparent);
        
        QPainter pixPainter(&pixmap);
        QStyleOptionProgressBar opt;
        opt.minimum=0;
        opt.maximum=max;
        opt.progress=value;
        opt.textVisible=true;
        opt.text=i18n("%1%", max!=0 ? (int)(((value*100.0)/max)+0.5) : 0);
        opt.rect=QRect(0, 0, pixmap.width(), pixmap.height());
        opt.state=QStyle::State_Enabled;
        opt.palette=QApplication::palette();
        opt.direction=QApplication::layoutDirection();
        opt.fontMetrics=QApplication::fontMetrics();
        
        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &opt, &pixPainter, 0L);
        pixPainter.end();
        painter->drawPixmap(rc.width() - pixmap.width(), rc.y(), pixmap);
    }

    painter->restore();
}
