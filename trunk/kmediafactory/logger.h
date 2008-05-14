//**************************************************************************
//   Copyright (C) 2004-2008 by Petri Damsten
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

#ifndef KMFLOGGER_H
#define KMFLOGGER_H

#include <kdemacros.h>
#include <KProcess>
#include <QObject>
#include <QColor>

/*
  class KDE_EXPORT Logger :public QObject
  {
      Q_OBJECT
    public:
      explicit Logger(QObject* parent);
      ~Logger();

      virtual void stop() = 0;
      virtual void start() = 0;
      virtual const QString& log() const = 0;
      virtual void message(const QString& msg, const QColor& color) = 0;
      // TODO move to job
      virtual void setFilter(const QString& filter) = 0;
      // TODO move to job
      virtual QString filter() const = 0;
      virtual bool save(QString file) const = 0;
      // TODO move to job
      void connectProcess(KProcess* proc,
                          const QString& filter = "",
                          KProcess::OutputChannelMode mode =
                              KProcess::SeparateChannels);

    public slots:
      // TODO move to job
      virtual void stdout() = 0;
      // TODO move to job
      virtual void stderr() = 0;
      void message(const QString& msg);

    protected:
      // TODO move to job
      KProcess* currentProcess();

    private:
      class Private;
      Private *const d;
  };
*/
namespace KMF
{

class KDE_EXPORT Logger : public QObject
{
    Q_OBJECT
  public:
    explicit Logger(QObject *parent = 0);
    ~Logger();

    virtual void start();
    virtual void stop();
    virtual const QString& log() const;
    virtual void message(const QString& msg, const QColor& color);
    // TODO move to job
    virtual void setFilter(const QString& filter);
    // TODO move to job
    virtual QString filter() const;
    virtual bool save(QString file) const;
    // TODO move to job
    void connectProcess(KProcess* proc,
                        const QString& filter = "",
                        KProcess::OutputChannelMode mode =
                            KProcess::SeparateChannels);

  signals:
    void line(QString line);

  public slots:
    // TODO move to job
    virtual void stdout();
    // TODO move to job
    virtual void stderr();
    void message(const QString& msg);

  protected:
    // TODO move to job
    KProcess* currentProcess();

  private:
    class Private;
    Private *const d;
};

} // namespace KMF

#endif // KMFLOGGER_H
