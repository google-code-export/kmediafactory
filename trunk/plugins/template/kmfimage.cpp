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
#include "kmfimage.h"
#include "kmfmenu.h"
#include "kmfmenupage.h"
#include <kstore/KoStore.h>
#include <kio/netaccess.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <QVariant>
#include <QApplication>

const char * transparent_xpm[] =
{
  "1 1 1 1",
  "   c None",
  " "
};

QImage KMFImage::m_empty(transparent_xpm);

KMFImage::KMFImage(QObject *parent)
  : KMFWidget(parent), m_scale(true), m_proportional(true)
{
  // set color to transparent, otherwise image will be drawn with one color.
  setColor(QColor(0,0,0,0));
}

KMFImage::~KMFImage()
{
}

int KMFImage::paintWidth() const
{
  //kdDebug() << k_funcinfo << paintRect().width() << endl;
  return paintRect().width();
}

int KMFImage::paintHeight() const
{
  return paintRect().height();
}

int KMFImage::paintX() const
{
  return paintRect().x();
}

int KMFImage::paintY() const
{
  return paintRect().y();
}

QRect KMFImage::paintRect(const QPoint offset) const
{
  KMF::Rect rc;
  if(m_scale)
  {
    if(m_proportional)
    {
      rc.set(KMFWidget::paintRect(), m_aspectRatio);
      rc.align(KMFWidget::paintRect(), halign(), valign());
    }
    else
      rc = KMFWidget::paintRect();
  }
  else
  {
    rc = QRect(0, 0, m_image.width(), m_image.height());
    rc.align(KMFWidget::paintRect(), halign(), valign());
  }
  rc.translate(offset.x(), offset.y());
  //kdDebug() << k_funcinfo << rc << endl;
  return rc;
}

void KMFImage::paintWidget(QImage& layer, bool shdw)
{
#warning TODO
#if 0
  QPoint off = (shdw) ? shadow().offset() : QPoint();
  KMF::Color clr = (shdw) ? shadow().color() : color();
  QRect rc = paintRect(off);
  QMImage image;

  if(!clr.isNull())
  {
    QMImage img(m_image, clr.rgb(), !shdw);
    image = img;
  }
  else
  {
    QMImage img(m_image);
    image = img;
  }

  if(image.columns() == 0 || image.rows() == 0)
    return;

  if(m_scale)
  {
    Magick::Geometry g(rc.width(), rc.height());
    g.aspect(!m_proportional);
    image.scale(g);
  }
  //kdDebug() << k_funcinfo << name() << rc << endl;
  layer.composite(image, rc.left(), rc.top(), Magick::OverCompositeOp);
#endif
}

void KMFImage::setImage(KUrl url)
{
  bool ok = false;

  if(url.protocol() == "project")
  {
    QList<KMF::MediaObject*>* mobs = m_prjIf->mediaObjects();
    int title;
    int chapter;

    parseTitleChapter(url.path().mid(1), title, chapter);
    if(title > 0 && title <= (int)mobs->count()
       && chapter <= (int)mobs->at(title-1)->chapters())
    {
      setImage(mobs->at(title-1)->preview(chapter));
      ok = true;
    }
  }
  else if(url.protocol() == "template")
  {
    const KMFTemplate* tmplate = menu()->templateStore();
    QImage img;

    img.loadFromData(tmplate->readFile(url.path().mid(1)));
    setImage(img);
    ok = true;
  }
  else if(url.protocol() == "kde")
  {
    QString tmpFile = KStandardDirs::locate(url.host().toLocal8Bit(),
                                            url.path().mid(1));
    if(!tmpFile.isEmpty())
    {
      ok = true;
      setImage(QImage(tmpFile));
    }
  }
  else
  {
    QString tmpFile;
    if(KIO::NetAccess::download(url, tmpFile, qApp->activeWindow()))
    {
      setImage(QImage(tmpFile));
      KIO::NetAccess::removeTempFile(tmpFile);
      ok = true;
    }
  }
  if(!ok)
  {
    setImage(m_empty);
    if(!takeSpace())
      hide();
  }
}

void KMFImage::setImage(const QImage& image)
{
  m_image = image;
  m_aspectRatio = (float)m_image.width() / (float)m_image.height();
}

void KMFImage::fromXML(const QDomElement& element)
{
  KMFWidget::fromXML(element);
  m_scale = element.attribute("scale", "1").toInt();
  m_proportional = element.attribute("proportional", "1").toInt();
  setImage(KUrl(element.attribute("url", "")));
}

int KMFImage::minimumPaintWidth() const
{
  int result = m_image.width();

  if(geometry().width().type() == KMFUnit::Absolute)
  {
    result = geometry().width().value();
  }
  else if(geometry().width().type() == KMFUnit::Minimum)
  {
    if(geometry().height().type() == KMFUnit::Absolute)
    {
      result = (int)((double)geometry().height().value() * m_aspectRatio);
    }
    else if(geometry().height().type() == KMFUnit::Percentage)
    {
      result = (int)((double)geometry().h() * m_aspectRatio);
    }
  }
  //kdDebug() << k_funcinfo << result << endl;
  return result;
}

int KMFImage::minimumPaintHeight() const
{
  int result = m_image.height();

  if(geometry().height().type() == KMFUnit::Absolute)
  {
    result = geometry().height().value();
  }
  else if(geometry().height().type() == KMFUnit::Minimum)
  {
    if(geometry().width().type() == KMFUnit::Absolute)
    {
      result = (int)((double)geometry().width().value() / m_aspectRatio);
    }
    else if(geometry().width().type() == KMFUnit::Percentage)
    {
      result = (int)((double)geometry().w() / m_aspectRatio);
    }
  }
  //kdDebug() << k_funcinfo << result << endl;
  return result;
}

void KMFImage::setProperty(const QString& name, QVariant value)
{
  KMFWidget::setProperty(name, value);
  if(name == "url" && !value.toString().isEmpty())
    setImage(KUrl(value.toString()));
}

#include "kmfimage.moc"
