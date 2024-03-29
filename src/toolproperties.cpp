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

#include "toolproperties.h"

#include <QtGui/QCheckBox>
#include <QtGui/QLineEdit>

#include <KIconDialog>
#include <KUrlRequester>

ToolProperties::ToolProperties(QWidget *parent)
    : KDialog(parent)
{
    setupUi(mainWidget());
    setButtons(KDialog::Ok | KDialog::Cancel);
    setButtonFocus(KDialog::Ok);
    setCaption(i18n("Tool Properties"));
    workPathUrl->setMode(KFile::Directory | KFile::LocalOnly);
    restoreDialogSize(KConfigGroup(KGlobal::config(), metaObject()->className()));
}

ToolProperties::~ToolProperties()
{
    KConfigGroup cg(KGlobal::config(), metaObject()->className());
    KDialog::saveDialogSize(cg);
}

void ToolProperties::setData(const ToolItem &item)
{
    nameEdit->setText(item.name);
    descriptionEdit->setText(item.description);
    commandUrl->setUrl(item.command);
    workPathUrl->setUrl(item.workPath);
    iconButton->setIcon(item.icon);
    mediaMenuCheckBox->setChecked(item.mediaMenu);
    runInTerminalCheckBox->setChecked(item.runInTerminal);
}

void ToolProperties::getData(ToolItem *item)
{
    item->name = nameEdit->text();
    item->description = descriptionEdit->text();
    item->command = commandUrl->url().prettyUrl();
    item->workPath = workPathUrl->url().prettyUrl();
    item->icon = iconButton->icon();
    item->mediaMenu = mediaMenuCheckBox->isChecked();
    item->runInTerminal = runInTerminalCheckBox->isChecked();
}

void ToolProperties::setReadOnly(bool readonly)
{
    nameEdit->setEnabled(!readonly);
    descriptionEdit->setEnabled(!readonly);
    commandUrl->setEnabled(!readonly);
    workPathUrl->setEnabled(!readonly);
    iconButton->setEnabled(!readonly);
    mediaMenuCheckBox->setEnabled(!readonly);
    runInTerminalCheckBox->setEnabled(!readonly);
}

#include "toolproperties.moc"
