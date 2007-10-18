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
#include <QPainter>
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
  QImage img(1, 1, QImage::Format_ARGB32);
  img.setDotsPerMeterX(DPM);
  img.setDotsPerMeterY(DPM);
  QFontMetrics fm(m_font, &img);
  return fm.size(Qt::TextSingleLine, m_text).width();
}

int KMFLabel::minimumPaintHeight() const
{
  //kDebug() << k_funcinfo << m_font.pixelHeight(m_text, m_descent);
  QImage img(1, 1, QImage::Format_ARGB32);
  img.setDotsPerMeterX(DPM);
  img.setDotsPerMeterY(DPM);
  QFontMetrics fm(m_font, &img);
  return fm.size(Qt::TextSingleLine, m_text).height();
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
  QList<KMF::MediaObject*> mobs = m_prjIf->mediaObjects();
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
        if(title <= (int)mobs.count()
          && chapter <= (int)mobs.at(title-1)->chapters())
          s = mobs.at(title-1)->text(chapter);
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
  QPainter p(&layer);
  QFontMetrics fm(m_font, &layer);

  //kDebug() << k_funcinfo << m_font.family() << m_font.pointSize() <<
  //    m_font.weight();
  Layer lt = page()->layerType(layer);
  QRect rc = (shdw)? paintRect(shadow().offset()) : paintRect();
  QString text = fitText(m_text, rc.width());
  QColor rgb = (shdw)? shadow().color() : color() ;
  KMF::Rect textrc(QPoint(0, 0), fm.size(Qt::TextSingleLine, m_text));

  p.setPen(QPen(rgb));
  p.setBrush(QBrush());
  p.setFont(m_font);

  textrc.align(rc, halign(), valign());
  bool aa = (lt == Background || lt == Temp);
  p.setRenderHint(QPainter::TextAntialiasing, aa);
  p.drawText(textrc, Qt::AlignLeft, text);
}

void KMFLabel::setProperty(const QString& name, QVariant value)
{
  KMFWidget::setProperty(name, value);
  if(name == "font" && !value.toString().isEmpty())
  {
    m_font = value.value<QFont>();
    //kDebug() << k_funcinfo << m_font.family() << m_font.pointSize() <<
    //    m_font.weight();
  }
}

QString KMFLabel::fitText(QString txt, int width)
{
  QString s = txt;
  int i = 0;
  QImage img(1, 1, QImage::Format_ARGB32);
  img.setDotsPerMeterX(DPM);
  img.setDotsPerMeterY(DPM);
  QFontMetrics fm(m_font, &img);

  if(!s.isEmpty())
  {
    while(!s.isEmpty() && fm.size(Qt::TextSingleLine, s).width() > width)
    {
      int n = s.lastIndexOf(' ');
      if(n < 0)
      {
        s = "";
        break;
      }
      s = s.left(n);
    }
    if(s.isEmpty())
    {
      while(fm.size(Qt::TextSingleLine, s).width() < width)
      {
        s += txt[i++];
      }
      if(!s.isEmpty())
        s = s.left(s.length() - 1);
    }
  }
  //kDebug() << k_funcinfo << "return: " << s;
  return s;
}

#include "kmflabel.moc"
