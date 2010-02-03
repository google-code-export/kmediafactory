// **************************************************************************
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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
// **************************************************************************

#include "kmediafactory.h"

#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtCore/QTimer>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QLabel>
#include <QtGui/QStandardItemModel>
#include <QtGui/QToolButton>

#include <KActionCollection>
#include <KCmdLineArgs>
#include <KConfig>
#include <KConfigDialog>
#include <KDebug>
#include <KDesktopFile>
#include <KDialog>
#include <KEditToolBar>
#include <KFileDialog>
#include <KIcon>
#include <KIconLoader>
#include <KLocale>
#include <KMainWindow>
#include <KMenuBar>
#include <KMessageBox>
#include <KPageWidget>
#include <KPushButton>
#include <KRun>
#include <KService>
#include <KShortcutsDialog>
#include <KStandardAction>
#include <KStandardDirs>
#include <KStatusBar>
#include <KToggleAction>
#include <KToolBar>
#include <KXMLGUIFactory>
#include <KIO/NetAccess>
#if KDE_IS_VERSION(4, 3, 90)
#include <KNS3/DownloadDialog>
#else
#include <KNS/Engine>
#endif

#include <kmediafactorysettings.h>
#include <kmfimageview.h>
#include <kmftools.h>
#include <run.h>
#include <kmediafactory/plugin.h>
#include "kmfapplication.h"
#include "kmfoptions.h"
#include "kmfplugininterface.h"
#include "kmfproject.h"
#include "mediapage.h"
#include "outputpage.h"
#include "projectoptions.h"
#include "projectoptions.h"
#include "sizewidget.h"
#include "templatepage.h"
#include "tools.h"

KMediaFactory::KMediaFactory()
    : KXmlGuiWindow(0)
    , m_janusIconList(0)
    , m_enabled(true)
    , m_newStuffDlg(0)
{
    // set the shell's ui resource file
    setXMLFile("kmediafactoryui.rc");

    // Central widget
    QWidget *main = new QWidget;
    QVBoxLayout *vbox = new QVBoxLayout;
    m_janus = new KPageWidget;
    m_janus->setFaceType(KPageView::List);
    vbox->setMargin(KDialog::marginHint());
    vbox->addWidget(m_janus);
    main->setLayout(vbox);
    setCentralWidget(main);
    const QObjectList &list = m_janus->children();

    for (int i = 0; i < list.size(); ++i) {
        QWidget *w = static_cast<QWidget *>(list[i]);

        if (qstrcmp(w->metaObject()->className(), "KDEPrivate::KPageListView") == 0) {
            m_janusIconList = w;
        }
    }

    // Pages
    // Project

    projectPage = new ProjectOptions;
    m_projectPageItem = new KPageWidgetItem(projectPage, i18n("Project"));
    m_projectPageItem->setHeader(i18n("Project Settings"));
    m_projectPageItem->setIcon(KIcon("kmediafactory"));
    m_janus->addPage(m_projectPageItem);

    // Media

    mediaPage = new MediaPage;
    m_mediaPageItem = new KPageWidgetItem(mediaPage, i18n("Media"));
    m_mediaPageItem->setHeader(i18n("Add Media To Project"));
    m_mediaPageItem->setIcon(KIcon("folder-video"));
    m_janus->addPage(m_mediaPageItem);

    // Template
    templatePage = new TemplatePage;
    m_templatePageItem = new KPageWidgetItem(templatePage, i18n("Template"));
    m_templatePageItem->setHeader(i18n("Select Project Template"));
    m_templatePageItem->setIcon(KIcon("folder-image"));
    m_janus->addPage(m_templatePageItem);
    connect(m_janus,
            SIGNAL(currentPageChanged(KPageWidgetItem *, KPageWidgetItem *)),
            templatePage,
            SLOT(currentPageChanged(KPageWidgetItem *, KPageWidgetItem *)));

    // Output
    outputPage = new OutputPage;
    m_ouputPageItem = new KPageWidgetItem(outputPage, i18n("Output"));
    m_ouputPageItem->setHeader(i18n("Choose Desired Output"));
    m_ouputPageItem->setIcon(KIcon("media-optical"));
    m_janus->addPage(m_ouputPageItem);
    connect(m_janus,
            SIGNAL(currentPageChanged(KPageWidgetItem *, KPageWidgetItem *)),
            outputPage,
            SLOT(currentPageChanged(KPageWidgetItem *, KPageWidgetItem *)));

    // then, setup our actions
    setupActions();

    // apply the saved mainwindow settings, if any, and ask the mainwindow
    // to automatically save settings if changed: window size, toolbar
    // position, icon size, etc.
    setAutoSaveSettings();

    // Do some things later
    // QTimer::singleShot(0, this, SLOT(initGUI()));

    KStatusBar *sb = statusBar();
    QLabel     *details = new QLabel(this);
    SizeWidget *sw = new SizeWidget(this);

    connect(mediaPage, SIGNAL(details(const QString &)), details, SLOT(setText(const QString &)));
    connect(mediaPage, SIGNAL(sizes(quint64, quint64)), sw, SLOT(setSizes(quint64, quint64)));
    sb->addPermanentWidget(details);
    sb->addPermanentWidget(sw);
}

KMediaFactory::~KMediaFactory()
{
    // delete m_newStuffDlg;
    // delete mediaPage;
    // delete templatePage;
    // delete outputPage;
    // delete pluginInterface;
    // delete mediaInterface;
}

void KMediaFactory::setupActions()
{
    kDebug();
    QAction *action;

    // File
    KStandardAction::openNew(this, SLOT(fileNew()), actionCollection());
    KStandardAction::open(this, SLOT(fileOpen()), actionCollection());
    KStandardAction::save(this, SLOT(fileSave()), actionCollection());
    KStandardAction::saveAs(this, SLOT(fileSaveAs()), actionCollection());
    KStandardAction::quit(this, SLOT(quit()), actionCollection());

    // KNewStuff
    action = new KAction(KIcon("get-hot-new-stuff"), i18n("Get New Tools"), this);
    actionCollection()->addAction("newstuff", action);
    connect(action, SIGNAL(triggered()), SLOT(newStuff()));

    // Settings
    KStandardAction::keyBindings(this, SLOT(optionsConfigureKeys()),
            actionCollection());
    KStandardAction::configureToolbars(this, SLOT(optionsConfigureToolbars()),
            actionCollection());
    KStandardAction::preferences(this, SLOT(optionsPreferences()),
            actionCollection());

    setStandardToolBarMenuEnabled(true);

    // Media file menu
    action = new KAction(KIcon("edit-delete"), i18n("Delete"), this);
    action->setShortcut(Qt::Key_Delete);
    actionCollection()->addAction("delete", action);
    connect(action, SIGNAL(triggered()), SLOT(itemDelete()));

    createGUI("kmediafactoryui.rc");

    QStringList dirs = KGlobal::dirs()->findDirs("data", "");

    for (QStringList::ConstIterator it = dirs.begin(); it != dirs.end(); ++it) {
        kDebug() << "Watching: " <<
        *it + "kmediafactory/tools";
        m_toolsWatch.addDir(*it + "kmediafactory/tools");
    }

    connect(&m_toolsWatch, SIGNAL(dirty(QString)), this, SLOT(updateToolsMenu()));
    updateToolsMenu();
}

void KMediaFactory::connectProject()
{
    kDebug();
    kmfApp->project()->disconnect();

    const KMF::PluginList list = kmfApp->plugins();

    for (KMF::PluginList::ConstIterator obj = list.begin();
         obj != list.end(); ++obj)
    {
        connect(kmfApp->project(), SIGNAL(preinit(const QString &)),
                *obj, SLOT(init(const QString &)));
    }

    connect(kmfApp->project(), SIGNAL(modified(const QString &, bool)),
            this, SLOT(setCaption(const QString &, bool)));

    connect(kmfApp->project(), SIGNAL(init(const QString &)),
            mediaPage, SLOT(projectInit()));
    connect(kmfApp->project(), SIGNAL(mediaModified()),
            mediaPage, SLOT(mediaModified()));

    connect(kmfApp->project(), SIGNAL(init(const QString &)),
            templatePage, SLOT(projectInit()));

    connect(kmfApp->project(), SIGNAL(init(const QString &)),
            outputPage, SLOT(projectInit()));
}

void KMediaFactory::itemDelete()
{
    // Dont delete media object if the media page is not visible!
    if (mediaPage->isVisible()) {
        QModelIndex i = mediaPage->mediaFiles->currentIndex();

        if (i.isValid()) {
            KMF::MediaObject *mob = kmfApp->project()->mediaObjects()->at(i);

            if (mob &&
                (KMessageBox::Yes ==
            KMessageBox::warningYesNo(mediaPage, i18n("Remove \"%1\" from project?", mob->text()),
                    i18n("Remove Media"))))
            {
                kmfApp->project()->mediaObjects()->removeAt(i);
                kmfApp->project()->setDirty(KMF::Media);
            }
        }
    }
}

void KMediaFactory::newStuff()
{
#if KDE_IS_VERSION(4, 3, 90)
    KNS3::DownloadDialog dialog("kmediafactory.knsrc", this);
    dialog.exec();
#else
    KNS::Engine *engine = new KNS::Engine();
    engine->init("kmediafactory.knsrc");
    engine->downloadDialogModal();
    delete engine;
#endif
}

void KMediaFactory::initGUI()
{
    kDebug();
    projectPage->init();

    const QObjectList &l = kmfApp->pluginInterface()->children();

    for (int i = 0; i < l.size(); ++i) {
        if (l[i]->inherits("KMF::Plugin")) {
            guiFactory()->addClient((KMF::Plugin *)l[i]);
        }
    }

    if (!kmfApp->url().isEmpty() &&
        KIO::NetAccess::exists(kmfApp->url(), KIO::NetAccess::SourceSide, this))
    {
        load(kmfApp->url());
    } else {
        fileNew();
    }
}

void KMediaFactory::resetGUI()
{
    templatePage->templatePreview->clear();
    QTreeView *tv = outputPage->progressListView;
    QStandardItemModel *model = static_cast<QStandardItemModel *>(tv->model());
    model->clear();
    outputPage->progressBar->reset();
    outputPage->showLogPushBtn->setEnabled(false);
    m_janus->setCurrentPage(m_projectPageItem);
}

void KMediaFactory::fileNew()
{
    kDebug();

    if (checkSaveProject()) {
        resetGUI();
        kmfApp->newProject();
        connectProject();
        kmfApp->project()->init();
        projectPage->setData(*kmfApp->project());
    }
}

void KMediaFactory::fileOpen()
{
    KUrl url =
        KFileDialog::getOpenUrl(KUrl("kfiledialog:///<KMFProject>"),
                "*.kmf|KMediaFactory project files",
                kapp->activeWindow());

    if (!url.isEmpty()) {
        load(url);
    }
}

void KMediaFactory::quit()
{
    close();
}

void KMediaFactory::load(const KUrl &url)
{
    if (checkSaveProject()) {
        resetGUI();
        kmfApp->newProject();
        connectProject();
        kmfApp->project()->open(url);
        projectPage->setData(*kmfApp->project());
        mediaPage->mediaModified();
        templatePage->updatePreview();
    }
}

void KMediaFactory::setProjectTitle(const QString &title)
{
    projectPage->setProjectTitle(title);
}

void KMediaFactory::setProjectType(const QString &type)
{
    projectPage->setProjectType(type);
}

void KMediaFactory::setProjectAspectRatio(const QString &aspect)
{
    projectPage->setProjectAspectRatio(aspect);
}

void KMediaFactory::setProjectDirectory(const QString &dir)
{
    projectPage->setProjectDirectory(dir);
}

void KMediaFactory::fileSave()
{
    if (!kmfApp->project()->save()) {
        fileSaveAs();
    }
}

void KMediaFactory::fileSaveAs()
{
    KUrl url =
        KFileDialog::getSaveUrl(KUrl("kfiledialog:///<KMFProject>"),
                "*.kmf|KMediaFactory project files",
                kapp->activeWindow());

    if (!url.isEmpty() && url.isValid()) {
        if (!kmfApp->project()->save(url)) {
            KMessageBox::error(kmfApp->activeWindow(),
                    i18n("Could not write to file: %1", url.prettyUrl()));
        }
    }
}

void KMediaFactory::saveProperties(KConfigGroup &config)
{
    if (!kmfApp->url().isEmpty()) {
        config.writePathEntry("lastUrl", kmfApp->url().prettyUrl());
    }
}

void KMediaFactory::readProperties(const KConfigGroup &config)
{
    QString url = config.readPathEntry("lastUrl", "");

    if (!url.isEmpty()) {
        load(KUrl(url));
    }
}

void KMediaFactory::dragEnterEvent(QDragEnterEvent *event)
{
    KUrl::List uriList = KUrl::List::fromMimeData(event->mimeData());

    event->setAccepted(!uriList.isEmpty());
}

void KMediaFactory::dropEvent(QDropEvent *event)
{
    KUrl::List uriList = KUrl::List::fromMimeData(event->mimeData());

    if (!uriList.isEmpty()) {
        const KUrl &url = uriList.first();
        load(url);
    }
}

void KMediaFactory::optionsConfigureKeys()
{
    KShortcutsDialog::configure(actionCollection());
}

void KMediaFactory::optionsConfigureToolbars()
{
    if (autoSaveSettings()) {
        KConfigGroup cg = KGlobal::config()->group("KMFMainWindow");
        saveMainWindowSettings(cg);
    }

    KEditToolBar dlg(actionCollection());
    connect(&dlg, SIGNAL(newToolbarConfig()), this, SLOT(newToolbarConfig()));
    dlg.setDefaultToolBar("default");
    dlg.exec();
}

void KMediaFactory::newToolbarConfig()
{
    createGUI("kmediafactoryui.rc");
    KConfigGroup cg = KGlobal::config()->group("KMFMainWindow");
    applyMainWindowSettings(cg);
}

void KMediaFactory::updateToolsMenu()
{
    kDebug();
    QAction *action;
    QList<QAction *> actions;
    QList<QAction *> media_actions;
    QStringList files =
        KGlobal::dirs()->findAllResources("appdata", "tools/*.desktop");

    unplugActionList("tools_list");
    unplugActionList("media_tools_list");
    mediaPage->mediaButtons->removeActions("media_tools_list");

    for (QStringList::ConstIterator it = files.begin(); it != files.end(); ++it) {
        KDesktopFile df(*it);
        KConfigGroup group = df.group("Desktop Entry");

        if (df.readName().isEmpty()) {
            continue;
        }

        action = new KAction(KIcon(df.readIcon()), df.readName(), this);
        connect(action, SIGNAL(triggered()), this, SLOT(execTool()));
        action->setObjectName(*it);
        actionCollection()->addAction((*it), action);

        if (group.readEntry("X-KMF-MediaMenu") == "true") {
            media_actions.append(action);
        } else {
            actions.append(action);
        }
    }

    plugActionList("tools_list", actions);
    plugActionList("media_tools_list", media_actions);
    mediaPage->mediaButtons->addActions(media_actions, "media_tools_list");
}

void KMediaFactory::execTool()
{
    QString error;
    QStringList envs;
    KDesktopFile desktopFile(sender()->objectName());
    KService service(&desktopFile);
    KUrl::List lst;
    QStringList program = KRun::processDesktopExec(service, lst);
    QString workingDir = desktopFile.readPath();

    if (workingDir.isEmpty()) {
        workingDir = kmfApp->project()->directory("", false);
    }

    enableUi(false);
    Run run(program, desktopFile.readPath());
    QString output = run.output();

    if (run.output().trimmed() != "") {
        kDebug() << "\n" + run.output();
    }

    enableUi(true);
}

void KMediaFactory::optionsPreferences()
{
    // An instance of your dialog could be already created and could be cached,
    // in which case you want to display the cached dialog instead of creating
    // another one
    if (!KConfigDialog::showDialog("KMediaFactorySettings")) {
        // KConfigDialog didn't find an instance of this dialog, so lets create it :
        KConfigDialog *dialog = new KConfigDialog(this, "KMediaFactorySettings",
                KMediaFactorySettings::self());
        dialog->addPage(new KMFOptions(dialog), i18n("Project"),
                "kmediafactory", i18n("Default Project Settings"));
        dialog->addPage(new Tools(dialog), i18n("Tools"),
                "configure", i18n("External Tools"));

        const KMF::PluginList list = kmfApp->plugins();

        for (KMF::PluginList::ConstIterator obj = list.begin();
             obj != list.end(); ++obj)
        {
            const KMF::ConfigPage *page = (*obj)->configPage();

            if (page) {
                dialog->addPage(page->page, page->config,
                        page->itemName, page->pixmapName,
                        page->itemDescription);
                delete page;
            }
        }

        // connect(dialog, SIGNAL(okClicked()), this, SLOT(updateToolsMenu()));

        dialog->show();
    }
}

void KMediaFactory::optionsShowToolbar()
{
    if (m_toolbarAction->isChecked()) {
        toolBar()->show();
    } else {
        toolBar()->hide();
    }
}

bool KMediaFactory::checkSaveProject()
{
    if (m_enabled == false) {
        return false;
    }

    if (kmfApp->project() && kmfApp->project()->isDirty()) {
        switch (KMessageBox::warningYesNoCancel(this,
                        i18n("Save changes to project?")))
        {
            case KMessageBox::Yes:

            if (!kmfApp->project()->save()) {
                fileSaveAs();

                if (kmfApp->project()->url().isEmpty()) {
                    return false;
                }
            }

            break;
            case KMessageBox::No:
            break;
            default: // cancel
            return false;
        }
    }

    return true;
}

bool KMediaFactory::queryClose()
{
    if (checkSaveProject() == false) {
        return false;
    }

    kmfApp->finalize();
    return true;
}

void KMediaFactory::enableUi(bool enabled)
{
    menuBar()->setEnabled(enabled);
    toolBar()->setEnabled(enabled);

    if (m_janusIconList) {
        m_janusIconList->setEnabled(enabled);
    }

    projectPage->setEnabled(enabled);
    mediaPage->mediaFiles->setEnabled(enabled);
    mediaPage->mediaButtons->setEnabled(enabled);
    templatePage->templates->setEnabled(enabled);
    // templatePage->previewCheckBox->setEnabled(enabled);
    templatePage->templatePreview->setEnabled(enabled);
    outputPage->outputs->setEnabled(enabled);
    m_enabled = enabled;
}

void KMediaFactory::showPage(int page)
{
    switch (page) {
        case Project:
            m_janus->setCurrentPage(m_projectPageItem);
            break;
        case Media:
            m_janus->setCurrentPage(m_mediaPageItem);
            break;
        case Template:
            m_janus->setCurrentPage(m_templatePageItem);
            break;
        case Output:
            m_janus->setCurrentPage(m_ouputPageItem);
            break;
    }
}

#include "kmediafactory.moc"
