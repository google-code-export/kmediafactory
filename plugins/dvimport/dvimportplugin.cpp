//**************************************************************************
//   Copyright (C) 2004 by Petri Damst�
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
#include "config.h"
#include "dvimportplugin.h"
#include "dvimportpluginsettings.h"
#include "progresslayout.h"
#include <videoobject.h>
#include <videopluginsettings.h>
#include <kmftime.h>
#include <kgenericfactory.h>
#include <kdeversion.h>
#include <kapplication.h>
#include <kaction.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kaboutdata.h>
#include <kpushbutton.h>
#include <qeventloop.h>
#include <qprogressbar.h>
#include <qcheckbox.h>
#include <libdv/dv.h>
#include <libdv/dv_types.h>

static const char description[] =
  I18N_NOOP("DV Import plugin for KMediaFactory.");
static const char version[] = VERSION;
static const KAboutData about("kmediafactory_dvimport",
                              I18N_NOOP("KMediaFactory DV Import"),
                              version, description, KAboutData::License_GPL,
                              "(C) 2005 Petri Damsten", 0, 0,
                              "petri.damsten@iki.fi");

typedef KGenericFactory<DVImportPlugin> dvimportFactory;
#if KDE_IS_VERSION(3, 3, 0)
K_EXPORT_COMPONENT_FACTORY(kmediafactory_dvimport, dvimportFactory(&about))
#else
K_EXPORT_COMPONENT_FACTORY(kmediafactory_dvimport,
    dvimportFactory(about.appName()))
#endif

DVImportPlugin::DVImportPlugin(QObject *parent,
                         const char* name, const QStringList&) :
  KMF::Plugin(parent, name )
{
  // Initialize GUI
  setInstance(KGenericFactory<DVImportPlugin>::instance());
  setXMLFile("kmediafactory_dvimportui.rc");
  // Add action for menu item
  addDVAction = new KAction(i18n("Add DV"), "camera_unmount", CTRL+Key_D,
                               this, SLOT(slotAddDV()), actionCollection(),
                               "dv");
}

void DVImportPlugin::init(const QString &type)
{
  deleteChildren();
  if (type.left(3) == "DVD")
  {
    addDVAction->setEnabled(true);
  }
  else
  {
    addDVAction->setEnabled(false);
  }
}

void DVImportPlugin::slotAddDV()
{
  QCheckBox* multipleFiles = new QCheckBox(0, "multipleFiles");
  KFileDialog dlg(":AddDV", "*.dv |DV video files",
                  kapp->mainWidget(), "filedialog", true, multipleFiles);

  multipleFiles->setText(i18n("Multiple files make multiple titles."));
  multipleFiles->setChecked(false);
  dlg.setOperationMode(KFileDialog::Opening);
  dlg.setCaption(i18n("Open"));
  dlg.setMode(KFile::Files | KFile::LocalOnly);
  dlg.exec();

  QStringList filenames = dlg.selectedFiles();
  KMF::UiInterface *m = uiInterface();

  if(m && filenames.count() > 0)
  {
    VideoObject* vob = 0;
    for(QStringList::ConstIterator filename = filenames.begin();
        filename != filenames.end(); ++filename)
    {
      QFileInfo fi(*filename);

      if(fi.isDir())
      {
        KMessageBox::error(kapp->activeWindow(),
                           i18n("Cannot add directory."));
        continue;
      }
      if(multipleFiles->isChecked() || filename == filenames.begin())
        vob = new VideoObject(this);
      if(parseDV(vob, *filename) == false)
      {
        delete vob;
        return;
      }
      vob->setTitleFromFileName();
      if(multipleFiles->isChecked() || filename == filenames.fromLast())
        m->addMediaObject(vob);
    }
  }
}

KMF::MediaObject* DVImportPlugin::createMediaObject(const QDomElement& element)
{
  KMF::MediaObject *mob = new VideoObject(this);
  if(mob)
    mob->fromXML(element);
  return mob;
}

void DVImportPlugin::cancel()
{
  m_cancel = true;
}

int DVImportPlugin::readFrame(QFile& video, char *frameBuffer, bool& isPAL)
{
  if(video.readBlock(frameBuffer, 120000) != 120000)
  {
    return 0;
  }

  isPAL = (frameBuffer[3] & 0x80);

  if (isPAL)
  {
    if (video.readBlock(frameBuffer + 120000, 144000 - 120000) !=
        144000 - 120000)
    {
      return 0;
    }
  }
  return 1;
}

bool DVImportPlugin::parseDV(VideoObject* vob, QString fileName)
{
  KMF::Time offset = vob->duration();
  int index = vob->files().count();
  vob->addFile(fileName);

  QDir dir(projectInterface()->projectDir("media"));
  QFileInfo fi(fileName);
  QFile sub(vob->videoFileName(index, VideoObject::PrefixMicroDVD));
  QFile in(fileName);
  uint8_t dvBuffer[144000];
  // assume NTSC for now, switch to PAL later if needed
  dv_decoder_t *decoder = dv_decoder_new(FALSE, FALSE, FALSE);
  int i = 0;
  bool isPAL = false;
  int lastframe = 1;
  i = 0;
  int framenum = 0;
  struct tm recDate;
  QDateTime date;
  QDateTime lastDate(QDate(1800, 1, 1));
  QDateTime lastCell(QDate(1800, 1, 1));
  QString subFormat = DVImportPluginSettings::subtitleFormat();
  int minChapLength = DVImportPluginSettings::minChapterLength();
  int chapterOffset = DVImportPluginSettings::chapterOffset();
  ProgressLayout progressDlg;
  //KMF::Time duration = KMF::Time(script.output());
#warning TODO
  int frames = 0; //(int)(duration.toSeconds() * input.frameRate());
  QDVD::AudioList audioTracks;
  QDVD::CellList cells;

  m_cancel = false;

  progressDlg.show();
  connect(progressDlg.cancelButton, SIGNAL(clicked()),
          this, SLOT(cancel()));
  progressDlg.progressBar->setTotalSteps(frames);

  if(index > 0)
    cells = vob->cellList();
  if(decoder)
  {
    if(in.open(IO_ReadOnly))
    {
      if(sub.open(IO_WriteOnly))
      {
        QTextStream subStream(&sub);

        while(readFrame(in, (char*)dvBuffer, isPAL) && !m_cancel)
        {
          ++framenum;
          // dv_parse_header(decoder, dvBuffer);
          dv_parse_packs(decoder, dvBuffer);
          dv_get_recording_datetime_tm(decoder, &recDate);
          /*
          kdDebug()
          << " : " << recDate.tm_year
          << " : " << recDate.tm_mon
          << " : " << recDate.tm_mday
          << " : " << recDate.tm_hour
          << " : " << recDate.tm_min
          << " : " << recDate.tm_sec << endl;
          */
          date.setTime_t(mktime(&recDate));
          // False dates ?
          if(date < lastDate && date.time() == QTime(0,0))
            continue;
          // New take
          if(date.addSecs(-1 * minChapLength) > lastCell)
          {
            lastCell = date;
            QDVD::Cell c;
            if(framenum > 1)
            {
#warning TODO /*
              KMF::Time start((double)(framenum + chapterOffset) /
                  input.frameRate());
              c.setStart(start + offset);*/
            }
            else
              c.setStart(offset);
            c.setName(date.toString(subFormat));
            cells.append(c);
          }
          if(framenum == 1)
            lastDate = date;

          progressDlg.progressBar->setProgress(framenum);
          kapp->eventLoop()->processEvents(QEventLoop::AllEvents);
          // New second or take
          if(lastDate != date)
          {
            lastDate = date;
            subStream << "{" << lastframe << "}{" << framenum - 1
                << "} " << lastDate.toString(subFormat) << endl;
            lastframe = framenum;
          }
        }
        subStream << "{" << lastframe << "}{" << framenum
            << "} " << lastDate.toString(subFormat) << endl;
        sub.close();

        QDVD::SubtitleList list = vob->subtitles();

        if(list.count() > 0)
        {
          list[0].setFile(list[0].file() + ";" + sub.name());
        }
        else
        {
          QDVD::Subtitle subtitle;
          subtitle.setFile(sub.name());
          subtitle.setLanguage(
              VideoPluginSettings::defaultSubtitleLanguage());
          subtitle.setAlignment(Qt::AlignmentFlags(Qt::AlignRight |
              Qt::AlignBottom));
          list.append(subtitle);
        }
        vob->setSubtitles(list);
      }
      in.close();
    }
    dv_decoder_free(decoder);
    audioTracks.append(
        QDVD::AudioTrack(VideoPluginSettings::defaultAudioLanguage()));
    vob->setAudioTracks(audioTracks);
    vob->setCellList(cells);
    vob->parseCellLengths();
  }
  return !m_cancel;
}

QStringList DVImportPlugin::supportedProjectTypes()
{
  QStringList result;
  result << "DVD-PAL" << "DVD-NTSC";
  return result;
}

#include "dvimportplugin.moc"
