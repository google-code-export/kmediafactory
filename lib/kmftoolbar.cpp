//**************************************************************************
//   Copyright (C) 2007 by Petri Damsten
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
#include "kmftoolbar.h"
#include <kicontheme.h>
#include <kdebug.h>
#include <QToolButton>
#include <QAction>
#include <QScrollBar>

KMFToolBar::KMFToolBar(QWidget* parent)
 : QScrollArea(parent)
{
  m_layout.setMargin(3);
  m_grid.setLayout(&m_layout);
  setWidget(&m_grid);
}

KMFToolBar::~KMFToolBar()
{
}

void KMFToolBar::resizeEvent(QResizeEvent* event)
{
  if(event)
    QScrollArea::resizeEvent(event);
  setMinimumSize(0, m_grid.height() + frameWidth() * 2 +
      ((horizontalScrollBar()->isVisible())?horizontalScrollBar()->height():0));
}

void KMFToolBar::add(QAction* action, const QString& group)
{
  kDebug() << k_funcinfo << endl;
  QToolButton* button = new QToolButton;
  button->setDefaultAction(action);
  button->setText(action->text());
  button->setIcon(action->icon());
  button->setAutoRaise(true);
  button->setFocusPolicy(Qt::NoFocus);
  button->setIconSize(QSize(K3Icon::SizeLarge, K3Icon::SizeLarge));
  button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

  m_layout.addWidget(button);
  m_grid.setLayout(&m_layout);
  m_grid.resize(m_layout.minimumSize());
  resizeEvent(0);
  m_actions[group].append(action);
}

#include "kmftoolbar.moc"
