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

#include "templatepage.h"
#include "kmediafactory.h"
#include "kmfapplication.h"
#include "kmfimageview.h"
#include <kmftools.h>
#include <KFileItemDelegate>
#include <KPageDialog>
#include <KCursor>
#include <KLocale>
#include <KFileDialog>
#include <KMessageBox>
#include <KDebug>
#include <KXMLGUIFactory>
#include <QTimer>
#include <QCheckBox>
#include <QLabel>
#include <QMenu>

TemplatePage::TemplatePage(QWidget *parent) :
  QWidget(parent), m_menu(0), m_scaled(false)
{
  setupUi(this);
  templates->setSpacing(5);
  templates->setItemDelegate(new KFileItemDelegate(this));
  templates->setIconSize(QSize(KIconLoader::SizeHuge, KIconLoader::SizeHuge));
  connect(templates, SIGNAL(customContextMenuRequested(const QPoint&)),
          this, SLOT(contextMenuRequested(const QPoint&)));
  connect(templatePreview,
          SIGNAL(contextMenuRequested(const QPoint&)),
          this, SLOT(imageContextMenuRequested(const QPoint&)));
}

TemplatePage::~TemplatePage()
{
}

void TemplatePage::projectInit()
{
  templates->setModel(kmfApp->project()->templateObjects());
  connect(templates->selectionModel(),
          SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
          this, SLOT(selectionChanged(const QItemSelection&, const QItemSelection&)));
  templates->blockSignals(true);
  KMF::TemplateObject* obj = kmfApp->project()->templateObj();
  QModelIndex i = kmfApp->project()->templateObjects()->indexOf(obj);
  if(i == QModelIndex())
    i = kmfApp->project()->templateObjects()->index(0);
  templates->selectionModel()->select(i, QItemSelectionModel::ClearAndSelect);
  templates->blockSignals(false);
}

void TemplatePage::selectionChanged(const QItemSelection& selected, 
                                    const QItemSelection& deselected)
{
  if (selected.indexes().count() == 0)
  {
    templates->selectionModel()->select(deselected, QItemSelectionModel::ClearAndSelect);
    return;
  }

  if(kmfApp->project())
  {
    QModelIndex index = selected.indexes()[0];
    KMF::TemplateObject* ob = kmfApp->project()->templateObjects()->at(index);
    if(ob)
    {
      if(ob->clicked() == false)
      {
        m_menu = 0;
        kmfApp->project()->setTemplateObj(ob);
        updatePreview(index.row());
      }
      else
      {
        templates->selectionModel()->select(deselected, QItemSelectionModel::ClearAndSelect);
      }
    }
  }
}

void TemplatePage::currentPageChanged(KPageWidgetItem* current,
                                      KPageWidgetItem*)
{
  if (/*current->parent() == this &&*/isVisible() &&
      (templatePreview->image().size() == QSize(0,0) ||
      m_lastUpdate <
      kmfApp->project()->lastModified(KMF::Media)))
  {
    QTimer::singleShot(0, this, SLOT(updatePreview()));
  }
  // Arranges icon in a nice row. Otherwise icons are arranged in one column
  templates->setViewMode(QListView::IconMode);
}

void TemplatePage::updatePreview(int n)
{
  if(!isVisible())
    return;

  if(n < 0 && templates->selectionModel())
  {
    QModelIndexList selected = templates->selectionModel()->selectedIndexes();
    if(selected.count() > 0)
      n = selected[0].row();
  }

  if(n >= 0 && n < kmfApp->project()->templateObjects()->rowCount())
  {
    QString menu;

    kmfApp->setOverrideCursor(QCursor(Qt::WaitCursor));
    kmfApp->interface()->setUseMessageBox(true);
    kmfApp->interface()->setStopped(false);
    if(kmfApp->project()->mediaObjects()->count() > 0 /*&&
      previewCheckBox->isChecked()*/)
    {
      QModelIndexList selected = templates->selectionModel()->selectedIndexes();
      KMF::TemplateObject* ob =
          kmfApp->project()->templateObjects()->at(selected[0].row());
      QStringList menus = ob->menus();
      if(m_menu < menus.count())
        menu = menus[m_menu];
      else
        menu = "Main";
    }
    KMF::TemplateObject* ob = kmfApp->project()->templateObjects()->at(n);
    // Scale to real 4:3. Should get aspect ratio from template plugin?
    QImage image = ob->preview(menu).scaled(768, 576, Qt::IgnoreAspectRatio,
                                            Qt::SmoothTransformation);
    templatePreview->setImage(image);
    kmfApp->interface()->setUseMessageBox(false);
    kmfApp->restoreOverrideCursor();
    m_lastUpdate = QDateTime::currentDateTime();
  }
}

void TemplatePage::contextMenuRequested(const QPoint &pos)
{
  QModelIndex i = templates->indexAt(pos);

  if(i.row() < 0 || i.row() > kmfApp->project()->templateObjects()->count())
    return;

  KMF::TemplateObject* ob = kmfApp->project()->templateObjects()->at(i.row());
  KMediaFactory* mainWindow = kmfApp->mainWindow();
  KXMLGUIFactory* factory = mainWindow->factory();

  QList<QAction*> actions;
  ob->actions(&actions);
  factory->plugActionList(mainWindow,
      QString::fromLatin1("template_actionlist"), actions);
  QWidget *w = factory->container("template_popup", mainWindow);
  if(w)
  {
    QMenu* popup = static_cast<QMenu*>(w);
    if(popup->actions().count() > 0)
      if(popup->exec(templates->viewport()->mapToGlobal(pos)) != 0)
        updatePreview();
  }
  factory->unplugActionList(mainWindow, "template_actionlist");
}

void TemplatePage::imageContextMenuRequested(const QPoint& pos)
{
  QMenu popup;
  QAction* action;
  QAction* saveAction = new QAction(i18n("Save Image"), this);
  QAction* scaledAction = new QAction(i18n("Scaled"), this);
  int selected = templates->selectionModel()->selection().indexes()[0].row();
  KMF::TemplateObject* ob = kmfApp->project()->templateObjects()->at(selected);
  QStringList menus;
  int i = 0;

  if (ob)
    menus = ob->menus();
  scaledAction->setCheckable(true);
  scaledAction->setChecked(m_scaled);

  popup.addAction(scaledAction);
  popup.addAction(saveAction);
  popup.addSeparator();
  for(QStringList::Iterator it = menus.begin();
      it != menus.end(); ++it, ++i)
  {
    action = new QAction(*it, this);
    action->setCheckable(true);
    action->setData(i);
    action->setChecked(i == m_menu);
    popup.addAction(action);
  }
  if((action = popup.exec(pos)) != 0)
  {
    if(action == scaledAction)
    {
      m_scaled = (m_scaled)?false:true;
      templatePreview->setScaled(m_scaled);
    }
    else if(action == saveAction)
    {
      KUrl url = KFileDialog::getSaveUrl(KUrl("kfiledialog:///<KMFPreview>"),
                                         i18n("*.png|PNG Graphics file"),
                                         kapp->activeWindow());
      if (!url.isEmpty() && url.isValid())
      {
        if(!templatePreview->image().save(url.path(), "PNG", 0))
        {
          KMessageBox::error(kmfApp->activeWindow(),
              i18n("Could not write to file: %1", url.prettyUrl()));
        }
      }
    }
    else
    {
      m_menu = action->data().toInt();
      //previewCheckBox->setChecked(true);
      updatePreview();
    }
  }
}

void TemplatePage::previewClicked()
{
  updatePreview();
}

#include "templatepage.moc"
