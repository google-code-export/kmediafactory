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

#include "kmfgeometry.h"

#include <QtCore/QRegExp>
#include <QtCore/QString>

#include <KDebug>

#include "kmfwidget.h"

QSize KMFUnit::m_maxSize = QSize(0, 0);

void KMFUnit::set(int value, Type type)
{
    m_value = value;
    m_type = type;
/*
 * kDebug() << m_geometry->owner()->name() << " S: " << m_edge <<  " S: " <<
 *             m_value;
 */
}

void KMFUnit::set(const QString &value)
{
    QString temp = value.toLower();

    QRegExp rx("[^0-9+\\-\\.%:min]");

    temp.remove(rx);

    if (temp == "") {
        m_type = Absolute;
        m_value = 0;
    }

    QStringList list = temp.split(':');
    temp = list[0];

    if (temp == "min") {
        m_type = Minimum;
        m_value = 0;
    } else if (temp.right(1) == "%")    {
        m_type = Percentage;
        m_value = (int)(temp.left(temp.length() - 1).toFloat() * 100);
    } else if (temp.left(1) == "+" || temp.left(1) == "-")    {
        m_type = Relative;
        m_value = temp.toInt();
    } else   {
        m_type = Absolute;
        m_value = temp.toInt();
    }

    if (list.count() > 1) {
        m_margin = list[1].toInt();
    }

    /*
     * kDebug() << m_geometry->owner()->name() << " s: " << m_edge <<  " s: " <<
     *           m_value;
     */
}

int KMFUnit::absoluteValue() const
{
    int total = 0;

    if (m_save == -1) {
        switch (m_type) {
            case Absolute:
                m_save = m_value;
                break;

            case Relative:
                m_save = m_geometry->parentOffset(m_edge) + m_value;
                break;

            case Percentage:
                total = m_geometry->totalPercentage(m_edge);
                m_save = m_geometry->parentSize(m_edge) * m_value / total;
                break;

            case Minimum:
                m_save = m_geometry->minimumSize(m_edge);
                break;
        }

        // int tmp = m_save;
        switch (m_edge) {
            case KMFUnit::Top:
            case KMFUnit::Height:
                m_save = qMin(qMax(m_save, 0), m_maxSize.height());
                break;

            case KMFUnit::Left:
            case KMFUnit::Width:
                m_save = qMin(qMax(m_save, 0), m_maxSize.width());
                break;
        }

        /*
         * if(tmp != m_save)
         * kDebug() << m_geometry->owner()->name() << " : " << tmp <<  " -> " <<
         *    m_save << " : " << m_maxSize << ", " << m_edge;
         */
    }

    /*
     * kDebug() << m_geometry->owner()->name() << " a: " << m_edge <<  " a: " <<
     *           m_save;
     */
    return m_save;
}

KMFGeometry::KMFGeometry(KMFWidget *owner)
    : m_owner(owner)
    , m_parent(0)
    , m_left(this, KMFUnit::Left, KMFUnit::Relative)
    , m_top(this, KMFUnit::Top, KMFUnit::Relative)
    , m_width(this, KMFUnit::Width, KMFUnit::Minimum)
    , m_height(this, KMFUnit::Height, KMFUnit::Minimum)
{
    if ((owner->parent() != 0) && owner->parent()->inherits("KMFWidget")) {
        m_parent = static_cast<KMFWidget *>(owner->parent());
    }
}

int KMFGeometry::parentSize(KMFUnit::Edge edge)
{
    if (m_parent) {
        switch (edge) {
            case KMFUnit::Left:
            case KMFUnit::Width:
                return m_parent->childWidth();

            case KMFUnit::Top:
            case KMFUnit::Height:
                return m_parent->childHeight();
        }
    }

    return 0;
}

int KMFGeometry::parentOffset(KMFUnit::Edge edge)
{
    if (m_parent) {
        switch (edge) {
            case KMFUnit::Left:
                return m_parent->childX(m_owner);

            case KMFUnit::Width:
                return m_parent->paintWidth();

            case KMFUnit::Top:
                return m_parent->childY(m_owner);

            case KMFUnit::Height:
                return m_parent->paintHeight();
        }
    }

    return 0;
}

int KMFGeometry::minimumSize(KMFUnit::Edge edge)
{
    if (m_parent) {
        switch (edge) {
            case KMFUnit::Left:
                return 0;

            case KMFUnit::Width:
                return m_owner->minimumWidth();

            case KMFUnit::Top:
                return 0;

            case KMFUnit::Height:
                return m_owner->minimumHeight();
        }
    }

    return 0;
}

int KMFGeometry::totalPercentage(KMFUnit::Edge edge)
{
    if (m_parent) {
        switch (edge) {
            case KMFUnit::Left:
                return 10000;

            case KMFUnit::Width:
                return m_parent->childWidthPercentage();

            case KMFUnit::Top:
                return 10000;

            case KMFUnit::Height:
                return m_parent->childHeightPercentage();
        }
    }

    return 0;
}

QRect KMFGeometry::paintRect() const
{
    return QRect(x(), y(), w(), h());
}

QRect KMFGeometry::rect() const
{
    return QRect(m_left.absoluteValue(), m_top.absoluteValue(),
            m_width.absoluteValue(), m_height.absoluteValue());
}

int KMFGeometry::x() const
{
    return m_left.absoluteValue() + m_left.margin();
}

int KMFGeometry::y() const
{
    return m_top.absoluteValue() + m_top.margin();
}

int KMFGeometry::w() const
{
    return m_width.absoluteValue() - m_left.margin() - m_width.margin();
}

int KMFGeometry::h() const
{
    return m_height.absoluteValue() - m_top.margin() - m_height.margin();
}

void KMFGeometry::setMargin(int margin)
{
    m_left.setMargin(margin);
    m_top.setMargin(margin);
    m_width.setMargin(margin);
    m_height.setMargin(margin);
}

void KMFGeometry::setMargin(QString margin)
{
    QStringList marginList = margin.split(QRegExp("[|, ]"));

    if (marginList.count() == 1) {
        setMargin(marginList[0].toInt());
    } else if (marginList.count() == 2)    {
        m_top.setMargin(marginList[0].toInt());
        m_width.setMargin(marginList[1].toInt());
        m_height.setMargin(marginList[0].toInt());
        m_left.setMargin(marginList[1].toInt());
    } else if (marginList.count() == 4)    {
        m_top.setMargin(marginList[0].toInt());
        m_width.setMargin(marginList[1].toInt());
        m_height.setMargin(marginList[2].toInt());
        m_left.setMargin(marginList[3].toInt());
    }

    /*
     * kDebug()
     *  << m_top.margin() << ", "
     *  << m_width.margin() << ", "
     *  << m_height.margin() << ", "
     *  << m_left.margin();
     */
}
