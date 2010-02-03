// **************************************************************************
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
// **************************************************************************

#ifndef KMFPLUGININTERFACE_H
#define KMFPLUGININTERFACE_H

#include <KProgressDialog>

#include <kmediafactory/plugininterface.h>

class KMFProgressDialog : public KMF::ProgressDialog
{
    Q_OBJECT

    public:
        KMFProgressDialog(QWidget *parent);

    public slots:
        virtual void setMaximum(int maximum);
        virtual void setValue(int value);
        virtual void setCaption(const QString &caption);
        virtual void setLabel(const QString &label);
        virtual void showCancelButton(bool show);
        virtual bool wasCancelled();
        virtual void close();

    private:
        KProgressDialog m_pdlg;
};

class KMFPluginInterface : public KMF::PluginInterface
{
    Q_OBJECT

    public:
        KMFPluginInterface(QObject *parent = 0);
        virtual ~KMFPluginInterface();

        virtual QList<KMF::MediaObject *> mediaObjects();
        virtual KMF::TemplateObject*templateObject();
        virtual KMF::OutputObject*outputObject();
        virtual QString title();
        virtual void setTitle(QString title);
        virtual QString projectDir(const QString &subDir = "");
        virtual QString projectType();
        virtual QDVD::VideoTrack::AspectRatio aspectRatio() const;
        virtual void setDirty(KMF::DirtyType type);
        virtual void setModified(KMF::DirtyType type);
        virtual QString lastSubType();
        virtual QDateTime lastModified(KMF::DirtyType type);
        virtual int serial();
        virtual bool addMediaAction(QAction *action, const QString &group = "") const;
        virtual bool addMediaObject(KMF::MediaObject *media) const;
        virtual bool addTemplateObject(KMF::TemplateObject *tob);
        virtual bool addOutputObject(KMF::OutputObject *oob);
        virtual bool removeMediaObject(KMF::MediaObject *media) const;
        virtual bool removeTemplateObject(KMF::TemplateObject *tob);
        virtual bool removeOutputObject(KMF::OutputObject *oob);
        virtual void addMediaObject(const QString &xml);
        virtual void selectTemplate(const QString &xml);
        virtual void selectOutput(const QString &xml);
        virtual void addJob(KMF::Job *job, KMF::JobDependency dependency = KMF::None);
        virtual void addJob(KMF::Job *job, KMF::Job *dependency);

        void setUseMessageBox(bool useMessageBox)
        {
            m_useMessageBox = useMessageBox;
        };
        bool useMessageBox() const
        {
            return m_useMessageBox;
        };
        void setStopped(bool stopped)
        {
            m_stopped = stopped;
        };
        bool stopped() const
        {
            return m_stopped;
        };

        uint jobCount() const
        {
            return m_jobs.count();
        };
        void clearJobs()
        {
            return m_jobs.clear();
        };

        // Plugin helpers
        virtual QStringList getOpenFileNames(const QString &startDir, const QString &filter,
            const QString &caption);
        virtual void debug(const QString &txt);
        virtual int  messageBox(const QString &caption, const QString &txt, int type);
        virtual KMF::ProgressDialog*progressDialog(const QString &caption, const QString &label,
            int maximum);
        virtual KMF::ProgressDialog*progressDialog();

    public slots:
        void progressDialogDestroyed();
        virtual void message(uint id, KMF::MsgType type, const QString &msg = QString());
        virtual void setMaximum(uint id, int maximum);
        virtual void setValue(uint id, int value);
        virtual void log(uint id, const QString &msg);

    private:
        bool m_useMessageBox;
        bool m_stopped;
        KMF::ProgressDialog *m_pdlg;
        QList<KMF::Job *> m_jobs;
};

#endif // KMFPLUGININTERFACE_H
