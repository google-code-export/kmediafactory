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
#include "logview.h"
#include <qlayout.h>
#include <qlabel.h>
#include <khtml_part.h>

LogView::LogView(QWidget *parent, const char *name)
 : LogViewLayout(parent, name)
{
  m_layout = new QVBoxLayout(htmlFrame);
  m_htmlPart = new KHTMLPart(htmlFrame);
  m_layout->addWidget(m_htmlPart->widget());
  logLabel->setBuddy(m_htmlPart->widget());
}

LogView::~LogView()
{
  delete m_htmlPart;
  delete m_layout;
}

void LogView::setData(const KURL& log)
{
  m_htmlPart->openURL(log);
}

#include "logview.moc"
