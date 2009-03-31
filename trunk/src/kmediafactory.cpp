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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//**************************************************************************

#include "kmediafactory.h"
#include "kmfapplication.h"
#include "mediapage.h"
#include "templatepage.h"
#include "outputpage.h"
#include "kmfplugininterface.h"
#include "projectoptions.h"
#include "kmfproject.h"
#include "kmfoptions.h"
#include "tools.h"
#include "kmediafactorysettings.h"
#include <kmfimageview.h>
#include <kmftools.h>
#include <run.h>
#include <kmediafactory/plugin.h>

#include <KIO/NetAccess>
#include <knewstuff2/engine.h>
#include <KRun>
#include <KService>
#include <KActionCollection>
#include <KMainWindow>
#include <KConfigDialog>
#include <KLocale>
#include <KStandardAction>
#include <KPageWidget>
#include <KDialog>
#include <KDebug>
#include <KFileDialog>
#include <KIconLoader>
#include <KCmdLineArgs>
#include <KConfig>
#include <KEditToolBar>
#include <KMessageBox>
#include <KPushButton>
#include <KMenuBar>
#include <KDesktopFile>
#include <KStandardDirs>
#include <KIcon>
#include <KXMLGUIFactory>
#include <KToggleAction>
#include <KShortcutsDialog>
#include <KToolBar>

#include <QLabel>
#include <QObject>
#include <QToolButton>
#include <QPoint>
#include <QTimer>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QStandardItemModel>

KMediaFactory::KMediaFactory()
  : KXmlGuiWindow(0), m_janusIconList(0), m_enabled(true),
    m_newStuffDlg(0)
{
  // set the shell's ui resource file
  setXMLFile("kmediafactoryui.rc");

  // Central widget
  QWidget* main = new QWidget;
  QVBoxLayout* vbox = new QVBoxLayout;
  m_janus = new KPageWidget;
  m_janus->setFaceType(KPageView::List);
  vbox->setMargin(KDialog::marginHint());
  vbox->addWidget(m_janus);
  main->setLayout(vbox);
  setCentralWidget(main);
  const QObjectList& list = m_janus->children();
  for(int i = 0; i < list.size(); ++i)
  {
    QWidget* w = static_cast<QWidget*>(list[i]);
    if (qstrcmp(w->metaObject()->className(), "QWidget") == 0)
      m_janusIconList = w;
  }

  // Pages
  // Media

  mediaPage = new MediaPage;
  m_mediaPageItem = new KPageWidgetItem(mediaPage, i18n( "Media"));
  m_mediaPageItem->setHeader(i18n("Media"));
  m_mediaPageItem->setIcon(KIcon("folder-video"));
  m_janus->addPage(m_mediaPageItem);

  // Template
  templatePage = new TemplatePage;
  m_templatePageItem = new KPageWidgetItem(templatePage, i18n("Template"));
  m_templatePageItem->setHeader(i18n("Template"));
  m_templatePageItem->setIcon(KIcon("folder-image"));
  m_janus->addPage(m_templatePageItem);
  connect(m_janus,
          SIGNAL(currentPageChanged(KPageWidgetItem*, KPageWidgetItem*)),
          templatePage,
          SLOT(currentPageChanged(KPageWidgetItem*, KPageWidgetItem*)));

  // Output
  outputPage = new OutputPage;
  m_ouputPageItem = new KPageWidgetItem(outputPage, i18n("Output"));
  m_ouputPageItem->setHeader(i18n("Output"));
  m_ouputPageItem->setIcon(KIcon("media-optical"));
  m_janus->addPage(m_ouputPageItem);
  connect(m_janus,
          SIGNAL(currentPageChanged(KPageWidgetItem*, KPageWidgetItem*)),
          outputPage,
          SLOT(currentPageChanged(KPageWidgetItem*, KPageWidgetItem*)));

  // then, setup our actions
  setupActions();

  // apply the saved mainwindow settings, if any, and ask the mainwindow
  // to automatically save settings if changed: window size, toolbar
  // position, icon size, etc.
  setAutoSaveSettings();

  // Do some things later
  //QTimer::singleShot(0, this, SLOT(initGUI()));
}

KMediaFactory::~KMediaFactory()
{
  //delete m_newStuffDlg;
  //delete mediaPage;
  //delete templatePage;
  //delete outputPage;
  //delete pluginInterface;
  //delete mediaInterface;
}

void KMediaFactory::setupActions()
{
  kDebug();
  QAction* action;

  // File
  KStandardAction::openNew(this, SLOT(fileNew()), actionCollection());
  KStandardAction::open(this, SLOT(fileOpen()), actionCollection());
  KStandardAction::save(this, SLOT(fileSave()), actionCollection());
  KStandardAction::saveAs(this, SLOT(fileSaveAs()), actionCollection());
  KStandardAction::quit(this, SLOT(quit()), actionCollection());

  // Project
  action = new KAction(KIcon("configure"), i18n("&Options"),this);
  actionCollection()->addAction("project_options", action);
  connect(action, SIGNAL(triggered()), SLOT(projectOptions()));

  // KNewStuff
  action = new KAction(KIcon("get-hot-new-stuff"), i18n("&Get new tools"),this);
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
  action = new KAction(KIcon("edit-delete"), i18n("&Delete"), this);
  action->setShortcut(Qt::Key_Delete);
  actionCollection()->addAction("delete", action);
  connect(action, SIGNAL(triggered()), SLOT(itemDelete()));

  createGUI("kmediafactoryui.rc");

  QStringList dirs = KGlobal::dirs()->findDirs("data", "");
  for(QStringList::ConstIterator it = dirs.begin(); it != dirs.end(); ++it)
  {
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
  for(KMF::PluginList::ConstIterator obj = list.begin();
      obj != list.end(); ++obj)
  {
    connect(kmfApp->project(), SIGNAL(preinit(const QString&)),
            *obj, SLOT(init(const QString&)));
  }

  connect(kmfApp->project(), SIGNAL(modified(const QString&, bool)),
          this, SLOT(setCaption(const QString&, bool)));

  connect(kmfApp->project(), SIGNAL(init(const QString&)),
          mediaPage, SLOT(projectInit()));
  connect(kmfApp->project(), SIGNAL(mediaModified()),
          mediaPage, SLOT(mediaModified()));

  connect(kmfApp->project(), SIGNAL(init(const QString&)),
          templatePage, SLOT(projectInit()));

  connect(kmfApp->project(), SIGNAL(init(const QString&)),
          outputPage, SLOT(projectInit()));
}

void KMediaFactory::itemDelete()
{
  QModelIndex i = mediaPage->mediaFiles->currentIndex();
  kmfApp->project()->mediaObjects()->removeAt(i);
  kmfApp->project()->setDirty(KMF::Media);
}

void KMediaFactory::projectOptions()
{
  //kDebug();
  ProjectOptions dlg(this);
  dlg.setData(*kmfApp->project());
  if (dlg.exec())
    dlg.getData(*kmfApp->project());
}

void KMediaFactory::newStuff()
{
  KNS::Engine *engine = new KNS::Engine();
  engine->init("kmediafactory.knsrc");
  KNS::Entry::List entries = engine->downloadDialogModal();
  delete engine;
}

void KMediaFactory::initGUI()
{
  kDebug();

  const QObjectList& l = kmfApp->pluginInterface()->children();
  for(int i = 0; i < l.size(); ++i)
    if(l[i]->inherits("KMF::Plugin"))
      guiFactory()->addClient((KMF::Plugin*)l[i]);

  if(!kmfApp->url().isEmpty() &&
      KIO::NetAccess::exists(kmfApp->url(), KIO::NetAccess::SourceSide, this))
    load(kmfApp->url());
  else
    fileNew();
}

void KMediaFactory::resetGUI()
{
  templatePage->templatePreview->clear();
  QTreeView* tv = outputPage->progressListView;
  QStandardItemModel* model = static_cast<QStandardItemModel*>(tv->model());
  model->clear();
  outputPage->progressBar->reset();
  outputPage->showLogPushBtn->setEnabled(false);
  m_janus->setCurrentPage(m_mediaPageItem);
}

void KMediaFactory::fileNew()
{
  kDebug();
  if(checkSaveProject())
  {
    resetGUI();
    kmfApp->newProject();
    connectProject();
    kmfApp->project()->init();
    if(KMediaFactorySettings::showProjectOptionsOnNew())
      projectOptions();
  }
}

void KMediaFactory::fileOpen()
{
  KUrl url =
      KFileDialog::getOpenUrl(KUrl("kfiledialog:///<KMFProject>"),
                                   "*.kmf|KMediaFactory project files");
  if (!url.isEmpty())
    load(url);
}

void KMediaFactory::quit()
{
  close();
}

void KMediaFactory::load(const KUrl& url)
{
  if(checkSaveProject())
  {
    resetGUI();
    kmfApp->newProject();
    connectProject();
    kmfApp->project()->open(url);
    mediaPage->mediaModified();
    templatePage->updatePreview();
  }
}

void KMediaFactory::fileSave()
{
  if(!kmfApp->project()->save())
    fileSaveAs();
}

void KMediaFactory::fileSaveAs()
{
  KUrl url =
      KFileDialog::getSaveUrl(KUrl("kfiledialog:///<KMFProject>"),
                                   "*.kmf|KMediaFactory project files");
  if (!url.isEmpty() && url.isValid())
  {
    if(!kmfApp->project()->save(url))
    {
      KMessageBox::error(kmfApp->activeWindow(),
          i18n("Could not write to file: %1", url.prettyUrl()));
    }
  }
}

void KMediaFactory::saveProperties(KConfigGroup& config)
{
  if (!kmfApp->url().isEmpty())
  {
    config.writePathEntry("lastUrl", kmfApp->url().prettyUrl());
  }
}

void KMediaFactory::readProperties(const KConfigGroup& config)
{
  QString url = config.readPathEntry("lastUrl", "");

  if (!url.isEmpty())
    load(KUrl(url));
}

void KMediaFactory::dragEnterEvent(QDragEnterEvent *event)
{
  KUrl::List uriList = KUrl::List::fromMimeData(event->mimeData());
  event->setAccepted(!uriList.isEmpty());
}

void KMediaFactory::dropEvent(QDropEvent *event)
{
  KUrl::List uriList = KUrl::List::fromMimeData(event->mimeData());

  if(!uriList.isEmpty())
  {
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
  if(autoSaveSettings())
  {
      KConfigGroup cg = KGlobal::config()->group("KMFMainWindow");
      saveMainWindowSettings(cg);
  }
  KEditToolBar dlg(actionCollection());
  connect(&dlg,SIGNAL(newToolbarConfig()),this,SLOT(newToolbarConfig()));
  dlg.setDefaultToolBar("default");
  dlg.exec();
}

void KMediaFactory::newToolbarConfig()
{
  createGUI("kmediafactoryui.rc");
  KConfigGroup cg = KGlobal::config()->group("KMFMainWindow");
  applyMainWindowSettings( cg );
}

void KMediaFactory::updateToolsMenu()
{
  kDebug();
  QAction* action;
  QList<QAction*> actions;
  QList<QAction*> media_actions;
  QStringList files =
      KGlobal::dirs()->findAllResources("appdata", "tools/*.desktop");

  unplugActionList("tools_list");
  unplugActionList("media_tools_list");
  mediaPage->mediaButtons->removeActions("media_tools_list");

  for(QStringList::ConstIterator it = files.begin(); it != files.end(); ++it)
  {
    KDesktopFile df(*it);
    KConfigGroup group = df.group("Desktop Entry");

    if (df.readName().isEmpty())
      continue;
    action = new KAction(KIcon(df.readIcon()), df.readName(), this);
    connect(action, SIGNAL(triggered()), this, SLOT(execTool()));
    action->setObjectName(*it);
    actionCollection()->addAction((*it), action);

    if(group.readEntry("X-KMF-MediaMenu") == "true")
      media_actions.append(action);
    else
      actions.append(action);
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
  if (workingDir.isEmpty())
    workingDir = kmfApp->project()->directory("", false);
  enableUi(false);
  Run run(program, desktopFile.readPath());
  QString output = run.output();
  if(run.output().trimmed() != "")
    kDebug() << "\n" + run.output();
  enableUi(true);
}

void KMediaFactory::optionsPreferences()
{
  //An instance of your dialog could be already created and could be cached,
  //in which case you want to display the cached dialog instead of creating
  //another one
  if(!KConfigDialog::showDialog("KMediaFactorySettings"))
  {
    //KConfigDialog didn't find an instance of this dialog, so lets create it :
    KConfigDialog* dialog = new KConfigDialog(this, "KMediaFactorySettings",
                                              KMediaFactorySettings::self());
    dialog->addPage(new KMFOptions(dialog), i18n("KMediaFactory"),
                    "kmediafactory", i18n("KMediaFactory"));
    dialog->addPage(new Tools(dialog), i18n("Tools"),
                    "configure", i18n("Tools"));

    const KMF::PluginList list = kmfApp->plugins();
    for(KMF::PluginList::ConstIterator obj = list.begin();
        obj != list.end(); ++obj)
    {
      const KMF::ConfigPage* page = (*obj)->configPage();
      if(page)
      {
        dialog->addPage(page->page, page->config,
                        page->itemName, page->pixmapName);
        delete page;
      }
    }
    //connect(dialog, SIGNAL(okClicked()), this, SLOT(updateToolsMenu()));

    dialog->show();
  }
}

void KMediaFactory::optionsShowToolbar()
{
  if (m_toolbarAction->isChecked())
    toolBar()->show();
  else
    toolBar()->hide();
}

bool KMediaFactory::checkSaveProject()
{
  if(m_enabled == false)
    return false;

  if(kmfApp->project() && kmfApp->project()->isDirty())
  {
    switch(KMessageBox::warningYesNoCancel(this,
           i18n("Save changes to project?")))
    {
      case KMessageBox::Yes :
        if(!kmfApp->project()->save())
        {
          fileSaveAs();
          if(kmfApp->project()->url().isEmpty())
            return false;
        }
        break;
      case KMessageBox::No :
        break;
      default: // cancel
        return false;
    }
  }
  return true;
}

bool KMediaFactory::queryClose()
{
  if(checkSaveProject() == false)
    return false;

  kmfApp->finalize();
  return true;
}

void KMediaFactory::enableUi(bool enabled)
{
  menuBar()->setEnabled(enabled);
  toolBar()->setEnabled(enabled);
  if(m_janusIconList)
    m_janusIconList->setEnabled(enabled);
  mediaPage->mediaFiles->setEnabled(enabled);
  mediaPage->mediaButtons->setEnabled(enabled);
  templatePage->templates->setEnabled(enabled);
  templatePage->previewCheckBox->setEnabled(enabled);
  templatePage->templatePreview->setEnabled(enabled);
  outputPage->outputs->setEnabled(enabled);
  m_enabled = enabled;
}

void KMediaFactory::showPage(int page)
{
  switch(page)
  {
    case 0: m_janus->setCurrentPage(m_mediaPageItem); break;
    case 1: m_janus->setCurrentPage(m_templatePageItem); break;
    case 2: m_janus->setCurrentPage(m_ouputPageItem); break;
  }
}

#include "kmediafactory.moc"
