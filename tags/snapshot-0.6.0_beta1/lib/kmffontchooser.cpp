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
#include <kdialog.h>
#include <kfontdialog.h>
#include <klocale.h>
#include <kiconloader.h>
#include <QLabel>
#include <QPushButton>
#include <QLayout>
#include <QFont>
#include <QPixmap>
#include <QHBoxLayout>
#include "kmffontchooser.h"

KMFFontChooser::KMFFontChooser(QWidget *parent)
 : QWidget(parent)
{
  QHBoxLayout* layout = new QHBoxLayout(this);

  m_label = new QLabel(this);
  m_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  layout->addWidget(m_label);
  layout->setMargin(0);
  layout->setSpacing(KDialog::spacingHint());

  m_button = new QPushButton(this);
  QString fontText = i18n("Font...");
  m_button->setText(fontText);
  m_button->setIcon(KIcon("character-set"));
  m_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  layout->addWidget(m_button);

  connect(m_button, SIGNAL(clicked()), this, SLOT(buttonClicked()));

  updateFontLabel();

  setFocusProxy(m_button);
}

KMFFontChooser::~KMFFontChooser()
{
}

void KMFFontChooser::setFont(const QFont& font)
{
  m_font = font;
  updateFontLabel();
}

void KMFFontChooser::updateFontLabel()
{
  QString s = QString("%1, %2pt").arg(m_font.family()).arg(m_font.pointSize());
  m_label->setText(s);
}

void KMFFontChooser::buttonClicked()
{
  KFontDialog::getFont(m_font);
  updateFontLabel();
}

#include "kmffontchooser.moc"