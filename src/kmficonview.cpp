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
#include "kmficonview.h"
#include <QPainter>

#define MARGIN 3

void KMFItemDelegate::paint(QPainter* painter,
                            const QStyleOptionViewItem& option,
                            const QModelIndex& index) const
{
  painter->save();
  if(option.state & QStyle::State_Selected)
  {
    QRect rc = option.rect;

    painter->setBrush(option.palette.brush(QPalette::Highlight));
    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawRoundRect(rc, 30, 30);
  }
  QStyleOptionViewItem op = option;
  op.state = QStyle::State_Enabled;
  op.state = QStyle::State_Selected;
  painter->translate(MARGIN, MARGIN);
  QItemDelegate::paint(painter, op, index);
  /*
  // This is from qitemdelegate.cpp GPL-2
    QStyleOptionViewItemV2 opt = setOptions(index, option);
    const QStyleOptionViewItemV2 *v2 = qstyleoption_cast<const QStyleOptionViewItemV2 *>(&option);
    opt.features = v2 ? v2->features : QStyleOptionViewItemV2::ViewItemFeatures(QStyleOptionViewItemV2::None);

    // prepare
    painter->save();
    if (d->clipPainting)
        painter->setClipRect(opt.rect);

    // get the data and the rectangles

    QVariant value;

    QIcon icon;
    QIcon::Mode iconMode = d->iconMode(option.state);
    QIcon::State iconState = d->iconState(option.state);

    QPixmap pixmap;
    QRect decorationRect;
    value = index.data(Qt::DecorationRole);
    if (value.isValid()) {
        if (value.type() == QVariant::Icon) {
            icon = qvariant_cast<QIcon>(value);
            decorationRect = QRect(QPoint(0, 0),
                                   icon.actualSize(option.decorationSize, iconMode, iconState));
        } else {
            pixmap = decoration(opt, value);
            decorationRect = QRect(QPoint(0, 0), pixmap.size());
        }
    }

    QString text;
    QRect displayRect;
    value = index.data(Qt::DisplayRole);
    if (value.isValid()) {
        if (value.type() == QVariant::Double)
            text = QLocale().toString(value.toDouble());
        else
            text = QItemDelegatePrivate::replaceNewLine(value.toString());

        displayRect = textRectangle(painter, d->textLayoutBounds(opt), opt.font, text);
    }

    QRect checkRect;
    Qt::CheckState checkState = Qt::Unchecked;
    value = index.data(Qt::CheckStateRole);
    if (value.isValid()) {
        checkState = static_cast<Qt::CheckState>(value.toInt());
        checkRect = check(opt, opt.rect, value);
    }

    // do the layout

    doLayout(opt, &checkRect, &decorationRect, &displayRect, false);

    // draw the item

    drawBackground(painter, opt, index);
    drawCheck(painter, opt, checkRect, checkState);
    if (!icon.isNull())
        icon.paint(painter, decorationRect, option.decorationAlignment, iconMode, iconState);
    else
        drawDecoration(painter, opt, decorationRect, pixmap);
    drawDisplay(painter, opt, displayRect, text);
    drawFocus(painter, opt, text.isEmpty() ? QRect() : displayRect);

    // done
    painter->restore();
  */
  painter->restore();
}

QSize KMFItemDelegate::sizeHint(const QStyleOptionViewItem& option,
                                const QModelIndex& index) const
{
  if(!index.isValid())
    return QSize(0, 0);
  QSize res = QItemDelegate::sizeHint(option, index);
  return res + QSize(2*MARGIN, 2*MARGIN);
}

#include "kmficonview.moc"
