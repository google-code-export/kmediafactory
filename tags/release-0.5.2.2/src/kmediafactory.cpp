//**************************************************************************
//   Copyright (C) 2004 by Petri Damsten
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
#include "kmfoptionslayout.h"
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
#include <kjanuswidget.h>
#include <kdialog.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <kcmdlineargs.h>
#include <kaction.h>
#include <kconfig.h>
#include <kurldrag.h>
#include <kkeydialog.h>
#include <kedittoolbar.h>
#include <kactionclasses.h>
#include <kmessagebox.h>
#include <kprogress.h>
#include <kpushbutton.h>
#include <kmenubar.h>
#include <kio/netaccess.h>
#include <kdesktopfile.h>
#include <dcopclient.h>
#include <kstandarddirs.h>

#include <qlabel.h>
#include <qvbox.h>
#include <qobjectlist.h>
#include <qtoolbutton.h>
#include <qpopupmenu.h>
#include <qpoint.h>
#include <qtimer.h>
#include <qsqlpropertymap.h>

#ifdef KMF_TEST
#include "qdvdinfo.h"
#endif

// TODO: Move everything not related to main window to application
// (loadPlugins...)

KMediaFactory::KMediaFactory()
  : KMainWindow( 0, "KMediaFactory" ), m_janusIconList(0), m_enabled(true),
    m_newStuffDlg(0)
{
  QObject *obj;

  // set the shell's ui resource file
  setXMLFile("kmediafactoryui.rc");

  // Centra widget
  QVBox* vbox = new QVBox(this);
  m_janus = new KJanusWidget(vbox, "m_janus", KJanusWidget::IconList);
  vbox->setMargin(KDialog::marginHint());
  setCentralWidget(vbox);
  for(QObjectListIt it(*m_janus->children()); it.current() != 0; ++it)
  {
    QWidget* w = static_cast<QWidget*>(it.current());
    if(w->isA("QWidget"))
      m_janusIconList = w;
  }

  // Pages
  // Media
  QVBox *parent = m_janus->addVBoxPage(i18n("Media"), i18n("Media"),
                                     DesktopIcon("folder_video"));
  mediaPage = new MediaPage(parent);
  mediaPage->mediaButtons->setIconSize(KIcon::SizeLarge);
  mediaPage->mediaButtons->setIconText(KToolBar::IconTextBottom);
  mediaPage->mediaButtons->setFrameStyle(QFrame::Panel | QFrame::Raised);
  mediaPage->mediaButtons->setLineWidth(1);
  mediaPage->mediaButtons->setMargin(0);

  // Template
  parent = m_janus->addVBoxPage(i18n("Template"), i18n("Template"),
                              DesktopIcon("kmultiple"));
  templatePage = new TemplatePage(parent);
  connect(m_janus, SIGNAL(aboutToShowPage(QWidget*)),
          templatePage, SLOT(aboutToShowPage(QWidget*)));

  // Output
  parent = m_janus->addVBoxPage(i18n("Output"), i18n("Output"),
                              DesktopIcon("cdwriter_unmount"));
  outputPage = new OutputPage(parent);
  connect(m_janus, SIGNAL(aboutToShowPage(QWidget*)),
          outputPage, SLOT(aboutToShowPage(QWidget*)));

  // then, setup our actions
  setupActions();

  // apply the saved mainwindow settings, if any, and ask the mainwindow
  // to automatically save settings if changed: window size, toolbar
  // position, icon size, etc.
  setAutoSaveSettings();

  // TODO: move these to correct places
  // TODO: make function QPtrList<KMF::Plugins> getPlugins()
  const QObjectList *list = kmfApp->pluginInterface()->children();
  for(QObjectListIt it(*list); (obj=it.current())!= 0; ++it)
    if(obj->inherits("KMF::Plugin"))
      guiFactory()->addClient((KMF::Plugin*)obj);

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
  // File
  KStdAction::openNew(this, SLOT(fileNew()), actionCollection());
  KStdAction::open(this, SLOT(fileOpen()), actionCollection());
  KStdAction::save(this, SLOT(fileSave()), actionCollection());
  KStdAction::saveAs(this, SLOT(fileSaveAs()), actionCollection());
  KStdAction::quit(this, SLOT(quit()), actionCollection());

  // Project
  new KAction(i18n("&Options"), "configure", 0, this,
              SLOT(projectOptions()), actionCollection(), "project_options");
  // KNewStuff
  new KAction(i18n("&Get new tools"), "bookmark", 0, this,
               SLOT(newStuff()), actionCollection(), "newstuff");

  // Settings
  KStdAction::keyBindings(this, SLOT(optionsConfigureKeys()),
                          actionCollection());
  KStdAction::configureToolbars(this, SLOT(optionsConfigureToolbars()),
                                actionCollection());
  KStdAction::preferences(this, SLOT(optionsPreferences()),
                          actionCollection());

#if KDE_IS_VERSION(3,5,0)
  setStandardToolBarMenuEnabled(true);
#else
  m_toolbarAction = KStdAction::showToolbar(this, SLOT(optionsShowToolbar()),
                                            actionCollection());
#endif
  // Media file menu
  new KAction(i18n("&Delete"), "editdelete", Key_Delete, this,
              SLOT(itemDelete()), actionCollection(), "delete" );

  // Testing
#ifdef KMF_TEST
  new KAction(i18n("&Test"), "configure", CTRL + Key_Z, this,
              SLOT(test()), actionCollection(), "test" );
#endif
  //m_statusbarAction = KStdAction::showStatusbar(this,
  //     SLOT(optionsShowStatusbar()), actionCollection());

  createGUI("kmediafactoryui.rc", false);

  updateToolsMenu();
}

void KMediaFactory::connectProject()
{
  connect(kmfApp->project(), SIGNAL(init(const QString&)),
          mediaPage->mediaFiles, SLOT(init(const QString&)));
  connect(kmfApp->project(), SIGNAL(newItem(KMF::Object*)),
          mediaPage->mediaFiles, SLOT(newItem(KMF::Object*)));
  connect(kmfApp->project(), SIGNAL(itemRemoved(KMF::Object*)),
          mediaPage->mediaFiles, SLOT(itemRemoved(KMF::Object*)));
  connect(kmfApp->project(), SIGNAL(newItem(KMF::Object*)),
          mediaPage, SLOT(calculateSizes()));
  connect(kmfApp->project(), SIGNAL(itemRemoved(KMF::Object*)),
          mediaPage, SLOT(calculateSizes()));
  connect(kmfApp->project(), SIGNAL(init(const QString&)),
          mediaPage, SLOT(calculateSizes()));
  connect(kmfApp->project(), SIGNAL(modified(const QString&, bool)),
          this, SLOT(setCaption(const QString&, bool)));

  const KMF::PluginList list = kmfApp->plugins();
  for(KMF::PluginList::ConstIterator obj = list.begin();
      obj != list.end(); ++obj)
  {
    connect(kmfApp->project(), SIGNAL(init(const QString&)),
            *obj, SLOT(init(const QString&)));
  }
}

void KMediaFactory::itemDelete()
{
  KMFIconViewItem* item =
      static_cast<KMFIconViewItem*>(mediaPage->mediaFiles->currentItem());
  if(item)
  {
    KMF::Object *ob = item->ob();
    kmfApp->project()->removeItem(static_cast<KMF::MediaObject*>(ob));
  }
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
  mediaPage->mediaFiles->clear();
  //templatePage->templates->clear();
  templatePage->templatePreview->clear();
  //outputPage->outputs->clear();
  outputPage->progressListView->clear();
  outputPage->progressBar->reset();
  outputPage->showLogPushBtn->setEnabled(false);
  m_janus->showPage(0);
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
  KURL url =
    KFileDialog::getOpenURL(":KMFProject",
                            "*.kmf|KMediaFactory project files");
  if (!url.isEmpty())
    load(url);
}

void KMediaFactory::quit()
{
  close();
}

void KMediaFactory::load(const KURL& url)
{
  if(checkSaveProject())
  {
    resetGUI();
    kmfApp->newProject();
    connectProject();
    kmfApp->project()->open(url);
    templatePage->templates->blockSignals(true);
    templatePage->templates->setCurrentItem(kmfApp->project()->templateObj());
    templatePage->templates->blockSignals(false);
    templatePage->updatePreview();
    outputPage->outputs->blockSignals(true);
    outputPage->outputs->setCurrentItem(kmfApp->project()->output());
    outputPage->outputs->blockSignals(false);
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
  KURL url =
    KFileDialog::getSaveURL(":KMFProject",
                            "*.kmf|KMediaFactory project files");
  if (!url.isEmpty() && url.isValid())
  {
    if(!kmfApp->project()->save(url))
    {
      KMessageBox::error(kmfApp->activeWindow(),
          i18n("Could not write to file: %1").arg(url.prettyURL()));
    }
  }
}

void KMediaFactory::saveProperties(KConfig *config)
{
  if (!kmfApp->url().isEmpty())
  {
#if KDE_IS_VERSION(3,1,3)
    config->writePathEntry("lastURL", kmfApp->url().prettyURL());
#else
    config->writeEntry("lastURL", kmfApp->url().prettyURL());
#endif
  }
}

void KMediaFactory::readProperties(KConfig *config)
{
  QString url = config->readPathEntry("lastURL");

  if (!url.isEmpty())
    load(KURL(url));
}

void KMediaFactory::dragEnterEvent(QDragEnterEvent *event)
{
  event->accept(KURLDrag::canDecode(event));
}

void KMediaFactory::dropEvent(QDropEvent *event)
{
  KURL::List urls;

  if (KURLDrag::decode(event, urls) && !urls.isEmpty())
  {
    const KURL &url = urls.first();
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
#if defined(KDE_MAKE_VERSION)
# if KDE_VERSION >= KDE_MAKE_VERSION(3,1,0)
  saveMainWindowSettings(KGlobal::config(), autoSaveGroup());
# else
  saveMainWindowSettings(KGlobal::config());
# endif
#else
  saveMainWindowSettings(KGlobal::config());
#endif
  KEditToolbar dlg(actionCollection());
  connect(&dlg, SIGNAL(newToolbarConfig()), this, SLOT(newToolbarConfig()));
#if KDE_IS_VERSION(3, 3, 0)
  dlg.setDefaultToolbar("default");
#endif
  dlg.exec();
}

void KMediaFactory::newToolbarConfig()
{
  createGUI("kmediafactoryui.rc", false);
#if defined(KDE_MAKE_VERSION)
# if KDE_VERSION >= KDE_MAKE_VERSION(3,1,0)
    applyMainWindowSettings(KGlobal::config(), autoSaveGroup());
# else
    applyMainWindowSettings(KGlobal::config());
# endif
#else
    applyMainWindowSettings(KGlobal::config());
#endif
}

void KMediaFactory::updateToolsMenu()
{
  KAction* action;
  QPtrList<KAction> actions;
  QPtrList<KAction> media_actions;
  QStringList files =
      KGlobal::dirs()->findAllResources("appdata", "tools/*.desktop");

  unplugActionList("tools_list");
  unplugActionList("media_tools_list");
  for(QStringList::ConstIterator it = files.begin(); it != files.end(); ++it)
  {
    KDesktopFile df(*it, true);

    if (df.readName().isEmpty())
      continue;
    action = new KAction(df.readName(), df.readIcon(), 0, this,
                         SLOT(execTool()), actionCollection(), (*it).latin1());
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
  QString dcopId = kmfApp->dcopClient()->appId();
  QValueList<QCString> envs;

  envs.append(QCString("KMF_DCOP=") + dcopId.latin1());
  envs.append(QCString("KMF_WINID=") +
      QString("%1").arg(kmfApp->mainWidget()->winId()).latin1());
  if(kmfApp->startServiceByDesktopPath(sender()->name(),
     QString::null, envs, &error))
  {
    KMessageBox::error(kapp->activeWindow(),
                       i18n("Error in starting %1: %2")
                           .arg(sender()->name()).arg(error));
  }
}

void KMediaFactory::optionsPreferences()
{
  //An instance of your dialog could be already created and could be cached,
  //in which case you want to display the cached dialog instead of creating
  //another one
  if(!KConfigDialog::showDialog("settings"))
  {
    //KConfigDialog didn't find an instance of this dialog, so lets create it :
    KConfigDialog* dialog = new KConfigDialog(this, "KMediaFactorySettings",
                                              KMediaFactorySettings::self(),
                                              KDialogBase::IconList,
                                              KDialogBase::Ok|
                                              KDialogBase::Cancel);
    QSqlPropertyMap::defaultMap()->insert("KMFLanguageComboBox", "language");
    QSqlPropertyMap::defaultMap()->insert("KMFLanguageListBox", "language");
    QSqlPropertyMap::defaultMap()->insert("KColorCombo", "color");

    dialog->addPage(new KMFOptionsLayout(dialog, "KMediaFactory"),
                    i18n("KMediaFactory"), "kmediafactory");
    dialog->addPage(new Tools(dialog, "KMediaFactory"),
                    i18n("Tools"), "configure");

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

void KMediaFactory::showPage( int page )
{
  m_janus->showPage(page);
}

#include "kmediafactory.moc"
