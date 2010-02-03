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
#ifndef KMFLABEL_H
#define KMFLABEL_H


#include <QtGui/QFont>

#include "kmfwidget.h"

/**
@author Petri Damsten
*/
class KMFLabel : public KMFWidget
{
    Q_OBJECT
  public:
    KMFLabel(QObject *parent = 0);
    ~KMFLabel();

    const QString& text() const { return m_text; };
    void setText(const QString& text);
    const QFont& font() const { return m_font; };
    void setFont(const QFont& font) { m_font = font; };
    virtual void fromXML(const QDomElement& element);
    virtual int minimumPaintWidth() const;
    virtual int minimumPaintHeight() const;
    virtual void setProperty(const QString& name, QVariant value);

  protected:
    virtual void paintWidget(QImage* layer, bool shdw = false) const;
    QString fitText(QString txt, int width) const;

  private:
    QString m_text;
    QFont m_font;
};

#endif

