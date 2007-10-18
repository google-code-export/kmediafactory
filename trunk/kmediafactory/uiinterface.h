//**************************************************************************
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
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************
#ifndef UIINTERFACE_H
#define UIINTERFACE_H

#include "kmfobject.h"
#include <kaction.h>
#include <kprocess.h>
#include <QString>
#include <QObject>
#include <QImage>
#include <stdint.h>

class QToolButton;
class QDomElement;

namespace KMF
{
  enum MsgType { None = -1, Info, Warning, Error, OK };
  enum AspectRatio { ASPECT_4_3 = 0 , ASPECT_16_9, ASPECT_LAST };

  class KDE_EXPORT MediaObject :public Object
  {
      Q_OBJECT
    public:
      enum { MainPreview = 0 };
      enum { MainTitle = 0 };

      MediaObject(QObject* parent);
      virtual void writeDvdAuthorXml(QDomElement& element,
                                     QString preferredLanguage,
                                     QString post, QString type) = 0;
      virtual QImage preview(int chapter = MainPreview) const = 0;
      virtual QString text(int chapter = MainTitle) const = 0;
      virtual int chapters() const = 0;
      virtual uint64_t size() const = 0;
      virtual QTime duration() const = 0;
      virtual QTime chapterTime(int chapter) const = 0;
  };

  class KDE_EXPORT OutputObject :public Object
  {
      Q_OBJECT
    public:
      OutputObject(QObject* parent);
  };

  class KDE_EXPORT TemplateObject :public Object
  {
      Q_OBJECT
    public:
      TemplateObject(QObject* parent);
      virtual QImage preview(const QString& = "")
          { return QImage(); };
      virtual QStringList menus() { return QStringList(); };
      virtual bool clicked() { return false; };
  };

  class KDE_EXPORT Logger :public QObject
  {
      Q_OBJECT
    public:
      virtual void stop() = 0;
      virtual void start() = 0;
      virtual const QString& log() const = 0;
      virtual void message(const QString& msg, const QColor& color) = 0;
      virtual void setFilter(const QString& filter) = 0;
      virtual QString filter() const = 0;
      virtual bool save(QString file) const = 0;
      void connectProcess(KProcess* proc,
                          const QString& filter = "",
                          KProcess::OutputChannelMode mode =
                              KProcess::SeparateChannels);

    public slots:
      virtual void stdout(KProcess* proc, char* buffer, int buflen) = 0;
      virtual void stderr(KProcess* proc, char* buffer, int buflen) = 0;
      void message(const QString& msg) { message(msg, QColor("black")); };
  };

  class KDE_EXPORT UiInterface :public QObject
  {
      Q_OBJECT
    public:
      UiInterface(QObject* parent);
      virtual bool addMediaAction(QAction* action,
                                  const QString& group = "") const = 0;
      virtual bool addMediaObject(MediaObject* media) const = 0;
      virtual bool addTemplateObject(TemplateObject* tob) = 0;
      virtual bool addOutputObject(OutputObject* oob) = 0;
      virtual bool removeMediaObject(MediaObject* media) const = 0;
      virtual bool removeTemplateObject(TemplateObject* tob) = 0;
      virtual bool removeOutputObject(OutputObject* oob) = 0;

      virtual bool message(MsgType type, const QString& msg) = 0;
      virtual bool progress(int advance) = 0;
      virtual bool setItemTotalSteps(int totalSteps) = 0;
      virtual bool setItemProgress(int progress) = 0;
      virtual Logger* logger() = 0;
  };
};

Q_DECLARE_METATYPE(KMF::OutputObject*);
Q_DECLARE_METATYPE(KMF::TemplateObject*);
Q_DECLARE_METATYPE(KMF::MediaObject*);

#endif // UIINTERFACE_H

