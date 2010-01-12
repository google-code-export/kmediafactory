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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//**************************************************************************

#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include "object.h"
#include "job.h"
#include <KProcess>
#include <kdemacros.h>
#include <QDateTime>
#include <stdint.h>
#include <qdvdinfo.h>

class QDomElement;

namespace KMF
{
  class KDE_EXPORT MediaObject :public Object
  {
      Q_OBJECT
    public:
      enum { MainPreview = 0 };
      enum { MainTitle = 0 };

      explicit MediaObject(QObject* parent);
      ~MediaObject();

    public slots:
      // Help Kross plugin declaring these as slots
      virtual QImage preview(int chapter = MainPreview) const = 0;
      virtual QString text(int chapter = MainTitle) const = 0;
      virtual int chapters() const = 0;
      virtual uint64_t size() const = 0;
      virtual QTime duration() const = 0;
      virtual QTime chapterTime(int chapter) const = 0;
      virtual int chapterId(int chapter) const { return chapter; }
  };

  class KDE_EXPORT OutputObject :public Object
  {
      Q_OBJECT
    public:
      explicit OutputObject(QObject* parent);
      ~OutputObject();
  };

  class KDE_EXPORT TemplateObject :public Object
  {
      Q_OBJECT
    public:
      explicit TemplateObject(QObject* parent);
      ~TemplateObject();

      virtual bool clicked();
      virtual QStringList menus();

    public slots:
      // Help Kross plugin declaring these as slots
      virtual QImage preview(const QString& = "");
  };

  class KDE_EXPORT ProgressDialog : public QObject
  {
      Q_OBJECT
    public:
      explicit ProgressDialog(QObject* parent);
      ~ProgressDialog();

    public slots:
      virtual void setMaximum(int maximum) = 0;
      virtual void setValue(int value) = 0;
      virtual void setCaption(const QString &caption) = 0;
      virtual void setLabel(const QString &label) = 0;
      virtual void showCancelButton(bool show) = 0;
      virtual bool wasCancelled() = 0;
      virtual void close() = 0;
  };

  class KDE_EXPORT PluginInterface :public QObject
  {
      Q_OBJECT
    public:
      explicit PluginInterface(QObject* parent);
      ~PluginInterface();

      virtual QList<MediaObject*> mediaObjects() = 0;
      virtual TemplateObject* templateObject() = 0;
      virtual OutputObject* outputObject() = 0;
      virtual QString title() = 0;
      virtual void setTitle(QString title) = 0;
      virtual QString projectDir(const QString& subDir = "") = 0;
      virtual QString projectType() = 0;
      virtual void setDirty(DirtyType type) = 0;
      virtual void setModified(DirtyType type) = 0;
      virtual QString lastSubType() = 0;
      virtual QDateTime lastModified(DirtyType type) = 0;
      virtual int serial() = 0;
      virtual bool addMediaAction(QAction* action,const QString& group = "") const = 0;
      virtual QDVD::VideoTrack::AspectRatio aspectRatio() const { return QDVD::VideoTrack::Aspect_4_3; }
      virtual bool addMediaObject(MediaObject* media) const = 0;
      virtual bool addTemplateObject(TemplateObject* tob) = 0;
      virtual bool addOutputObject(OutputObject* oob) = 0;
      virtual bool removeMediaObject(MediaObject* media) const = 0;
      virtual bool removeTemplateObject(TemplateObject* tob) = 0;
      virtual bool removeOutputObject(OutputObject* oob) = 0;
      virtual void addMediaObject(const QString& xml) = 0;
      virtual void selectTemplate(const QString& xml) = 0;
      virtual void selectOutput(const QString& xml) = 0;

      virtual void addJob(KMF::Job *job, JobDependency dependency = KMF::None) = 0;
      virtual void addJob(KMF::Job *job, KMF::Job *dependency) = 0;

      static uint messageId();
      virtual void message(uint id, MsgType type, const QString& msg = QString()) = 0;
      virtual void setMaximum(uint id, int maximum) = 0;
      virtual void setValue(uint id, int value) = 0;
      virtual void log(uint id, const QString& msg) = 0;
      // sub id hack
      static uint subId(uint id);
      static uint parent(uint id);

      // Plugin helpers
      virtual QStringList getOpenFileNames(const QString &startDir,
                                           const QString &filter,
                                           const QString &caption) = 0;
      virtual void debug(const QString &txt) = 0;
      virtual int  messageBox(const QString &caption, const QString &txt,
                              int type) = 0;
      virtual ProgressDialog* progressDialog(const QString &caption, const QString &label,
                                             int maximum) = 0;
      virtual ProgressDialog* progressDialog() = 0;
  };
}

Q_DECLARE_METATYPE(KMF::OutputObject*)
Q_DECLARE_METATYPE(KMF::TemplateObject*)
Q_DECLARE_METATYPE(KMF::MediaObject*)

#endif // PLUGININTERFACE_H
