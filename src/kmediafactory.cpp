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

#include "kmediafactory.h"
#include "kmfapplication.h"
#include "mediapage.h"
#include "templatepage.h"
#include "outputpage.h"
#include "kmfuiinterface.h"
#include "kmficonview.h"
#include "projectoptions.h"
#include "kmfproject.h"
#include "kmfprogresslistview.h"
#include "kmfoptions.h"
#include "tools.h"
#include "kmediafactorysettings.h"
#include "kmfimageview.h"
#include "kmftools.h"
#include "kmfnewstuff.h"
#include <kmediafactory/plugin.h>

#include <kmainwindow.h>
#include <kconfigdialog.h>
#include <klocale.h>
#include <kstdaction.h>
#include <kpagewidget.h>
#include <kdialog.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <kcmdlineargs.h>
#include <kaction.h>
#include <kconfig.h>
#include <kkeydialog.h>
#include <kedittoolbar.h>
#include <kmessagebox.h>
#include <kprogressbar.h>
#include <kpushbutton.h>
#include <kmenubar.h>
#include <kio/netaccess.h>
#include <kdesktopfile.h>
#include <kstandarddirs.h>
#include <kicon.h>
#include <kxmlguifactory.h>
#include <ktoggleaction.h>

#include <QLabel>
#include <QObject>
#include <QToolButton>
#include <QPoint>
#include <QTimer>
#include <QDragEnterEvent>
#include <QDropEvent>

#ifdef KMF_TEST
#include "qdvdinfo.h"
#endif

// TODO: Move everything not related to main window to application
// (loadPlugins...)

KMediaFactory::KMediaFactory()
  : KMainWindow(0), m_janusIconList(0), m_enabled(true),
    m_newStuffDlg(0)
{
  // set the shell's ui resource file
  setXMLFile("kmediafactoryui.rc");

  // Centra widget
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
    if(w->metaObject()->className() == "QWidget")
      m_janusIconList = w;
  }

  // Pages
  // Media

  mediaPage = new MediaPage;
  m_mediaPageItem = new KPageWidgetItem(mediaPage, i18n( "Media"));
  m_mediaPageItem->setHeader(i18n("Media"));
  m_mediaPageItem->setIcon(KIcon("folder_video"));
  m_janus->addPage(m_mediaPageItem);

  // Template
  templatePage = new TemplatePage;
  m_templatePageItem = new KPageWidgetItem(templatePage, i18n("Template"));
  m_templatePageItem->setHeader(i18n("Template"));
  m_templatePageItem->setIcon(KIcon("kmultiple"));
  m_janus->addPage(m_templatePageItem);
  connect(m_janus,
          SIGNAL(currentPageChanged(KPageWidgetItem*, KPageWidgetItem*)),
          templatePage,
          SLOT(currentPageChanged(KPageWidgetItem*, KPageWidgetItem*)));

  // Output
  outputPage = new OutputPage;
  m_ouputPageItem = new KPageWidgetItem(outputPage, i18n("Ouput"));
  m_ouputPageItem->setHeader(i18n("Output"));
  m_ouputPageItem->setIcon(KIcon("cdwriter_unmount"));
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

  // TODO: move these to correct places
  // TODO: make function QPtrList<KMF::Plugins> getPlugins()
  const QObjectList& l = kmfApp->pluginInterface()->children();
  for(int i = 0; i < l.size(); ++i)
    if(l[i]->inherits("KMF::Plugin"))
      guiFactory()->addClient((KMF::Plugin*)l[i]);

  // Do some things later
  QTimer::singleShot(0, this, SLOT(initGUI()));
}

KMediaFactory::~KMediaFactory()
{
  delete m_newStuffDlg;
  //delete mediaPage;
  //delete templatePage;
  //delete outputPage;
  //delete pluginInterface;
  //delete mediaInterface;
}

void KMediaFactory::setupActions()
{
  KAction* action;

  // File
  KStdAction::openNew(this, SLOT(fileNew()), actionCollection());
  KStdAction::open(this, SLOT(fileOpen()), actionCollection());
  KStdAction::save(this, SLOT(fileSave()), actionCollection());
  KStdAction::saveAs(this, SLOT(fileSaveAs()), actionCollection());
  KStdAction::quit(this, SLOT(quit()), actionCollection());

  // Project
  action = new KAction(i18n("&Options"), actionCollection(), "project_options");
  action->setIcon(KIcon("configure"));
  connect(action, SIGNAL(triggered()), this, SLOT(projectOptions()));

  // KNewStuff
  new KAction(i18n("&Get new tools"), actionCollection(), "newstuff");
  action->setIcon(KIcon("bookmark"));
  connect(action, SIGNAL(triggered()), this, SLOT(newStuff()));

  // Settings
  KStdAction::keyBindings(this, SLOT(optionsConfigureKeys()),
                          actionCollection());
  KStdAction::configureToolbars(this, SLOT(optionsConfigureToolbars()),
                                actionCollection());
  KStdAction::preferences(this, SLOT(optionsPreferences()),
                          actionCollection());

  setStandardToolBarMenuEnabled(true);

  // Media file menu
  new KAction(i18n("&Delete"), actionCollection(), "delete" );
  action->setShortcut(Qt::Key_Delete);
  action->setIcon(KIcon("editdelete"));
  connect(action, SIGNAL(triggered()), this, SLOT(itemDelete()));

  // Testing
#ifdef KMF_TEST
  new KAction(i18n("&Test"), "configure", Qt::CTRL + Qt::Key_Z, this,
              SLOT(test()), actionCollection(), "test" );
#endif
  //m_statusbarAction = KStdAction::showStatusbar(this,
  //     SLOT(optionsShowStatusbar()), actionCollection());

  createGUI("kmediafactoryui.rc");

  updateToolsMenu();
}

void KMediaFactory::connectProject()
{
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
  connect(kmfApp->project(), SIGNAL(templatesModified()),
          templatePage, SLOT(templatesModified()));

  connect(kmfApp->project(), SIGNAL(init(const QString&)),
          outputPage, SLOT(projectInit()));
  connect(kmfApp->project(), SIGNAL(outputsModified()),
          outputPage, SLOT(outputsModified()));
}

void KMediaFactory::itemDelete()
{
  /*
  KMFIconViewItem* item =
      static_cast<KMFIconViewItem*>(mediaPage->mediaFiles->currentItem());
  if(item)
  {
    KMF::Object *ob = item->ob();
    kmfApp->project()->removeItem(static_cast<KMF::MediaObject*>(ob));
  }
  */
}

void KMediaFactory::projectOptions()
{
  ProjectOptions dlg(this);
  dlg.setData(*kmfApp->project());
  if (dlg.exec())
    dlg.getData(*kmfApp->project());
}

void KMediaFactory::newStuff()
{
  if(!m_newStuffDlg)
  {
    m_newStuffDlg =
        new KMFNewStuff(KMediaFactorySettings::providersUrl(), this);
    connect(m_newStuffDlg, SIGNAL(scriptInstalled()),
            this, SLOT(updateToolsMenu()));
  }
  m_newStuffDlg->download();
}

void KMediaFactory::initGUI()
{
  if(!kmfApp->url().isEmpty() &&
      KIO::NetAccess::exists(kmfApp->url(), true, this))
    load(kmfApp->url());
  else
    fileNew();
}

void KMediaFactory::resetGUI()
{
  templatePage->templatePreview->clear();
#warning TODO: delete KMFProgressItems
  //outputPage->progressListView->clear();
  outputPage->progressBar->reset();
  outputPage->showLogPushBtn->setEnabled(false);
  m_janus->setCurrentPage(m_mediaPageItem);
}

void KMediaFactory::fileNew()
{
  if(checkSaveProject())
  {
    resetGUI();
    kmfApp->newProject();
    connectProject();
    kmfApp->project()->init();
    templatePage->loadingFinished();
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
    templatePage->updatePreview();
    templatePage->loadingFinished();
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
          i18n("Could not write to file: %1").arg(url.prettyUrl()));
    }
  }
}

void KMediaFactory::saveProperties(KConfig *config)
{
  if (!kmfApp->url().isEmpty())
  {
    config->writePathEntry("lastUrl", kmfApp->url().prettyUrl());
  }
}

void KMediaFactory::readProperties(KConfig *config)
{
  QString url = config->readPathEntry("lastUrl");

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
  KKeyDialog::configure(actionCollection());
}

void KMediaFactory::optionsConfigureToolbars()
{
    // use the standard toolbar editor
  saveMainWindowSettings(KGlobal::config(), autoSaveGroup());
  KEditToolbar dlg(actionCollection());
  connect(&dlg, SIGNAL(newToolbarConfig()), this, SLOT(newToolbarConfig()));
  dlg.setDefaultToolbar("default");
  dlg.exec();
}

void KMediaFactory::newToolbarConfig()
{
  createGUI("kmediafactoryui.rc");
  applyMainWindowSettings(KGlobal::config(), autoSaveGroup());
}

void KMediaFactory::updateToolsMenu()
{
  KAction* action;
  QList<KAction*> actions;
  QList<KAction*> media_actions;
  QStringList files =
      KGlobal::dirs()->findAllResources("appdata", "tools/*.desktop");

  unplugActionList("tools_list");
  unplugActionList("media_tools_list");
  for(QStringList::ConstIterator it = files.begin(); it != files.end(); ++it)
  {
    KDesktopFile df(*it, true);

    if (df.readName().isEmpty())
      continue;
    action = new KAction(df.readName(), actionCollection(), (*it));
    action->setIcon(KIcon(df.readIcon()));
    connect(action, SIGNAL(triggered()), this, SLOT(execTool()));

    if(df.readEntry("X-KMF-MediaMenu") == "true")
      media_actions.append(action);
    else
      actions.append(action);
  }
  plugActionList("tools_list", actions);
  plugActionList("media_tools_list", media_actions);
}

void KMediaFactory::execTool()
{
  QString error;
  QStringList envs;

  envs.append(QString("KMF_DBUS=org.kde.kmediafactory_%1 /KMediaFactory")
      .arg(getpid()));
  envs.append("KMF_WINID=" +
      QString("%1").arg(kmfApp->mainWindow()->winId()));
  if(kmfApp->startServiceByDesktopPath(sender()->objectName(),
     QString::null, envs, &error))
  {
    KMessageBox::error(kapp->activeWindow(),
                       i18n("Error in starting %1: %2")
                           .arg(sender()->objectName()).arg(error));
  }
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
                                              KMediaFactorySettings::self(),
                                              KPageDialog::List);
#warning TODO how to do this KDE4
/*
    Q3SqlPropertyMap::defaultMap()->insert("KMFLanguageComboBox", "language");
    Q3SqlPropertyMap::defaultMap()->insert("KMFLanguageListBox", "language");
    Q3SqlPropertyMap::defaultMap()->insert("KColorCombo", "color");
    */

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
    connect(dialog, SIGNAL(okClicked()), this, SLOT(updateToolsMenu()));

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

  if(kmfApp->project() && kmfApp->project()->isModified())
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

#ifdef KMF_TEST
void KMediaFactory::test()
{
  QDVD::Info info;

  info.parseDVD();
}
#endif

void KMediaFactory::enableUi(bool enabled)
{
  menuBar()->setEnabled(enabled);
  toolBar()->setEnabled(enabled);
  if(m_janusIconList)
    m_janusIconList->setEnabled(enabled);
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