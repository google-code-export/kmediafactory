//**************************************************************************
//   Copyright (C) 2004, 2005 by Petri Damstén
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
#include <kurlrequester.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kdebug.h>
#include <qimage.h>
#include <qslider.h>
#include <qlabel.h>
#include "kmfplayer.h"
#include "kxinewidget.h"
#include "positionslider.h"

KMF::Player::Player(QWidget *parent, const char *name)
 : PlayerLayout(parent, name)
{
  sliderPosition->setRange(0, 65535);
  sliderPosition->setSteps(100, 1000);
  sliderPosition->setTracking(false);
  sliderPosition->setFocusPolicy(QWidget::NoFocus);
}

KMF::Player::~Player()
{
}

void KMF::Player::setURL(const KURL& dvdURL)
{
  url->setURL(dvdURL.prettyURL());
  open();
}

void KMF::Player::checkUrl(const KURL& dvdURL)
{
  QFileInfo fi(dvdURL.path());
  bool dvd = false;

  if(dvdURL.protocol() == "dvd")
  {
    dvd = true;
  }
  else if(fi.isDir())
  {
    QDir dir(fi.filePath() + "/VIDEO_TS");
    if(dir.exists())
      dvd = true;
  }
  else if(fi.filePath().startsWith("/dev/"))
  {
    dvd = true;
  }
  else if(fi.extension().lower() == "iso")
  {
    dvd = true;
  }

  if(!dvd)
  {
    titleMenuButton->hide();
    rootMenuButton->hide();
    selectButton->hide();
    leftButton->hide();
    rightButton->hide();
    downButton->hide();
    upButton->hide();
    rewButton->hide();
    fwdButton->hide();
  }

  if(dvd && dvdURL.protocol() != "dvd")
    url->setURL("dvd://" + dvdURL.path());
}

void KMF::Player::open()
{
  xine->clearQueue();
  checkUrl(url->url());
  xine->appendToQueue(url->url());
  if (!xine->isXineReady())
  {
    if (!xine->initXine())
      return;
  }
  sliderPosition->setPosition(0,false);
  m_pos = QTime();
  updateInfo();
  xine->slotPlay();
}

void KMF::Player::saveFrame()
{
  KURL url = KFileDialog::getSaveURL(":KMFScreenshot",
                                     i18n("*.png|PNG Graphics file"));
  if (!url.isEmpty() && url.isValid())
  {
    if(!xine->getScreenshot().save(url.path(), "PNG", 0))
    {
      KMessageBox::error(kapp->activeWindow(),
          i18n("Could not write to file: %1").arg(url.prettyURL()));
    }
  }
}

void KMF::Player::xineMessage(const QString& msg)
{
  kdDebug() << msg << endl;
}

void KMF::Player::configureFileDialog(KURLRequester* url)
{
  url->fileDialog()->setMode(static_cast<KFile::Mode>(KFile::File |
      KFile::Directory | KFile::ExistingOnly |
      KFile::LocalOnly));
  url->fileDialog()->setFilter("*.mpg *.iso|" +
      i18n("DVD files and directories"));
}

void KMF::Player::nextChapter()
{
  if(xine->isXineReady())
    xine->playNextChapter();
}

void KMF::Player::prevChapter()
{
  if(xine->isXineReady())
    xine->playPreviousChapter();
}

void KMF::Player::updateInfo()
{
  labelInfo->setText(m_pos.toString());
}

void KMF::Player::slotNewPosition(int pos, const QTime& playtime)
{
  QTime length = xine->getLength();
  QTime shortTime(0, 0, 10);

  if (!xine->isSeekable() || length < shortTime || length < playtime)
  {
    sliderPosition->setPosition(0,false);
    sliderPosition->setEnabled(false);
  }
  else
  {
    sliderPosition->setPosition(pos, false);
    sliderPosition->setEnabled(true);
  }

  m_pos = playtime;
  updateInfo();
}

#include "kmfplayer.moc"
