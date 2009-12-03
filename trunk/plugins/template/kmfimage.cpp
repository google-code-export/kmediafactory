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
#include "kmfimage.h"
#include "kmfmenu.h"
#include "kmfmenupage.h"
#include <kstore/KoStore.h>
#include <lib/kmftools.h>
#include <KIO/NetAccess>
#include <KDebug>
#include <KStandardDirs>
#include <QVariant>
#include <QApplication>
#include <QPainter>

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
  setColor(QColor());
}

KMFImage::~KMFImage()
{
}

int KMFImage::paintWidth() const
{
  //kDebug() << paintRect().width();
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
  //kDebug() << rc;
  return rc;
}

QImage KMFImage::mask(const QImage& img, const QRgb& maskColor, bool oneBitMask) const
{
  QImage result(img.width(), img.height(), QImage::Format_ARGB32);
  double alphaScale = qAlpha(maskColor) / 255.0;

  //kDebug() << maskColor << alphaScale;
  for (int y = 0; y < img.height(); y++)
  {
    for (int x = 0; x < img.width(); x++)
    {
      QRgb pix = img.pixel(x, y);
      if(oneBitMask)
      {
        if(qAlpha(pix) > 128)
          pix = qRgba(qRed(maskColor), qGreen(maskColor),
                      qBlue(maskColor), 255);
        else
          pix = qRgba(qRed(maskColor), qGreen(maskColor), qBlue(maskColor), 0);
      }
      else
      {
        pix = qRgba(qRed(maskColor), qGreen(maskColor), qBlue(maskColor),
                    (int)(alphaScale * qAlpha(pix)));
      }
      result.setPixel(x, y, pix);
    }
  }
  return result;
}

void KMFImage::paintWidget(QImage* layer, bool shdw) const
{
    QPoint off = (shdw) ? shadow().offset() : QPoint();
    QColor clr = (shdw) ? shadow().color() : color();
    QRect rc = paintRect(off);
    QPainter p(layer);
    QImage image;

    if (m_image.isNull()) {
        QSize size;
        if (!m_scale) {
            size = svgSize().toSize();
        } else {
            if (!m_proportional) {
                size = QSize(rc.width(), rc.height());
            } else {
                size = svgSize().toSize();
                if (m_aspectRatio > (float)rc.width() / (float)rc.height()) {
                    size = QSize(rc.width(), size.height() * rc.width() / rc.height());
                } else {
                    size = QSize(size.width() * rc.height() / rc.width(), rc.height());
                }
            }
        }
        QImage img(size, QImage::Format_ARGB32_Premultiplied);
        QPainter p(&img);
        if (m_element.isEmpty()) {
            m_svg.render(&p, QRect(0, 0, size.width(), size.height()));
        } else {
            m_svg.render(&p, m_element, QRect(0, 0, size.width(), size.height()));
        }
        image = img;
    } else {
        image = m_image;
    }

    if(clr.isValid())
        image = mask(image, clr.rgba(), !shdw);
    else
        image = image;

    if(image.width() == 0 || image.height() == 0)
        return;

    if(m_scale && !m_image.isNull())
    {
        Qt::AspectRatioMode mode = (m_proportional) ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio;
        image = image.scaled(rc.width(), rc.height(), mode, Qt::SmoothTransformation);
    }
    //kDebug() << m_url << ": " <<  shdw;
    p.drawImage(QPoint(rc.left(), rc.top()), image);
}

void KMFImage::setImage(KUrl url)
{
    bool ok = false;
    bool svg = false;

    m_url = url;
    m_element = url.fragment().mid(1);
    QString ext = url.path().mid(url.path().lastIndexOf('.') + 1);
    if (ext == "svg" || ext == "svgz") {
        svg = true;
    }
    kDebug() << url << ext;
    if(url.protocol() == "project")
    {
        QList<KMF::MediaObject*> mobs = m_interface->mediaObjects();
        int title;
        int chapter;

        parseTitleChapter(url.path().mid(1), title, chapter);
        if(title > 0 && title <= (int)mobs.count()
        && chapter <= (int)mobs.at(title-1)->chapters())
        {
            setImage(mobs.at(title-1)->preview(chapter));
            ok = true;
        }
    }
    else if(url.protocol() == "template")
    {
        const KMFTemplate* tmplate = menu()->templateStore();
        if (svg) {
            setImage(tmplate->readFile(url.path().mid(1)));
        } else {
            QImage img;
            img.loadFromData(tmplate->readFile(url.path().mid(1)));
            setImage(img);
        }
        ok = true;
    }
    else if(url.protocol() == "kde")
    {
        QString tmpFile = KStandardDirs::locate(url.host().toLocal8Bit(),
                                                url.path().mid(1));
        kDebug() << url.host().toLocal8Bit() << url.path().mid(1) << tmpFile;
        if(!tmpFile.isEmpty())
        {
            if (svg) {
                setImage(KMF::Tools::loadByteArray(tmpFile));
            } else {
                setImage(QImage(tmpFile));
            }
            ok = true;
        }
    }
    else
    {
        QString tmpFile;
        if(url.protocol() != "file") {
            m_element = "";
        }
        if(KIO::NetAccess::download(url, tmpFile, qApp->activeWindow()))
        {
            if (svg) {
                setImage(KMF::Tools::loadByteArray(tmpFile));
            } else {
                setImage(QImage(tmpFile));
            }
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
  m_svg.load(QByteArray());
  m_image = image;
  m_aspectRatio = (float)m_image.width() / (float)m_image.height();
}

QSizeF KMFImage::svgSize() const
{
  if (m_element.isEmpty()) {
      return m_svg.defaultSize();
  } else {
      return m_svg.boundsOnElement(m_element).size();
  }
}

void KMFImage::setImage(const QByteArray& ba)
{
  m_image = QImage();
  m_svg.load(ba);
  QSizeF size = svgSize();
  m_aspectRatio = (float)size.width() / (float)size.height();
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
  //kDebug() << result;
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
  //kDebug() << result;
  return result;
}

void KMFImage::setProperty(const QString& name, QVariant value)
{
  KMFWidget::setProperty(name, value);
  if(name == "url")
    setImage(value.value<KUrl>());
}

#include "kmfimage.moc"
