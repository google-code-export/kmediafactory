//**************************************************************************
//   Copyright (C) 2008 by Petri Damsten
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

#ifndef KMFJOB_P_H
#define KMFJOB_P_H

#include "plugininterface.h"

// http://api.kde.org/4.0-api/kdelibs-apidocs/threadweaver/html/MainComponents.html
namespace KMF
{
  class JobHelper : public QObject
  {
      Q_OBJECT
    public:
      explicit JobHelper(KMF::Job* parent);
      ~JobHelper();

      void message(KMF::PluginInterface::MsgType type, const QString& msg);
      void log(const QString& msg);
      void setValue(int value);
      void setMaximum(int maximum);

    signals:
      void newMessage(PluginInterface::MsgType type, const QString& msg);
      void newLogMessage(const QString& msg);
      void valueChanged(int value);
      void maximumChanged(int maximum);
  };
} // namespace KMF

#endif // KMFJOB_P_H
