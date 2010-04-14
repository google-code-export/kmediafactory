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

#include "kmfplugininterface.h"

#include <QtXml/QDomDocument>

#include <KDebug>
#include <KFileDialog>
#include <KMessageBox>
#include <ThreadWeaver/DependencyPolicy>
#include <ThreadWeaver/Weaver>

#include "kmediafactory.h"
#include "kmfapplication.h"
#include "kmfprogressitemdelegate.h"
#include "kmfproject.h"
#include "mediapage.h"
#include "outputpage.h"
#include "templatepage.h"

KMFProgressDialog::KMFProgressDialog(QWidget *parent)
    : KMF::ProgressDialog(parent)
    , m_pdlg(parent)
{
    m_pdlg.setAutoClose(false);
}

void KMFProgressDialog::setMaximum(int maximum)
{
    m_pdlg.progressBar()->setMaximum(maximum);
}

void KMFProgressDialog::setValue(int value)
{
    m_pdlg.progressBar()->setValue(value);
}

void KMFProgressDialog::setCaption(const QString &caption)
{
    m_pdlg.setCaption(caption);
}

void KMFProgressDialog::setLabel(const QString &label)
{
    m_pdlg.setLabelText(label);
}

void KMFProgressDialog::showCancelButton(bool show)
{
    m_pdlg.showCancelButton(show);
}

bool KMFProgressDialog::wasCancelled()
{
    return m_pdlg.wasCancelled();
}

void KMFProgressDialog::close()
{
    m_pdlg.close();
    deleteLater();
}

KMFPluginInterface::KMFPluginInterface(QObject *parent)
    : KMF::PluginInterface(parent)
    , m_useMessageBox(false)
    , m_pdlg(0)
{
}

KMFPluginInterface::~KMFPluginInterface()
{
}

bool KMFPluginInterface::addMediaAction(QAction *action, const QString &group) const
{
    kmfApp->mainWindow()->mediaPage->mediaButtons->add(action, group);
    return true;
}

bool KMFPluginInterface::addMediaObject(KMF::MediaObject *media) const
{
    KMFProject *project = kmfApp->project();

    return project ? project->addItem(media) : false;
}

bool KMFPluginInterface::addTemplateObject(KMF::TemplateObject *tob)
{
    KMFProject *project = kmfApp->project();

    if (project) {
        project->templateObjects()->append(tob);
    }

    return true;
}

bool KMFPluginInterface::addOutputObject(KMF::OutputObject *oob)
{
    KMFProject *project = kmfApp->project();

    if (project) {
        project->outputObjects()->append(oob);
    }

    return true;
}

bool KMFPluginInterface::removeMediaObject(KMF::MediaObject *media) const
{
    KMFProject *project = kmfApp->project();

    if (project) {
        project->removeItem(media);
    }

    return true;
}

bool KMFPluginInterface::removeTemplateObject(KMF::TemplateObject *tob)
{
    KMFProject *project = kmfApp->project();

    if (project) {
        project->templateObjects()->removeAll(tob);
    }

    return true;
}

bool KMFPluginInterface::removeOutputObject(KMF::OutputObject *oob)
{
    KMFProject *project = kmfApp->project();

    if (project) {
        project->outputObjects()->removeAll(oob);
    }

    return true;
}

void KMFPluginInterface::addMediaObject(const QString &xml)
{
    kDebug() << xml;
    QDomDocument doc;
    doc.setContent(xml);
    kmfApp->project()->mediaObjFromXML(doc.documentElement());
}

void KMFPluginInterface::selectTemplate(const QString &xml)
{
    QDomDocument doc;

    doc.setContent(xml);
    kmfApp->project()->templateFromXML(doc.documentElement());
}

void KMFPluginInterface::selectOutput(const QString &xml)
{
    QDomDocument doc;

    doc.setContent(xml);
    kmfApp->project()->outputFromXML(doc.documentElement());
}

bool KMFPluginInterface::executableJobsRemaining()
{
    foreach (KMF::Job* job, m_jobs) {
        if (!job->isFinished() && job->canBeExecuted()) {
            return true;
        }
    }
    return false;
}

void KMFPluginInterface::addJob(KMF::Job *job, KMF::JobDependency dependency)
{
    switch (dependency) {
        case KMF::All:
            foreach (KMF::Job * dep, m_jobs) {
                ThreadWeaver::DependencyPolicy::instance().addDependency(job, dep);
            }
            break;

        case KMF::Last:
            if (!m_jobs.isEmpty() && (m_jobs.last() != 0)) {
                ThreadWeaver::DependencyPolicy::instance().addDependency(job, m_jobs.last());
            }
            break;

        case KMF::None:
        default:
            break;
    }

    m_jobs.append(job);

    connect(job, SIGNAL(newMessage(uint, KMF::MsgType, const QString &)),
            this, SLOT(message(uint, KMF::MsgType, const QString &)));
    connect(job, SIGNAL(newLogMessage(uint, const QString &)),
            this, SLOT(log(uint, const QString &)));
    connect(job, SIGNAL(maximumChanged(uint, int)),
            this, SLOT(setMaximum(uint, int)));
    connect(job, SIGNAL(valueChanged(uint, int)),
            this, SLOT(setValue(uint, int)));
    ThreadWeaver::Weaver::instance()->enqueue(job);
}

void KMFPluginInterface::addJob(KMF::Job *job, KMF::Job *dependency)
{
    ThreadWeaver::DependencyPolicy::instance().addDependency(job, dependency);
    addJob(job);
}

void KMFPluginInterface::message(uint id, KMF::MsgType type, const QString &msg)
{
    kmfApp->mainWindow()->outputPage->message(id, type, msg);
}

void KMFPluginInterface::setMaximum(uint id, int max)
{
    kmfApp->mainWindow()->outputPage->setMaximum(id, max);
}

void KMFPluginInterface::setValue(uint id, int value)
{
    kmfApp->mainWindow()->outputPage->setValue(id, value);
}

void KMFPluginInterface::log(uint id, const QString &msg)
{
    kmfApp->mainWindow()->outputPage->log(id, msg);
}

void KMFPluginInterface::progressDialogDestroyed()
{
    m_pdlg = 0;
}

KMF::ProgressDialog *KMFPluginInterface::progressDialog(const QString &caption,
        const QString &label, int maximum)
{
    if (m_pdlg) {
        delete m_pdlg;
    }

    m_pdlg = new KMFProgressDialog(kmfApp->mainWindow());
    connect(m_pdlg, SIGNAL(destroyed()), this, SLOT(progressDialogDestroyed()));
    m_pdlg->setCaption(caption);
    m_pdlg->setLabel(label);
    m_pdlg->setMaximum(maximum);
    m_pdlg->showCancelButton(true);
    return m_pdlg;
}

KMF::ProgressDialog *KMFPluginInterface::progressDialog()
{
    if (m_pdlg) {
        return m_pdlg;
    }

    return progressDialog("KMediaFactory", i18n("Progress:"), 100);
}

int KMFPluginInterface::messageBox(const QString &caption, const QString &txt, int type)
{
    kDebug() << caption << txt << type << KMessageBox::Error;

    if (type == KMessageBox::Information) {
        KMessageBox::information(kmfApp->mainWindow(), txt, caption);
    } else if (type == KMessageBox::Sorry) {
        KMessageBox::sorry(kmfApp->mainWindow(), txt, caption);
    } else if (type == KMessageBox::Error) {
        KMessageBox::error(kmfApp->mainWindow(), txt, caption);
    }

    return 0;
}

QStringList KMFPluginInterface::getOpenFileNames(const QString &startDir, const QString &filter,
        const QString &caption)
{
    QString start = QString("kfiledialog:///<%1>").arg(startDir);

    // kDebug() << start;
    return KFileDialog::getOpenFileNames(KUrl(start), filter,
            kmfApp->mainWindow(), caption);
}

void KMFPluginInterface::debug(const QString &txt)
{
    kDebug() << txt;
}

QString KMFPluginInterface::title()
{
    if (kmfApp->project()) {
        return kmfApp->project()->title();
    }

    return QString();
}

void KMFPluginInterface::setTitle(QString title)
{
    if (kmfApp->project()) {
        kmfApp->project()->setTitle(title);
    }
}

QList<KMF::MediaObject *> KMFPluginInterface::mediaObjects()
{
    if (kmfApp->project()) {
        return kmfApp->project()->mediaObjects()->list();
    }

    return QList<KMF::MediaObject *>();
}

KMF::TemplateObject *KMFPluginInterface::templateObject()
{
    if (kmfApp->project()) {
        return kmfApp->project()->templateObj();
    }

    return 0;
}

KMF::OutputObject *KMFPluginInterface::outputObject()
{
    if (kmfApp->project()) {
        return kmfApp->project()->output();
    }

    return 0;
}

QString KMFPluginInterface::projectDir(const QString &subDir)
{
    if (kmfApp->project()) {
        return kmfApp->project()->directory(subDir);
    }

    return QString();
}

void KMFPluginInterface::setDirty(KMF::DirtyType type)
{
    if (kmfApp->project()) {
        kmfApp->project()->setDirty(type);
    }
}

void KMFPluginInterface::setModified(KMF::DirtyType type)
{
    if (kmfApp->project()) {
        kmfApp->project()->setModified(type);
    }
}

QString KMFPluginInterface::projectType()
{
    if (kmfApp->project()) {
        return kmfApp->project()->type();
    }

    return QString();
}

QDVD::VideoTrack::AspectRatio KMFPluginInterface::aspectRatio() const
{
    if (kmfApp->project()) {
        return kmfApp->project()->aspectRatio();
    }

    return QDVD::VideoTrack::Aspect_4_3;
}

QString KMFPluginInterface::lastSubType()
{
    if (kmfApp->project()) {
        return kmfApp->project()->lastSubType();
    }

    return QString();
}

QDateTime KMFPluginInterface::lastModified(KMF::DirtyType type)
{
    if (kmfApp->project()) {
        return kmfApp->project()->lastModified(type);
    }

    return QDateTime();
}

int KMFPluginInterface::serial()
{
    if (kmfApp->project()) {
        return kmfApp->project()->serial();
    }

    return -1;
}

#include "kmfplugininterface.moc"
