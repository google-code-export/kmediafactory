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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//**************************************************************************

#ifndef KMFGEOMETRY_H
#define KMFGEOMETRY_H


#include <QtCore/QRect>


class QString;
class KMFWidget;
class KMFGeometry;

class KMFUnit
{
  friend class KMFGeometry;

  public:
    enum Type { Absolute = 0, Relative, Percentage, Minimum };
    enum Edge { Left = 0, Top, Width, Height };

    KMFUnit(KMFGeometry* geometry, Edge edge, Type type) : 
        m_save(-1),m_value(0), m_type(type), m_margin(0),
        m_geometry(geometry), m_edge(edge) {};

    int absoluteValue() const;
    static void setMaxRes(QSize size) { m_maxSize = size; };
    void set(const QString& value);
    void set(int value, Type type);
    int value() const { return m_value; };
    void setValue(int value) { m_value = value; };
    int margin() const { return m_margin; };
    void setMargin(int margin) { m_margin = margin; };
    Type type() const { return m_type; };
    void setType(Type type) { m_type = type; };

  protected:
    int saved() const { return m_save; };

  private:
    mutable int m_save;
    int m_value;
    Type m_type;
    int m_margin;
    KMFGeometry* m_geometry;
    Edge m_edge;
    static QSize m_maxSize;
};

class KMFGeometry
{
  friend class KMFUnit;

  public:
    KMFGeometry(KMFWidget* owner);

    QRect rect() const;
    QRect paintRect() const;
    KMFUnit& left() { return m_left; };
    KMFUnit& top() { return m_top; };
    KMFUnit& width() { return m_width; };
    KMFUnit& height() { return m_height; };
    const KMFUnit& left() const { return m_left; };
    const KMFUnit& top() const { return m_top; };
    const KMFUnit& width() const { return m_width; };
    const KMFUnit& height() const { return m_height; };
    int x() const;
    int y() const;
    int w() const;
    int h() const;
    void setMargin(int margin);
    void setMargin(QString margin);

  protected:
    int parentSize(KMFUnit::Edge edge);
    int parentOffset(KMFUnit::Edge edge);
    int totalPercentage(KMFUnit::Edge edge);
    int minimumSize(KMFUnit::Edge edge);
    KMFWidget* owner() { return m_owner; };

  private:
    KMFWidget* m_owner;
    KMFWidget* m_parent;
    KMFUnit m_left;
    KMFUnit m_top;
    KMFUnit m_width;
    KMFUnit m_height;
};

#endif

