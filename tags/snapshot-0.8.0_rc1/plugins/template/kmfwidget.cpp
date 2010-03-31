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

#include "kmfwidget.h"

#include <QtCore/QObject>
#include <QtCore/QRegExp>
#include <QtCore/QVariant>
#include <QtGui/QPainter>

#include <KDebug>

#include <cmath>

#include <kmftools.h>
#include "kmflayout.h"
#include "kmfmenupage.h"

// Exponential blur, Jani Huhtanen, 2006
//
template<int aprec, int zprec>
static inline void blurinner(unsigned char *bptr, int &zR, int &zG, int &zB, int &zA, int alpha);

template<int aprec, int zprec>
static inline void blurrow(QImage &im, int line, int alpha);

template<int aprec, int zprec>
static inline void blurcol(QImage &im, int col, int alpha);

/*
 *  expblur(QImage &img, int radius)
 *
 *  In-place blur of image 'img' with kernel
 *  of approximate radius 'radius'.
 *
 *  Blurs with two sided exponential impulse
 *  response.
 *
 *  aprec = precision of alpha parameter
 *  in fixed-point format 0.aprec
 *
 *  zprec = precision of state parameters
 *  zR,zG,zB and zA in fp format 8.zprec
 */
template<int aprec, int zprec>
void expblur(QImage &img, int radius)
{
    if (radius < 1) {
        return;
    }

    /* Calculate the alpha such that 90% of
     * the kernel is within the radius.
     * (Kernel extends to infinity)
     */
    int alpha = (int)((1 << aprec) * (1.0f - expf(-2.3f / (radius + 1.f))));

    for (int row = 0; row < img.height(); row++) {
        blurrow<aprec, zprec>(img, row, alpha);
    }

    for (int col = 0; col < img.width(); col++) {
        blurcol<aprec, zprec>(img, col, alpha);
    }

    return;
}

template<int aprec, int zprec>
static inline void blurinner(unsigned char *bptr, int &zR, int &zG, int &zB, int &zA, int alpha)
{
    int R, G, B, A;

    R = *bptr;
    G = *(bptr + 1);
    B = *(bptr + 2);
    A = *(bptr + 3);

    zR += (alpha * ((R << zprec) - zR)) >> aprec;
    zG += (alpha * ((G << zprec) - zG)) >> aprec;
    zB += (alpha * ((B << zprec) - zB)) >> aprec;
    zA += (alpha * ((A << zprec) - zA)) >> aprec;

    *bptr =     zR >> zprec;
    *(bptr + 1) = zG >> zprec;
    *(bptr + 2) = zB >> zprec;
    *(bptr + 3) = zA >> zprec;
}

template<int aprec, int zprec>
static inline void blurrow(QImage &im, int line, int alpha)
{
    int zR, zG, zB, zA;

    QRgb *ptr = (QRgb *)im.scanLine(line);

    zR = *((unsigned char *)ptr) << zprec;
    zG = *((unsigned char *)ptr + 1) << zprec;
    zB = *((unsigned char *)ptr + 2) << zprec;
    zA = *((unsigned char *)ptr + 3) << zprec;

    for (int index = 1; index < im.width(); index++) {
        blurinner<aprec, zprec>((unsigned char *)&ptr[index],
                                zR, zG, zB, zA, alpha);
    }

    for (int index = im.width() - 2; index >= 0; index--) {
        blurinner<aprec, zprec>((unsigned char *)&ptr[index],
                                zR, zG, zB, zA, alpha);
    }
}

template<int aprec, int zprec>
static inline void blurcol(QImage &im, int col, int alpha)
{
    int zR, zG, zB, zA;

    QRgb *ptr = (QRgb *)im.bits();

    ptr += col;

    zR = *((unsigned char *)ptr) << zprec;
    zG = *((unsigned char *)ptr + 1) << zprec;
    zB = *((unsigned char *)ptr + 2) << zprec;
    zA = *((unsigned char *)ptr + 3) << zprec;

    for (int index = im.width(); index < (im.height() - 1) * im.width();
         index += im.width())
    {
        blurinner<aprec, zprec>((unsigned char *)&ptr[index],
                                zR, zG, zB, zA, alpha);
    }

    for (int index = (im.height() - 2) * im.width(); index >= 0;
         index -= im.width())
    {
        blurinner<aprec, zprec>((unsigned char *)&ptr[index],
                                zR, zG, zB, zA, alpha);
    }
}

void KMFShadow::toXML(QDomElement &element) const
{
    element.setAttribute("offset.x", m_offset.x());
    element.setAttribute("offset.y", m_offset.y());
    element.setAttribute("color", m_color.rgb());
    element.setAttribute("type", m_type);
    element.setAttribute("radius", m_radius);
    element.setAttribute("sigma", m_sigma);
}

void KMFShadow::fromXML(const QDomElement &element)
{
    m_offset.setX(element.attribute("offset.x", 0).toInt());
    m_offset.setY(element.attribute("offset.y", 0).toInt());
    m_color = KMF::Tools::toColor(element.attribute("color", "#00000088"));
    m_type = static_cast<Type>(element.attribute("type", "0").toInt());
    m_radius = element.attribute("radius", "1").toDouble();
    m_sigma = element.attribute("sigma", "0.5").toDouble();
}

KMFWidget::KMFWidget(QObject *parent)
    : KMFTemplateBase(parent)
    , m_geometry(this)
    , m_layer(Background)
    , m_valign(KMF::Rect::Middle)
    , m_halign(KMF::Rect::Center)
    , m_hidden(false)
    , m_color("transparent")
    , m_takeSpace(false)
    , m_row(0)
    , m_column(0)
{
    m_color.setAlpha(0);
}

KMFWidget::~KMFWidget()
{
}

int KMFWidget::minimumWidth() const
{
    return minimumPaintWidth() + m_geometry.left().margin() +
           m_geometry.width().margin();
}

int KMFWidget::minimumHeight() const
{
    return minimumPaintHeight() + m_geometry.top().margin() +
           m_geometry.height().margin();
}

int KMFWidget::minimumPaintWidth() const
{
    int result = 0;

    foreach (QObject * ob, children()) {
        if (ob->inherits("KMFWidget")) {
            result = qMax(static_cast<KMFWidget *>(ob)->minimumWidth(), result);
        }
    }
    return result;
}

int KMFWidget::minimumPaintHeight() const
{
    int result = 0;

    foreach (QObject * ob, children()) {
        if (ob->inherits("KMFWidget")) {
            result = qMax(static_cast<KMFWidget *>(ob)->minimumHeight(), result);
        }
    }
    return result;
}

void KMFWidget::setLayer(const Layer layer)
{
    foreach(QObject * obj, children()) {
        if (obj->inherits("KMFWidget")) {
            ((KMFWidget *)obj)->setLayer(layer);
        }
    }
    m_layer = layer;
}

void KMFWidget::setShadow(const KMFShadow &shadow)
{
    foreach (QObject * obj, children()) {
        if (obj->inherits("KMFWidget")) {
            ((KMFWidget *)obj)->setShadow(shadow);
        }
    }
    m_shadow = shadow;
}

QRect KMFWidget::paintRect(const QPoint offset) const
{
    QRect result = m_geometry.paintRect();

    result.translate(offset.x(), offset.y());
    return result;
}

void KMFWidget::paint(QImage *layer, bool background) const
{
    if ((m_shadow.type() != KMFShadow::None) && background) {
        QImage temp = *layer;
        QColor c = m_shadow.color();

        c.setAlpha(0);
        temp.fill(c.rgba());
        temp.setText("layer", "temp");
        paintWidget(&temp, true);

        if (m_shadow.type() == KMFShadow::Blur) {
            expblur<16, 7>(temp, (int)m_shadow.radius());
            // temp = KImageEffect::blur(temp, m_shadow.radius(), m_shadow.sigma());
        }

        // static int i = 0;
        // temp.save(m_prjIf->projectDir("menus") + QString("%1.png").arg(++i));
        QPainter p(layer);
        p.drawImage(QPoint(0, 0), temp);
    }

    paintWidget(layer, false);
}

void KMFWidget::setColor(const QString &s)
{
    m_color = KMF::Tools::toColor(s);
}

void KMFWidget::fromXML(const QDomElement &element)
{
    setObjectName(element.attribute("name", ""));
    m_geometry.setMargin(element.attribute("margin", "0"));
    m_takeSpace = element.attribute("take_space", "0").toInt();
    m_row = element.attribute("row", "0").toInt();
    m_column = element.attribute("column", "0").toInt();

    if (element.hasAttribute("color")) {
        setColor(element.attribute("color"));
    }

    if (element.hasAttribute("x")) {
        m_geometry.left().set(element.attribute("x"));
    }

    if (element.hasAttribute("y")) {
        m_geometry.top().set(element.attribute("y"));
    }

    if (element.hasAttribute("w")) {
        m_geometry.width().set(element.attribute("w"));
    }

    if (element.hasAttribute("h")) {
        m_geometry.height().set(element.attribute("h"));
    }

    if (element.hasAttribute("halign")) {
        QString align = element.attribute("halign").toLower();

        if (align == "left") {
            m_halign = KMF::Rect::Left;
        } else if (align == "right") {
            m_halign = KMF::Rect::Right;
        } else if (align == "center") {
            m_halign = KMF::Rect::Center;
        }
    }

    if (element.hasAttribute("valign")) {
        QString align = element.attribute("valign").toLower();

        if (align == "top") {
            m_valign = KMF::Rect::Top;
        } else if (align == "bottom") {
            m_valign = KMF::Rect::Bottom;
        } else if (align == "middle") {
            m_valign = KMF::Rect::Middle;
        }
    }

    if (element.hasAttribute("layer")) {
        QString layer = element.attribute("layer").toLower().trimmed();

        if (layer == "sub") {
            setLayer(Sub);
        } else if (layer == "select") {
            setLayer(Select);
        } else if (layer == "highlight") {
            setLayer(Highlight);
        }
    }

    QDomNode n = element.firstChild();

    while (!n.isNull()) {
        QDomElement e = n.toElement();

        if (!e.isNull()) {
            if (e.tagName() == "shadow") {
                m_shadow.fromXML(e);
            }
        }

        n = n.nextSibling();
    }
    // kDebug() << name() << " rect: " << m_geometry.rect();
}

int KMFWidget::toInt(const QString &s, int offset)
{
    if (s.toUpper()[0] == 'X') {
        return -1;
    }

    if (s[0] == '$') {
        return s.mid(1).toInt();
    } else {
        return s.toInt() + offset;
    }
}

void KMFWidget::parseTitleChapter(const QString &s, int &title, int &chapter)
{
    title = 0;
    chapter = 0;
    QStringList list = s.split('.');

    if (list.count() > 0) {
        title = toInt(list[0], page()->titleStart());
    }

    if (list.count() > 1) {
        chapter = toInt(list[1], page()->chapterStart());
    }
}

void KMFWidget::setProperty(const QString &name, QVariant value)
{
    if (name == "color") {
        // kDebug() << value.value<QColor>();
        setColor(value.value<QColor>());
    }
}

#include "kmfwidget.moc"
