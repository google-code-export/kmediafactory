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
      void message(KMF::MsgType type, const QString& txt);
      void log(const QString& msg, const QString& txt);
      void setValue(int value, const QString& txt);
      void setMaximum(int maximum, const QString& txt);

      void setFilter(const QString& filter);
      QString filter() const;

      void failed();
      bool aborted();

      virtual void output(const QString& line);
      virtual bool success() const;
      virtual void requestAbort();

    signals:
      void newMessage(KMF::MsgType type, const QString& txt);
      void newLogMessage(const QString& msg, const QString& txt);
      void valueChanged(int value, const QString& txt);
      void maximumChanged(int maximum, const QString& txt);

    private:
      class Private;
      Private *const d;
  };
} // namespace KMF

#endif // KMFJOB_H
