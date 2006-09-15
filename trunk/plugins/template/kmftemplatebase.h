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
#ifndef KMFTEMPLATEBASE_H
#define KMFTEMPLATEBASE_H

#include <qobject.h>

namespace KMF
{
  class ProjectInterface;
  class UiInterface;
  class Plugin;
}
class TemplateObject;
class KMFMenu;
class KMFMenuPage;

/**
*/
class KMFTemplateBase : public QObject
{
    Q_OBJECT
  public:
    KMFTemplateBase(QObject *parent = 0);
    ~KMFTemplateBase();
  protected:
    KMF::UiInterface* m_uiIf;
    KMF::ProjectInterface* m_prjIf;

    KMFMenuPage* page();
    KMFMenu* menu();
    TemplateObject* templateObject();
    KMF::Plugin* plugin();
    QString uiText(QString s);
};

#endif
