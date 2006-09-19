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
#include "subtitleoptions.h"
#include "kmflanguagewidgets.h"
#include "kmffontchooser.h"
#include <kurlrequester.h>
#include <kfontdialog.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kdebug.h>
#include <QLabel>

SubtitleOptions::SubtitleOptions(QWidget *parent)
 : KDialog(parent)
{
  setupUi(mainWidget());
  setButtons(KDialog::Ok | KDialog::Cancel);
  setCaption(i18n("Subtitle options"));
  m_languageModel.useAllLanguages();
  languageCombo->setModel(&m_languageModel);
}

SubtitleOptions::~SubtitleOptions()
{
}

void SubtitleOptions::getData(QDVD::Subtitle& obj) const
{
  int align;
  int n = languageCombo->currentIndex();

  obj.setLanguage(m_languageModel.at(n));
  obj.setFile(subtitleUrl->url().prettyUrl());
  obj.setFont(subtitleFontChooser->font());

  align =  0x10 << verticalAlignCombo->currentIndex();
  n = horizontalAlignCombo->currentIndex();
  align |= (n == 0) ? 0 : 0x01 << (n - 1);
  obj.setAlignment(QFlags<Qt::AlignmentFlag>(align));
}

void SubtitleOptions::setData(const QDVD::Subtitle& obj)
{
  int n;

  subtitleUrl->setFilter(
      "*.sub *.srt *.ssa *.smi *.rt *.txt *.aqt *.jss *.js *.ass|" +
      i18n("Subtitle files") +
      "\n*.*|" + i18n("All files"));
  QModelIndex i = m_languageModel.index(obj.language());
  languageCombo->setCurrentIndex(i.row());
  subtitleUrl->setUrl(obj.file());
  subtitleFontChooser->setFont(obj.font());

  n = (obj.alignment() & Qt::AlignVertical_Mask) >> 4;
  verticalAlignCombo->setCurrentIndex((n > 2) ? 2 : n-1);
  n = obj.alignment() & Qt::AlignHorizontal_Mask;
  horizontalAlignCombo->setCurrentIndex((n > 2)?3:n);
}

void SubtitleOptions::slotOk()
{
  QFileInfo fi(subtitleUrl->url().prettyUrl());

  if(fi.exists())
    accept();
  else
    KMessageBox::sorry(kapp->activeWindow(),
                       i18n("Subtitle file does not exists."));
}

#include "subtitleoptions.moc"
