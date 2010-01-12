//**************************************************************************
//   Copyright (C) 2004-2008 by Petri Damsten
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
#include "kmfapplication.h"

LogView::LogView(QWidget *parent)
 : KDialog(parent)
{
  setupUi(mainWidget());
  setButtons(KDialog::Close);

  setInitialSize(QSize(640, 420));
  KConfigGroup cg = KGlobal::config()->group("LogView");
  restoreDialogSize(cg);
}

void LogView::closeEvent(QCloseEvent *e)
{
  KConfigGroup cg = KGlobal::config()->group("LogView");
  KDialog::saveDialogSize(cg);
  KGlobal::config()->sync();
  KDialog::closeEvent(e);
}

LogView::~LogView()
{
}

void LogView::setData(const KUrl& log)
{
  htmlFrame->setSource(log);
}

#include "logview.moc"