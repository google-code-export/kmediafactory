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

#ifndef _KMEDIAFACTORY_H_
#define _KMEDIAFACTORY_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <KXmlGuiWindow>
#include <KDirWatch>

// Define this for the test menu item
// #define KMF_TEST

class ProjectOptions;
class MediaPage;
class TemplatePage;
class OutputPage;
class KMFMediaInterface;
class KUrl;
class KToggleAction;
class KPageWidget;
class KMFNewStuff;
class KPageWidgetItem;

/**
 * @short Application Main Window
 * @author Petri Damsten <petri.damsten@iki.fi>
 * @version 0.1
 */
class KMediaFactory : public KXmlGuiWindow
{
    Q_OBJECT
    friend class KMFApplication;
  public:
    enum Page { Project=0, Media , Template, Output };

    /**
     * Default Constructor
     */
    KMediaFactory();

    /**
     * Default Destructor
     */
    virtual ~KMediaFactory();

    /**
     * setup menu actions
     */
    void setupActions();
    void enableUi(bool enabled);
    void showPage(int page);

    ProjectOptions *projectPage;
    MediaPage *mediaPage;
    TemplatePage *templatePage;
    OutputPage *outputPage;

  protected:
    virtual bool queryClose();
    /**
     * Overridden virtuals for Qt drag 'n drop (XDND)
     */
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);

    /**
     * This function is called when it is time for the app to save its
     * properties for session management purposes.
     */
    void saveProperties(KConfigGroup&);

    /**
     * This function is called when this app is restored.  The KConfig
     * object points to the session management config file that was saved
     * with @ref saveProperties
     */
    void readProperties(const KConfigGroup&);

  public slots:
    void fileNew();
    void quit();
    void load(const KUrl&);
    void setProjectTitle(const QString& title);
    void setProjectType(const QString& type);
    void setProjectDirectory(const QString& dir);

  protected slots:
    void initGUI();

  private slots:
    void fileOpen();
    void newStuff();
    void fileSave();
    void fileSaveAs();
    void itemDelete();
    void execTool();
    void optionsConfigureKeys();
    void optionsConfigureToolbars();
    void optionsPreferences();
    void newToolbarConfig();
    void optionsShowToolbar();
    void updateToolsMenu();
#ifdef KMF_TEST
    //void test();
#endif

  private:
    KToggleAction* m_toolbarAction;
    KPageWidget* m_janus;
    QWidget* m_janusIconList;
    bool m_enabled;
    KMFNewStuff* m_newStuffDlg;
    KPageWidgetItem* m_projectPageItem;
    KPageWidgetItem* m_mediaPageItem;
    KPageWidgetItem* m_templatePageItem;
    KPageWidgetItem* m_ouputPageItem;
    KDirWatch m_toolsWatch;

    void connectProject();
    void resetGUI();
    bool checkSaveProject();
};

#endif // _KMEDIAFACTORY_H_
