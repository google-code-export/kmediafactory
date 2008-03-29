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
#include "toolproperties.h"
#include <kurlrequester.h>
#include <kicondialog.h>
#include <qlineedit.h>
#include <qcheckbox.h>

ToolProperties::ToolProperties(QWidget *parent, const char *name)
    :ToolPropertiesLayout(parent, name)
{
  workPathURL->setMode(KFile::Directory | KFile::LocalOnly);
}

void ToolProperties::setData(const QToolListItem& item)
{
  nameEdit->setText(item.name);
  descriptionEdit->setText(item.description);
  commandURL->setURL(item.command);
  workPathURL->setURL(item.workPath);
  iconButton->setIcon(item.icon);
  mediaMenuCheckBox->setChecked(item.mediaMenu);
  runInTerminalCheckBox->setChecked(item.runInTerminal);
}

void ToolProperties::getData(QToolListItem* item)
{
  item->name = nameEdit->text();
  item->description = descriptionEdit->text();
  item->command = commandURL->url();
  item->workPath = workPathURL->url();
  item->icon = iconButton->icon();
  item->mediaMenu = mediaMenuCheckBox->isChecked();
  item->runInTerminal = runInTerminalCheckBox->isChecked();
}

void ToolProperties::setReadOnly(bool readonly)
{
  nameEdit->setEnabled(!readonly);
  descriptionEdit->setEnabled(!readonly);
  commandURL->setEnabled(!readonly);
  workPathURL->setEnabled(!readonly);
  iconButton->setEnabled(!readonly);
  mediaMenuCheckBox->setEnabled(!readonly);
  runInTerminalCheckBox->setEnabled(!readonly);
}


#include "toolproperties.moc"
