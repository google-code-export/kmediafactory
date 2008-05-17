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

#include "k3bobject.h"
#include <kmediafactory/job.h>
#include <kstore/KoStore.h>
#include <kstore/KoStoreDevice.h>
#include <KApplication>
#include <KAboutData>
#include <KLocale>
#include <KStandardDirs>
#include <KIconLoader>
#include <KUrl>
#include <KRun>
#include <QDir>
#include <QPixmap>
#include <QTextStream>
#include <QDomDocument>

class K3bProjectJob : public KMF::Job
{
public:
  QString cmd;
  QString doc;
  QString dvdDir;
  QString title;

  void run()
  {
    message(KMF::Start, i18n("Making K3b project."));
    saveDocument(KUrl(doc));
    if (!cmd.isEmpty())
    {
      cmd += " " + doc;
      KRun::runCommand(cmd, kapp->activeWindow());
    }
    message(KMF::Done);
  }

  // All the following are taken from k3b project
  
  bool saveDocument(const KUrl& url)
  {
    // create the store
    KoStore* store = KoStore::createStore(url.path(), KoStore::Write,
                                          "application/x-k3b");
    if(!store)
      return false;
  
    if(store->bad())
    {
      delete store;
      return false;
    }
  
    // open the document inside the store
    store->open("maindata.xml");
  
    // save the data in the document
    QDomDocument xmlDoc("k3b_video_dvd_project");
  
    xmlDoc.appendChild(xmlDoc.createProcessingInstruction( "xml",
                      "version=\"1.0\" encoding=\"UTF-8\""));
    QDomElement docElem = xmlDoc.createElement("k3b_video_dvd_project");
    xmlDoc.appendChild(docElem);
  
    bool success = saveDocumentData(&docElem);
    if(success)
    {
      KoStoreDevice dev(store);
      dev.open(QIODevice::WriteOnly);
      QTextStream xmlStream(&dev);
      xmlStream.setCodec("UTF-8");
      xmlDoc.save(xmlStream, 0);
    }
  
    // close the document inside the store
    store->close();
  
    // remove the store (destructor writes the store to disk)
    delete store;
    return success;
  }
  
  bool saveDocumentData(QDomElement* docElem)
  {
    QDomDocument doc = docElem->ownerDocument();
  
    saveGeneralDocumentData(docElem);
  
    // all options
    // ----------------------------------------------------------------------
    QDomElement optionsElem = doc.createElement("options");
    saveDocumentDataOptions(optionsElem);
    docElem->appendChild(optionsElem);
    // ----------------------------------------------------------------------
  
    // the header stuff
    // ----------------------------------------------------------------------
    QDomElement headerElem = doc.createElement("header");
    saveDocumentDataHeader(headerElem);
    docElem->appendChild(headerElem);
  
    // now do the "real" work: save the entries
    // ----------------------------------------------------------------------
    QDomElement topElem = doc.createElement("files");
    addFiles(dvdDir, &doc, &topElem);
    docElem->appendChild(topElem);
    // ----------------------------------------------------------------------
  
    return true;
  }
  
  bool saveGeneralDocumentData(QDomElement* part)
  {
    QDomDocument doc = part->ownerDocument();
    QDomElement mainElem = doc.createElement( "general" );
    /*
    QDomElement propElem = doc.createElement( "writing_mode" );
    switch( writingMode() ) {
      case K3b::DAO:
        propElem.appendChild( doc.createTextNode( "dao" ) );
        break;
      case K3b::TAO:
        propElem.appendChild( doc.createTextNode( "tao" ) );
        break;
      case K3b::RAW:
        propElem.appendChild( doc.createTextNode( "raw" ) );
        break;
      default:
        propElem.appendChild( doc.createTextNode( "auto" ) );
        break;
    }
    mainElem.appendChild( propElem );
  
    propElem = doc.createElement( "dummy" );
    propElem.setAttribute( "activated", dummy() ? "yes" : "no" );
    mainElem.appendChild( propElem );
  
    propElem = doc.createElement( "on_the_fly" );
    propElem.setAttribute( "activated", onTheFly() ? "yes" : "no" );
    mainElem.appendChild( propElem );
  
    propElem = doc.createElement( "only_create_images" );
    propElem.setAttribute( "activated", onlyCreateImages() ? "yes" : "no" );
    mainElem.appendChild( propElem );
  
    propElem = doc.createElement( "remove_images" );
    propElem.setAttribute( "activated", removeImages() ? "yes" : "no" );
    mainElem.appendChild( propElem );
    */
    part->appendChild( mainElem );
  
    return true;
  }
  
  void saveDocumentDataOptions(QDomElement&)
  {
    /*
    QDomDocument doc = optionsElem.ownerDocument();
    QDomElement topElem = doc.createElement( "rock_ridge" );
    topElem.setAttribute( "activated", isoOptions().createRockRidge() ? "yes" :
        "no" );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "joliet" );
    topElem.setAttribute( "activated", isoOptions().createJoliet() ? "yes" : "no"
                        );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "udf" );
    topElem.setAttribute( "activated", isoOptions().createUdf() ? "yes" : "no" );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "iso_allow_lowercase" );
    topElem.setAttribute( "activated", isoOptions().ISOallowLowercase() ? "yes" :
        "no" );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "iso_allow_period_at_begin" );
    topElem.setAttribute( "activated", isoOptions().ISOallowPeriodAtBegin() ?
        "yes" : "no" );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "iso_allow_31_char" );
    topElem.setAttribute( "activated", isoOptions().ISOallow31charFilenames() ?
        "yes" : "no" );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "iso_omit_version_numbers" );
    topElem.setAttribute( "activated", isoOptions().ISOomitVersionNumbers() ?
        "yes" : "no" );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "iso_omit_trailing_period" );
    topElem.setAttribute( "activated", isoOptions().ISOomitTrailingPeriod() ?
        "yes" : "no" );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "iso_max_filename_length" );
    topElem.setAttribute( "activated", isoOptions().ISOmaxFilenameLength() ? "yes"
    : "no" );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "iso_relaxed_filenames" );
    topElem.setAttribute( "activated", isoOptions().ISOrelaxedFilenames() ? "yes"
    : "no" );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "iso_no_iso_translate" );
    topElem.setAttribute( "activated", isoOptions().ISOnoIsoTranslate() ? "yes" :
        "no" );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "iso_allow_multidot" );
    topElem.setAttribute( "activated", isoOptions().ISOallowMultiDot() ? "yes" :
        "no" );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "iso_untranslated_filenames" );
    topElem.setAttribute( "activated", isoOptions().ISOuntranslatedFilenames() ?
        "yes" : "no" );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "follow_symbolic_links" );
    topElem.setAttribute( "activated", isoOptions().followSymbolicLinks() ? "yes"
    : "no" );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "create_trans_tbl" );
    topElem.setAttribute( "activated", isoOptions().createTRANS_TBL() ? "yes" :
        "no" );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "hide_trans_tbl" );
    topElem.setAttribute( "activated", isoOptions().hideTRANS_TBL() ? "yes" : "no"
                        );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "iso_level" );
    topElem.appendChild( doc.createTextNode(
        QString::number(isoOptions().ISOLevel()) ) );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "discard_symlinks" );
    topElem.setAttribute( "activated", isoOptions().discardSymlinks() ? "yes" :
        "no" );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "discard_broken_symlinks" );
    topElem.setAttribute( "activated", isoOptions().discardBrokenSymlinks() ?
        "yes" : "no" );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "preserve_file_permissions" );
    topElem.setAttribute( "activated", isoOptions().preserveFilePermissions() ?
        "yes" : "no" );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "force_input_charset" );
    topElem.setAttribute( "activated", isoOptions().forceInputCharset() ? "yes" :
        "no" );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "input_charset" );
    topElem.appendChild( doc.createTextNode( isoOptions().inputCharset() ) );
    optionsElem.appendChild( topElem );
  
  
    topElem = doc.createElement( "whitespace_treatment" );
    switch( isoOptions().whiteSpaceTreatment() ) {
      case K3bIsoOptions::strip:
        topElem.appendChild( doc.createTextNode( "strip" ) );
        break;
      case K3bIsoOptions::extended:
        topElem.appendChild( doc.createTextNode( "extended" ) );
        break;
      case K3bIsoOptions::replace:
        topElem.appendChild( doc.createTextNode( "replace" ) );
        break;
      default:
        topElem.appendChild( doc.createTextNode( "noChange" ) );
        break;
    }
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "whitespace_replace_string" );
    topElem.appendChild( doc.createTextNode(
        isoOptions().whiteSpaceTreatmentReplaceString() ) );
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "data_track_mode" );
    if( m_dataMode == K3b::MODE1 )
      topElem.appendChild( doc.createTextNode( "mode1" ) );
    else if( m_dataMode == K3b::MODE2 )
      topElem.appendChild( doc.createTextNode( "mode2" ) );
    else
      topElem.appendChild( doc.createTextNode( "auto" ) );
    optionsElem.appendChild( topElem );
  
  
    // save multisession
    topElem = doc.createElement( "multisession" );
    switch( m_multisessionMode ) {
    case START:
      topElem.appendChild( doc.createTextNode( "start" ) );
      break;
    case CONTINUE:
      topElem.appendChild( doc.createTextNode( "continue" ) );
      break;
    case FINISH:
      topElem.appendChild( doc.createTextNode( "finish" ) );
      break;
    default:
      topElem.appendChild( doc.createTextNode( "none" ) );
      break;
    }
    optionsElem.appendChild( topElem );
  
    topElem = doc.createElement( "verify_data" );
    topElem.setAttribute( "activated", verifyData() ? "yes" : "no" );
    optionsElem.appendChild( topElem );
    // ----------------------------------------------------------------------
    */
  }
  
  void saveDocumentDataHeader(QDomElement& headerElem)
  {
    QString app = i18n("%1 - Version %2"
        , KGlobal::mainComponent().aboutData()->programName()
        , KGlobal::mainComponent().aboutData()->version());
  
    QDomDocument doc = headerElem.ownerDocument();
    QDomElement topElem = doc.createElement("volume_id");
    topElem.appendChild(doc.createTextNode(title));
    headerElem.appendChild(topElem);
    /*
    topElem = doc.createElement( "volume_set_id" );
    topElem.appendChild( doc.createTextNode( isoOptions().volumeSetId() ) );
    headerElem.appendChild( topElem );
  
    topElem = doc.createElement( "volume_set_size" );
    topElem.appendChild( doc.createTextNode(
        QString::number(isoOptions().volumeSetSize()) ) );
    headerElem.appendChild( topElem );
  
    topElem = doc.createElement( "volume_set_number" );
    topElem.appendChild( doc.createTextNode(
        QString::number(isoOptions().volumeSetNumber()) ) );
    headerElem.appendChild( topElem );
  
    topElem = doc.createElement( "system_id" );
    topElem.appendChild( doc.createTextNode( isoOptions().systemId() ) );
    headerElem.appendChild( topElem );
  
    topElem = doc.createElement( "application_id" );
    topElem.appendChild( doc.createTextNode( isoOptions().applicationID() ) );
    headerElem.appendChild( topElem );
  
    topElem = doc.createElement( "publisher" );
    topElem.appendChild( doc.createTextNode( isoOptions().publisher() ) );
    headerElem.appendChild( topElem );
    */
    topElem = doc.createElement("preparer");
    topElem.appendChild(doc.createTextNode(app));
    headerElem.appendChild(topElem);
    // ----------------------------------------------------------------------
  }
  
  void addFiles(const QString& directory, QDomDocument* doc,
                          QDomElement* parent)
  {
    QDir dir(directory);
  
    if(!dir.exists())
      return;
    const QFileInfoList& files = dir.entryInfoList();
    foreach(QFileInfo file, files)
    {
      if(file.fileName() == "." || file.fileName() == "..")
        continue;
  
      saveDataItem(&file, doc, parent);
    }
  }
  
  void saveDataItem(const QFileInfo* fi, QDomDocument* doc,
                              QDomElement* parent)
  {
    if(fi->isFile())
    {
      QString file = fi->absoluteFilePath();
      // K3b can't handle relative paths ?
      //KUrl::relativePath(interface()->projectDir(), fi->absFilePath());
      QDomElement topElem = doc->createElement("file");
      topElem.setAttribute("name", fi->fileName());
      QDomElement subElem = doc->createElement("url");
      subElem.appendChild(doc->createTextNode(file));
      topElem.appendChild(subElem);
      parent->appendChild(topElem);
    }
    else
    {
      QDomElement topElem = doc->createElement( "directory" );
      topElem.setAttribute("name", fi->fileName());
      addFiles(fi->absoluteFilePath(), doc, &topElem);
      parent->appendChild(topElem);
    }
  }
};

K3bObject::K3bObject(QObject *parent)
 : DvdDirectoryObject(parent)
{
  setObjectName("k3b");
  setTitle(i18n("K3b Project"));
}

K3bObject::~K3bObject()
{
}

void K3bObject::actions(QList<QAction*>*) const
{
}

bool K3bObject::fromXML(const QDomElement&)
{
  return true;
}

int K3bObject::timeEstimate() const
{
  return DvdDirectoryObject::timeEstimate() + TotalPoints;
}

bool K3bObject::make(QString type)
{
  if(DvdDirectoryObject::make(type) == false)
    return false;

  interface()->message(KMF::Start, i18n("K3B Project"));
  K3bProjectJob *job = new K3bProjectJob();
  job->cmd = KStandardDirs::findExe("k3b");
  job->doc = interface()->projectDir() + "dvd.k3b";
  job->dvdDir = interface()->projectDir("DVD");
  job->title = interface()->title();
  interface()->addJob(job, KMF::Last);
  interface()->message(KMF::Done);
  return true;
}

void K3bObject::toXML(QDomElement*) const
{
}

QPixmap K3bObject::pixmap() const
{
  return KIconLoader::global()->loadIcon("k3b", KIconLoader::NoGroup,
                                         KIconLoader::SizeLarge);
}

#include "k3bobject.moc"
