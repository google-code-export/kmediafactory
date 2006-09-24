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
#include "kmflabel.h"
#include "kmfmenu.h"
#include "kmfmenupage.h"
#include <rect.h>
#include <kmftools.h>
#include <kdebug.h>
#include <QColor>
#include <QRegExp>
#include <QVariant>
#include <string>

KMFLabel::KMFLabel(QObject *parent) :
  KMFWidget(parent), m_text("")
{
}

KMFLabel::~KMFLabel()
{
}

int KMFLabel::minimumPaintWidth() const
{
#warning TODO
  return 0; //m_font.pixelWidth(m_text);
}

int KMFLabel::minimumPaintHeight() const
{
  //kdDebug() << k_funcinfo << m_font.pixelHeight(m_text, m_descent) << endl;
#warning TODO
  return 0;//m_font.pixelHeight();
}

void KMFLabel::fromXML(const QDomElement& element)
{
  KMFWidget::fromXML(element);
  QDomNode n = element.firstChild();
  while(!n.isNull())
  {
    QDomElement e = n.toElement();
    if(!e.isNull())
    {
      if(e.tagName() == "font")
        m_font = KMF::Tools::fontFromXML(e);
      else if(e.tagName() == "text")
        setText(e.text());
    }
    n = n.nextSibling();
  }
}

void KMFLabel::setText(const QString& text)
{
  QRegExp rx("%([\\d\\.$]+)%" );
  QList<KMF::MediaObject*>* mobs = m_prjIf->mediaObjects();
  int title, chapter;
  QString s;
  int pos = 0;
  KMFTemplate* tmplate = menu()->templateStore();

  m_text = tmplate->translate(text.toLocal8Bit());

  while(pos >= 0)
  {
    pos = rx.indexIn(text, pos);
    if(pos > -1)
    {
      parseTitleChapter(rx.cap(1), title, chapter);

      if(title < 1)
        s = m_prjIf->title();
      else
      {
        if(title <= (int)mobs->count()
          && chapter <= (int)mobs->at(title-1)->chapters())
          s = mobs->at(title-1)->text(chapter);
        else
          s = "";
      }
      m_text.replace("%" + rx.cap(1) + "%", s);
      pos += rx.matchedLength();
    }
  }
  m_text.replace("%%", "%");
  if(m_text.isEmpty() && !takeSpace())
    hide();
}

void KMFLabel::paintWidget(QImage& layer, bool shdw)
{
#warning TODO
#if 0
  /*
  int h = (int)t.textHeight();
  QRegExp re("[gjpqy]");

  if(descent || re.search(text) == -1) // no letters that have descent
    h += (int)t.descent();
  */
  //kdDebug() << k_funcinfo << m_font.family() << m_font.pointSize() <<
  //    m_font.weight() << endl;
  Layer lt = page()->layerType(layer);
  QRect rc = (shdw)? paintRect(shadow().offset()) : paintRect();
  QString text = fitText(m_text, rc.width());
  QColor rgb = (shdw)? shadow().color() : color() ;
  KMF::Rect textrc(QPoint(0, 0), m_font.pixelSize(text));
  std::list<Magick::Drawable> drawList;

  drawList.push_back(Magick::DrawableStrokeWidth(0));
  drawList.push_back(Magick::DrawableGravity(Magick::NorthWestGravity));
  drawList.push_back(Magick::DrawableFillColor(rgb));
  drawList.push_back(Magick::DrawableFillOpacity((double)rgb.alpha() / 255.0));
  if(!m_font.file().isEmpty())
  {
    drawList.push_back(Magick::DrawableFont(
        (const char*)m_font.file().local8Bit()));
    /*
    kdDebug() << k_funcinfo << (const char*)m_font.file().local8Bit() << endl;
    */
  }
  else
  {
    drawList.push_back(Magick::DrawableFont(
        (const char*)m_font.family().local8Bit()));
    /*
    kdDebug() << k_funcinfo << (const char*)m_font.family().local8Bit() << endl;
    */
  }
  drawList.push_back(Magick::DrawablePointSize(m_font.pointSize()));

  textrc.align(rc, halign(), valign());
  drawList.push_back(
      Magick::DrawableTextAntialias(lt == Background || lt == Temp));
  drawList.push_back(Magick::DrawableText(textrc.x(), textrc.y(),
                     (const char*)text.utf8(), "UTF-8"));
  layer.draw(drawList);
#endif
}

void KMFLabel::setProperty(const QString& name, QVariant value)
{
  KMFWidget::setProperty(name, value);
  if(name == "font" && !value.toString().isEmpty())
  {
    m_font = value.value<QFont>();
    //kdDebug() << k_funcinfo << m_font.family() << m_font.pointSize() <<
    //    m_font.weight() << endl;
  }
}

QString KMFLabel::fitText(QString txt, int width)
{
#warning TODO
#if 0
  QString s = txt;
  int i = 0;

  if(!s.isEmpty())
  {
    while(!s.isEmpty() && m_font.pixelWidth(s) > width)
    {
      int n = s.findRev(' ');
      if(n < 0)
      {
        s = "";
        break;
      }
      s = s.left(n);
    }
    if(s.isEmpty())
    {
      while(m_font.pixelWidth(s) < width)
      {
        s += txt[i++];
      }
      if(!s.isEmpty())
        s = s.left(s.length() - 1);
    }
  }
  //kdDebug() << k_funcinfo << "return: " << s << endl;
  return s;
#endif
}

#include "kmflabel.moc"
