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
#ifndef KMFWIDGETFACTORY_H
#define KMFWIDGETFACTORY_H

#include <kmftemplatebase.h>
#include <templateobject.h>
#include <q3dict.h>

class QDomElement;
class KMFWidget;
class KConfigXML;

/**
@author Petri Damsten
*/
class KMFWidgetFactory : public KMFTemplateBase
{
    Q_OBJECT
  public:
    KMFWidgetFactory(QObject *parent = 0);
    ~KMFWidgetFactory();

    KMFWidget* create(const QDomElement& element, QObject* parent = 0);
    static KMFWidget* createPage(const QDomElement& element,
                                 QObject* parent = 0,
                                 int title = 0, int chapter = 0);

  private:
    KMFWidget* newWidget(QString type,
                                           QObject* parent = 0);
    int m_title;
    int m_chapter;
    const KConfigXML* m_customProperties;
};

#endif
