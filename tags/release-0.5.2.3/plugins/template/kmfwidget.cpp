//**************************************************************************
//   Copyright (C) 2004 by Petri Damst�
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
#include "kmfwidget.h"
#include "kmflayout.h"
#include "kmfmenupage.h"
#include <qobjectlist.h>
#include <magick/effect.h>
#include "Magick++/Exception.h"
#include <kdebug.h>
#include <qvariant.h>
#include <qdom.h>
#include <qregexp.h>

void KMFShadow::toXML(QDomElement& element) const
{
  element.setAttribute("offset.x", m_offset.x());
  element.setAttribute("offset.y", m_offset.y());
  element.setAttribute("color", m_color.rgb());
  element.setAttribute("type", m_type);
  element.setAttribute("radius", m_radius);
  element.setAttribute("sigma", m_sigma);
}

void KMFShadow::fromXML(const QDomElement& element)
{
  m_offset.setX(element.attribute("offset.x", 0).toInt());
  m_offset.setY(element.attribute("offset.y", 0).toInt());
  m_color.setNamedColor(element.attribute("color", "#00000088"));
  m_type = static_cast<Type>(element.attribute("type", "0").toInt());
  m_radius = element.attribute("radius", "1").toDouble();
  m_sigma = element.attribute("sigma", "0.5").toDouble();
}

KMFWidget::KMFWidget(QObject *parent, const char *name)
 : KMFTemplateBase(parent, name), m_geometry(this), m_layer(Background),
   m_valign(KMF::Rect::Middle), m_halign(KMF::Rect::Center), m_hidden(false),
   m_color(qRgba(0,0,0,255)), m_takeSpace(false), m_row(0), m_column(0)
{
}

KMFWidget::~KMFWidget()
{
}

int KMFWidget::minimumWidth() const
{
  return minimumPaintWidth() + m_geometry.left().margin()
                             + m_geometry.width().margin();
}

int KMFWidget::minimumHeight() const
{
  return minimumPaintHeight() + m_geometry.top().margin()
                              + m_geometry.height().margin();
}

int KMFWidget::minimumPaintWidth() const
{
  const QObjectList *list = children();
  int result = 0;

  if(list)
    for(QObjectListIt it(*list); it.current() != 0; ++it)
      if((*it)->inherits("KMFWidget"))
        result = QMAX(static_cast<KMFWidget*>(*it)->minimumWidth(), result);
  return result;
}

int KMFWidget::minimumPaintHeight() const
{
  const QObjectList *list = children();
  int result = 0;

  if(list)
    for(QObjectListIt it(*list); it.current() != 0; ++it)
      if((*it)->inherits("KMFWidget"))
        result = QMAX(static_cast<KMFWidget*>(*it)->minimumHeight(), result);
  return result;
}

void KMFWidget::setLayer(const Layer layer)
{
  const QObjectList *list = children();
  QObject *obj;

  if(list)
    for(QObjectListIt it(*list); (obj=it.current())!= 0; ++it)
      if(obj->inherits("KMFWidget"))
        ((KMFWidget*)obj)->setLayer(layer);

  m_layer = layer;
}

void KMFWidget::setShadow(const KMFShadow& shadow)
{
  const QObjectList *list = children();
  QObject *obj;

  if(list)
    for(QObjectListIt it(*list); (obj=it.current())!= 0; ++it)
      if(obj->inherits("KMFWidget"))
        ((KMFWidget*)obj)->setShadow(shadow);

  m_shadow = shadow;
}

QRect KMFWidget::paintRect(const QPoint offset) const
{
  QRect result = m_geometry.paintRect();
  result.moveBy(offset.x(), offset.y());
  return result;
}

void KMFWidget::paint(KMFMenuPage* page)
{
  if(m_shadow.type() != KMFShadow::None && layer() == Background)
  {
    Magick::Image& temp = page->layer(Temp);
    temp.read(QString("xc:%1").arg(QColor(m_shadow.color()).name()));
    temp.opacity(TransparentOpacity);
    paintWidget(temp, true);
    if(m_shadow.type() == KMFShadow::Blur)
    {
      MagickLib::ExceptionInfo exceptionInfo;
      GetExceptionInfo(&exceptionInfo);
      MagickLib::Image* newImage =
          BlurImageChannel(temp.image(), MagickLib::AllChannels,
                       m_shadow.radius(), m_shadow.sigma(), &exceptionInfo);
      temp.replaceImage(newImage);
      //throwException(exceptionInfo);
    }
    page->layer(Background).composite(temp, 0, 0, Magick::OverCompositeOp);
  }
  paintWidget(page->layer(layer()), false);
}

void KMFWidget::setColor(const QString& s)
{
  m_color.setNamedColor(s);
}

void KMFWidget::fromXML(const QDomElement& element)
{
  setName(element.attribute("name", ""));
  m_geometry.setMargin(element.attribute("margin", "0"));
  m_takeSpace = element.attribute("take_space", "0").toInt();
  m_row = element.attribute("row", "0").toInt();
  m_column = element.attribute("column", "0").toInt();
  if(element.hasAttribute("color"))
    setColor(element.attribute("color"));
  if(element.hasAttribute("x"))
    m_geometry.left().set(element.attribute("x"));
  if(element.hasAttribute("y"))
    m_geometry.top().set(element.attribute("y"));
  if(element.hasAttribute("w"))
    m_geometry.width().set(element.attribute("w"));
  if(element.hasAttribute("h"))
    m_geometry.height().set(element.attribute("h"));
  if(element.hasAttribute("halign"))
  {
    QString align = element.attribute("halign").lower();
    if(align = "left")
      m_halign = KMF::Rect::Left;
    else if(align = "right")
      m_halign = KMF::Rect::Right;
    else if(align = "center")
      m_halign = KMF::Rect::Center;
  }
  if(element.hasAttribute("valign"))
  {
    QString align = element.attribute("valign").lower();
    if(align = "top")
      m_valign = KMF::Rect::Top;
    else if(align = "bottom")
      m_valign = KMF::Rect::Bottom;
    else if(align = "middle")
      m_valign = KMF::Rect::Middle;
  }
  if(element.hasAttribute("layer"))
  {
    QString layer = element.attribute("layer").lower().stripWhiteSpace();
    if(layer == "sub")
      setLayer(Sub);
    else if(layer == "select")
      setLayer(Select);
    else if(layer == "highlight")
      setLayer(Highlight);
  }

  QDomNode n = element.firstChild();
  while(!n.isNull())
  {
    QDomElement e = n.toElement();
    if(!e.isNull())
    {
      if(e.tagName() == "shadow")
        m_shadow.fromXML(e);
    }
    n = n.nextSibling();
  }
  //kdDebug() << name() << " rect: " << m_geometry.rect() << endl;
}

int KMFWidget::toInt(const QString& s, int offset)
{
  if(s.upper()[0] == 'X')
    return -1;
  if(s[0] == '$')
    return s.mid(1).toInt();
  else
    return s.toInt() + offset;
}

void KMFWidget::parseTitleChapter(const QString& s, int& title, int& chapter)
{
  title = 0;
  chapter = 0;
  QStringList list = QStringList::split(".", s);

  if(list.count() > 0)
    title = toInt(list[0], page()->titleStart());
  if(list.count() > 1)
    chapter = toInt(list[1], page()->chapterStart());
}

void KMFWidget::setProperty(const QString& name, QVariant value)
{
  if(name == "color")
    setColor(value.toColor().rgb());
}

#include "kmfwidget.moc"
