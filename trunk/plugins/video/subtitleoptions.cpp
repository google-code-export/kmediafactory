// **************************************************************************
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
// **************************************************************************

#include "subtitleoptions.h"

#include <QtGui/QLabel>

#include <KApplication>
#include <KDebug>
#include <KFontDialog>
#include <KFontRequester>
#include <KMessageBox>
#include <KUrlRequester>
#include <KIO/NetAccess>

#include <kmflanguagewidgets.h>

SubtitleOptionsWidget::SubtitleOptionsWidget(QWidget *parent, bool wantFile) : QWidget(parent)
{
    setupUi(this);
    m_languageModel.useAllLanguages();
    languageCombo->setModel(&m_languageModel);
    encodingCombo->model()->sort(0);

    if (!wantFile) {
        delete subtitleUrl;
        subtitleUrl = 0L;
        delete subtitleFileLabel;
        subtitleFileLabel = 0L;
    }
}

SubtitleOptionsWidget::~SubtitleOptionsWidget()
{
}

void SubtitleOptionsWidget::getData(QDVD::Subtitle &obj) const
{
    int align;
    int n = languageCombo->currentIndex();
    int hor[] = { 0, Qt::AlignLeft, Qt::AlignRight, Qt::AlignHCenter };
    int ver[] = { Qt::AlignTop, Qt::AlignBottom, Qt::AlignVCenter };

    obj.setLanguage(m_languageModel.at(n));

    if (subtitleUrl) {
        obj.setFile(subtitleUrl->url().pathOrUrl());
    }

    obj.setFont(subtitleFont->font());
    obj.setEncoding(encodingCombo->currentText());

    align =  ver[verticalAlignCombo->currentIndex()];
    align |= hor[horizontalAlignCombo->currentIndex()];
    obj.setAlignment(QFlags<Qt::AlignmentFlag>(align));
}

void SubtitleOptionsWidget::setData(const QDVD::Subtitle &obj)
{
    int n;

    QModelIndex i = m_languageModel.index(obj.language());

    languageCombo->setCurrentIndex(i.row());

    if (subtitleUrl) {
        subtitleUrl->setFilter(
                "*.sub *.srt *.ssa *.smi *.rt *.txt *.aqt *.jss *.js *.ass|" +
                i18n("Subtitle files") +
                "\n*.*|" + i18n("All files"));
        subtitleUrl->setUrl(obj.file());
    }

    subtitleFont->setFont(obj.font());
    encodingCombo->setCurrentIndex(encodingCombo->findText(obj.encoding()));

    switch (obj.alignment() & Qt::AlignVertical_Mask) {
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

    switch (obj.alignment() & Qt::AlignHorizontal_Mask) {
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

SubtitleOptions::SubtitleOptions(QWidget *parent)
: KDialog(parent)
{
    setMainWidget(m_widget = new SubtitleOptionsWidget(this, true));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setCaption(i18n("Subtitle Options"));
    restoreDialogSize(KConfigGroup(KGlobal::config(), metaObject()->className()));
}

SubtitleOptions::~SubtitleOptions()
{
    KConfigGroup cg(KGlobal::config(), metaObject()->className());
    KDialog::saveDialogSize(cg);
}

void SubtitleOptions::accept()
{
    if (KIO::NetAccess::exists(m_widget->url(), KIO::NetAccess::SourceSide, kapp->activeWindow())) {
        KDialog::accept();
    } else   {
        KMessageBox::sorry(kapp->activeWindow(), i18n("Subtitle file does not exists."));
    }
}

#include "subtitleoptions.moc"
