//**************************************************************************
//   Copyright (C) 2004, 2005 by Petri Damst�
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
#include "sizewidget.h"
#include "kmftools.h"
#include <kdebug.h>
#include <QLabel>
#include <QSplitter>
#include <Q3ValueList>

SizeWidget::SizeWidget(QWidget* parent)
  : QWidget(parent), m_max(0), m_size(0)
{
  setupUi(this);
}

SizeWidget::~SizeWidget()
{
}

void SizeWidget::update()
{
  sizeTakenLabel->setText(KMF::Tools::sizeString(m_size));
  if(m_size < m_max)
    sizeLeftLabel->setText(KMF::Tools::sizeString(m_max - m_size));
  else
    sizeLeftLabel->setText(KMF::Tools::sizeString(m_size - m_max));

  QPalette palette1;
  palette1.setColor(sizeLeftLabel->foregroundRole(),
                   (m_max > m_size) ? QColor(QPalette::Text) : QColor(Qt::red));
  sizeLeftLabel->setPalette(palette1);

  QPalette palette2;
  palette2.setColor(sizeLeft->backgroundRole(),
      (m_max > m_size)
      ? sizeTakenLabel->palette().color(sizeLeft->backgroundRole())
      : QColor(Qt::red));
  sizeLeft->setPalette(palette2);

  int n = 0;
  Q3ValueList<int> sizes = sizeSplitter->sizes();
  Q3ValueList<int>::Iterator it = sizes.begin();
  while(it != sizes.end())
  {
    n += *it;
    ++it;
  }
  if(m_size < m_max)
    sizes[0] = (int)((double)n * ((double)m_size / (double)m_max));
  else
    sizes[0] = (int)((double)n * ((double)m_max / (double)m_size));
  sizes[1] = n - sizes[0];
  sizeSplitter->setSizes(sizes);
}

#include "sizewidget.moc"
