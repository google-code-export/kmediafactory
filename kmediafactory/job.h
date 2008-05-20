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

#include "kmediafactory.h"
#include <KProcess>
#include <kdemacros.h>
#include <threadweaver/Job.h>

#define CHECK_IF_ABORTED(result) { \
if (aborted()) { return result; }; \
}

namespace KMF
{
  class KDE_EXPORT Job : public ThreadWeaver::Job
  {
      Q_OBJECT
    public:
      explicit Job(QObject* parent = 0);
      ~Job();
  
      KProcess* process(uint id, const QString& filter = "", KProcess::OutputChannelMode mode =
                        KProcess::SeparateChannels);
      uint msgId();
      void message(uint id, KMF::MsgType type, const QString& msg = QString());
      void log(uint id, const QString& msg);
      void setValue(uint id, int value);
      void setMaximum(uint id, int maximum);

      void setFilter(const QString& filter);
      QString filter() const;

      void failed();
      bool aborted() const;

      virtual void output(const QString& line);
      virtual bool success() const;
      virtual void requestAbort();

    signals:
      void newMessage(uint id, KMF::MsgType type, const QString& msg);
      void newLogMessage(uint id, const QString& msg);
      void valueChanged(uint id, int value);
      void maximumChanged(uint id, int maximum);

    private:
      class Private;
      Private* d;
      // Create d on demand (when executing thread) because of thread issues
      Private* d_func();
  };
} // namespace KMF

#endif // KMFJOB_H
