//**************************************************************************
//   Copyright (C) 2004, 2005 by Petri Damstén
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
#include <qlabel.h>

SubtitleOptions::SubtitleOptions(QWidget *parent, const char *name)
 : SubtitleOptionsLayout(parent, name)
{
}

SubtitleOptions::~SubtitleOptions()
{
}

void SubtitleOptions::getData(QDVD::Subtitle& obj) const
{
  int align;

  obj.setLanguage(languageCombo->language());
  obj.setFile(subtitleURL->url());
  obj.setFont(subtitleFontChooser->font());

  align =  0x10 << verticalAlignCombo->currentItem();
  int n = horizontalAlignCombo->currentItem();
  align |= (n == 0) ? 0 : 0x01 << (n - 1);
  obj.setAlignment(Qt::AlignmentFlags(align));
}

void SubtitleOptions::setData(const QDVD::Subtitle& obj)
{
  int n;

  subtitleURL->setFilter(
      "*.sub *.srt *.ssa *.smi *.rt *.txt *.aqt *.jss *.js *.ass|" +
      i18n("Subtitle files") +
      "\n*.*|" + i18n("All files"));

  languageCombo->setLanguage(obj.language());
  subtitleURL->setURL(obj.file());
  subtitleFontChooser->setFont(obj.font());

  n = (obj.alignment() & Qt::AlignVertical_Mask) >> 4;
  verticalAlignCombo->setCurrentItem((n > 2) ? 2 : n-1);
  n = obj.alignment() & Qt::AlignHorizontal_Mask;
  horizontalAlignCombo->setCurrentItem((n > 2)?3:n);
}

void SubtitleOptions::okClicked()
{
  QFileInfo fi(subtitleURL->url());

  if(fi.exists())
    accept();
  else
    KMessageBox::sorry(kapp->activeWindow(),
                       i18n("Subtitle file does not exists."));
}

#include "subtitleoptions.moc"
