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
#include <QLocale>
#include <QPainterPath>
#include <limits.h>

#define MARGIN 6

// From Qt4 examples painting/painterpaths/window.cpp GPL-2
void KMFItemDelegate::drawRoundRect(QPainter* painter, const QRect& rect,
                                    int radius) const
{
  int dr = radius * 2;
  QPainterPath roundRectPath;

  roundRectPath.moveTo(rect.right(), rect.top() - radius);
  roundRectPath.arcTo(rect.right() - dr, rect.top(), dr, dr, 0.0, 90.0);
  roundRectPath.lineTo(rect.left() + radius, rect.top());
  roundRectPath.arcTo(rect.left(), rect.top(), dr, dr, 90.0, 90.0);
  roundRectPath.lineTo(rect.left(), rect.bottom() - radius);
  roundRectPath.arcTo(rect.left(), rect.bottom() - dr, dr, dr, 180.0, 90.0);
  roundRectPath.lineTo(rect.right() - radius, rect.bottom());
  roundRectPath.arcTo(rect.right() - dr, rect.bottom() -dr, dr, dr,
                      270.0, 90.0);
  roundRectPath.closeSubpath();

  painter->drawPath(roundRectPath);
}

// From qt4 qitemdelegate.cpp GPL-2
QString KMFItemDelegate::replaceNewLine(QString text) const
{
  const QChar nl = QLatin1Char('\n');
  for (int i = 0; i < text.count(); ++i)
    if (text.at(i) == nl)
      text[i] = QChar::LineSeparator;
  return text;
}

// From qt4 qitemdelegate.cpp GPL-2
QRect KMFItemDelegate::textLayoutBounds(
    const QStyleOptionViewItemV2 &option) const
{
  QRect rect = option.rect;
  const bool wrapText = option.features & QStyleOptionViewItemV2::WrapText;
  switch (option.decorationPosition)
  {
    case QStyleOptionViewItem::Left:
    case QStyleOptionViewItem::Right:
        rect.setWidth(INT_MAX >> 6);
        break;
    case QStyleOptionViewItem::Top:
    case QStyleOptionViewItem::Bottom:
        rect.setWidth(wrapText ? option.decorationSize.width() :
            (INT_MAX >> 6));
        break;
  }
  return rect;
}

// This function is mainly from qitemdelegate.cpp GPL-2
void KMFItemDelegate::paint(QPainter* painter,
                            const QStyleOptionViewItem& option,
                            const QModelIndex& index) const
{
  painter->save();

  QStyleOptionViewItemV2 opt = setOptions(index, option);
  const QStyleOptionViewItemV2 *v2 =
      qstyleoption_cast<const QStyleOptionViewItemV2 *>(&option);
  opt.features = v2 ? v2->features :
      QStyleOptionViewItemV2::ViewItemFeatures(QStyleOptionViewItemV2::None);
  drawBackground(painter, opt, index);

  if(option.state & QStyle::State_Selected)
  {
    painter->setBrush(option.palette.brush(QPalette::Highlight));
    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::Antialiasing);
    drawRoundRect(painter, opt.rect, 10);
  }
  opt.rect.adjust(MARGIN, MARGIN, -1 * MARGIN, -1 * MARGIN);

  QVariant value;

  QIcon icon;
  QPixmap pixmap;
  QRect decorationRect;
  value = index.data(Qt::DecorationRole);
  if (value.isValid())
  {
    if (value.type() == QVariant::Icon)
    {
      icon = qvariant_cast<QIcon>(value);
      decorationRect = QRect(QPoint(0, 0),
                              icon.actualSize(option.decorationSize,
                                              QIcon::Normal, QIcon::Off));
    }
    else
    {
      pixmap = decoration(opt, value);
      decorationRect = QRect(QPoint(0, 0), pixmap.size());
    }
  }

  QString text;
  QRect displayRect;
  value = index.data(Qt::DisplayRole);
  if (value.isValid())
  {
    if (value.type() == QVariant::Double)
      text = QLocale().toString(value.toDouble());
    else
      text = replaceNewLine(value.toString());

    displayRect = textRectangle(painter, textLayoutBounds(opt),
                                opt.font, text);
  }

  QRect checkRect;
  // do the layout
  doLayout(opt, &checkRect, &decorationRect, &displayRect, false);

  // draw the item
  if (!icon.isNull())
  {
    icon.paint(painter, decorationRect, option.decorationAlignment,
               QIcon::Normal, QIcon::Off);
  }
  else
  {
    // No grayed icon when selected
    QStyle::State save = opt.state;
    opt.state = QStyle::State_Enabled;
    drawDecoration(painter, opt, decorationRect, pixmap);
    opt.state = save;
  }
  drawDisplay(painter, opt, displayRect, text);

  // done
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
