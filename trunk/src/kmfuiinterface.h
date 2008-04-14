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
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************

#ifndef KMFUIINTERFACE_H
#define KMFUIINTERFACE_H

#include <KProgressDialog>
#include <kmediafactory/uiinterface.h>

namespace KMF {
  class Logger;
}

class KMFProgressDialog : public KMF::ProgressDialog
{
    Q_OBJECT
  public: 
    KMFProgressDialog(QWidget* parent);

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

class KMFUiInterface : public KMF::UiInterface
{
    Q_OBJECT
  public:
    KMFUiInterface(QObject *parent);
    virtual ~KMFUiInterface();
    virtual bool addMediaAction(QAction* action,
                                const QString& group = "") const;
    virtual bool addMediaObject(KMF::MediaObject* media) const;
    virtual bool addTemplateObject(KMF::TemplateObject* tob);
    virtual bool addOutputObject(KMF::OutputObject* oob);
    virtual bool removeMediaObject(KMF::MediaObject* media) const;
    virtual bool removeTemplateObject(KMF::TemplateObject* tob);
    virtual bool removeOutputObject(KMF::OutputObject* oob);
    virtual void addMediaObject(const QString& xml);
    virtual void selectTemplate(const QString& xml);
    virtual void selectOutput(const QString& xml);

    virtual bool message(KMF::MsgType type, const QString& msg);
    virtual bool progress(int advance);
    virtual bool setItemTotalSteps(int totalSteps);
    virtual bool setItemProgress(int progress);
    virtual KMF::Logger* logger();
    void setUseMessageBox(bool useMessageBox) { m_useMessageBox = useMessageBox; };
    void setStopped(bool stopped) { m_stopped = stopped; };

    // Plugin helpers
    virtual QStringList getOpenFileNames(const QString &startDir,
                                         const QString &filter,
                                         const QString &caption);
    virtual void debug(const QString &txt);
    virtual int  messageBox(const QString &caption, const QString &txt,
                            int type);
    virtual KMF::ProgressDialog* progressDialog(const QString &caption, const QString &label,
                                                int maximum);
    virtual KMF::ProgressDialog* progressDialog();

  public slots:
    void progressDialogDestroyed();

  private:
    bool m_useMessageBox;
    bool m_stopped;
    KMF::ProgressDialog* m_pdlg;
};

#endif // KMFUIINTERFACE_H
