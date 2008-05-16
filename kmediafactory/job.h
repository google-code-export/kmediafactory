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

#ifndef KMFJOB_H
#define KMFJOB_H

#include "plugininterface.h"
#include <kdemacros.h>
#include <threadweaver/Job.h>

namespace KMF
{
  class KDE_EXPORT Job : public ThreadWeaver::Job
  {
      Q_OBJECT
    public:
      explicit Job(QObject* parent = 0);
      ~Job();
  
      KProcess* process(const QString& filter = "",
                        KProcess::OutputChannelMode mode =
                            KProcess::SeparateChannels);
      void message(KMF::PluginInterface::MsgType type, const QString& msg);
      void log(const QString& msg);
      void setValue(int value);
      void setMaximum(int maximum);

      void setFilter(const QString& filter);
      QString filter() const;

      void failed();
      bool aborted();

      virtual void output(const QString& line);
      virtual bool success() const;
      virtual void requestAbort();

      void TODO_REMOVE_ME_START() { run(); };

    signals:
      void newMessage(PluginInterface::MsgType type, const QString& msg);
      void newLogMessage(const QString& msg);
      // Howto connect these to list items??
      // Main message?
      // ...or on first maximum message connect to list item widget
      void valueChanged(int value);
      void maximumChanged(int maximum);

    private:
      class Private;
      Private *const d;
  };
} // namespace KMF

#endif // KMFJOB_H
