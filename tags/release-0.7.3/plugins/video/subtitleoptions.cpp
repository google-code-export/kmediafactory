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
#include "subtitleoptions.h"
#include "kmflanguagewidgets.h"
#include "kmffontchooser.h"
#include <KUrlRequester>
#include <KFontDialog>
#include <KMessageBox>
#include <KApplication>
#include <KDebug>
#include <KIO/NetAccess>
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
  int hor[] = { 0, Qt::AlignLeft, Qt::AlignRight, Qt::AlignHCenter };
  int ver[] = { Qt::AlignTop, Qt::AlignBottom, Qt::AlignVCenter };

  obj.setLanguage(m_languageModel.at(n));
  obj.setFile(subtitleUrl->url().pathOrUrl());
  obj.setFont(subtitleFontChooser->font());

  align =  ver[verticalAlignCombo->currentIndex()];
  align |= hor[horizontalAlignCombo->currentIndex()];
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

  switch(obj.alignment() & Qt::AlignVertical_Mask)
  {
    case Qt::AlignTop:
      n = 0;
      break;
    case Qt::AlignVCenter:
        n = 2;
        break;
    case Qt::AlignBottom:
    default:
        n = 1;
        break;
  }
  verticalAlignCombo->setCurrentIndex(n);

  switch(obj.alignment() & Qt::AlignHorizontal_Mask)
  {
    case Qt::AlignLeft:
      n = 1;
      break;
    case Qt::AlignRight:
      n = 2;
      break;
    case Qt::AlignHCenter:
      n = 3;
      break;
    default:
      n = 0;
  }
  horizontalAlignCombo->setCurrentIndex(n);
}

void SubtitleOptions::accept()
{
  if(KIO::NetAccess::exists(subtitleUrl->url(), KIO::NetAccess::SourceSide,
     kapp->activeWindow()))
  {
    KDialog::accept();
  }
  else
  {
    KMessageBox::sorry(kapp->activeWindow(),
                       i18n("Subtitle file does not exists."));
  }
}

#include "subtitleoptions.moc"
