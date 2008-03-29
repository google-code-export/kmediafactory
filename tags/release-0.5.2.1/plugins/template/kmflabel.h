//**************************************************************************
//   Copyright (C) 2004 by Petri Damstén
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
#ifndef KMFLABEL_H
#define KMFLABEL_H

#include "font.h"
#include "kmfwidget.h"

/**
@author Petri Damsten
*/
class KMFLabel : public KMFWidget
{
    Q_OBJECT
  public:
    KMFLabel(QObject *parent = 0, const char *name = 0);
    ~KMFLabel();

    const QString& text() const { return m_text; };
    void setText(const QString& text);
    const KMF::Font& font() const { return m_font; };
    void setFont(const KMF::Font& font) { m_font = font; };
    virtual void fromXML(const QDomElement& element);
    virtual int minimumPaintWidth() const;
    virtual int minimumPaintHeight() const;
    virtual void setProperty(const QString& name, QVariant value);

  protected:
    virtual void paintWidget(Magick::Image& layer, bool shdw = false);
    QString fitText(QString txt, int width);

  private:
    QString m_text;
    KMF::Font m_font;
};

#endif
