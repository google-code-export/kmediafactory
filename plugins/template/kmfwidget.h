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
#ifndef KMFWIDGET_H
#define KMFWIDGET_H

#include "kmfgeometry.h"
#include "kmftemplatebase.h"
#include "rect.h"
#include <QObject>
#include <QColor>

class QImage;
class QColor;
class QDomElement;
class KMFMenuPage;

class KMFShadow
{
  public:
    enum Type { None = 0, Hard, Blur };

    KMFShadow() :
      m_offset(0,0), m_color(qRgba(0, 0, 0, 128)), m_type(None),
      m_radius(10.0), m_sigma(5.0) {};

    const QColor& color() const { return m_color; };
    void setColor(const QColor& color) { m_color = color; };
    const QPoint& offset() const { return m_offset; };
    void setOffset(const QPoint& offset) { m_offset = offset; };
    const Type& type() const { return m_type; };
    void setType(const Type& type) { m_type = type; };
    const double& sigma() const { return m_sigma; };
    void setSigma(const double& sigma) { m_sigma = sigma; };
    const double& radius() const { return m_radius; };
    void setRadius(const double& radius) { m_radius = radius; };

    void toXML(QDomElement& element) const;
    void fromXML(const QDomElement& element);

  private:
    QPoint m_offset;
    QColor m_color;
    Type m_type;
    double m_radius;
    double m_sigma;
};

/**
*/
class KMFWidget : public KMFTemplateBase
{
    Q_OBJECT
  public:
    enum Layer { None = 0, Background = 1, Sub = 2, Highlight = 4,
                 Select = 8, Temp = 16 };

    KMFWidget(QObject *parent = 0);
    ~KMFWidget();

    int x() const { return m_geometry.left().absoluteValue(); };
    int y() const { return m_geometry.top().absoluteValue(); };
    virtual int paintX() const { return m_geometry.x(); };
    virtual int paintY() const { return m_geometry.y(); };
    virtual int childX(const KMFWidget*) const { return paintX(); };
    virtual int childY(const KMFWidget*) const { return paintY(); };
    int width() const  { return m_geometry.width().absoluteValue(); };
    int height() const  { return m_geometry.height().absoluteValue(); };
    virtual int paintWidth() const { return m_geometry.w(); };
    virtual int paintHeight() const { return m_geometry.h(); };
    virtual int childWidth() const  { return paintWidth(); };
    virtual int childHeight() const  { return paintHeight(); };
    virtual int childWidthPercentage() const { return 10000; };
    virtual int childHeightPercentage() const { return 10000; };
    QRect rect() const { return m_geometry.rect(); };
    virtual QRect paintRect(const QPoint offset = QPoint(0,0)) const;
    virtual int minimumPaintWidth() const;
    virtual int minimumPaintHeight() const;
    int minimumWidth() const;
    int minimumHeight() const;
    KMFGeometry& geometry() { return m_geometry; };
    const KMFGeometry& geometry() const { return m_geometry; };
    virtual void setProperty(const QString& name, QVariant value);

    void paint(KMFMenuPage* page);

    Layer layer() const { return m_layer; };
    virtual void setLayer(const Layer layer);
    const KMFShadow& shadow() const { return m_shadow; };
    virtual void setShadow(const KMFShadow& shadow);
    const KMF::Rect::VAlign& valign() const { return m_valign; };
    void setValign(const KMF::Rect::VAlign& valign) { m_valign = valign; };
    const KMF::Rect::HAlign& halign() const { return m_halign; };
    void setHalign(const KMF::Rect::HAlign& halign) { m_halign = halign; };
    virtual bool isHidden() const { return m_hidden; };
    bool takeSpace() const { return m_takeSpace; };
    int row() const { return m_row; };
    int column() const { return m_column; };
    void hide() { m_hidden = true; };
    void show() { m_hidden = false; };
    const QColor& color() const { return m_color; };
    void setColor(const QColor& color) { m_color = color; };
    void setColor(const QString& s);

    virtual void fromXML(const QDomElement& element);

  protected:
    virtual void paintWidget(QImage&, bool = false) { };
    int parse(const QString& coordinate,
	      KMFUnit::Type& type);
    void parseTitleChapter(const QString& s, int& title, int& chapter);

  private:
    KMFGeometry m_geometry;
    Layer m_layer;
    KMFShadow m_shadow;
    KMF::Rect::VAlign m_valign;
    KMF::Rect::HAlign m_halign;
    bool m_hidden;
    QColor m_color;
    bool m_takeSpace;
    int m_row;
    int m_column;

    int toInt(const QString& s, int offset);
};

#endif
