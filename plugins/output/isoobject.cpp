#include "isoobject.h"
#include <KDebug>
#include <KIconLoader>
#include <KLocale>
#include <KRun>
#include <KStandardDirs>
#include <KUrl>
#include <KAction>
#include <KActionCollection>
#include <QtCore/QFile>
#include <kmftools.h>
#include <outputpluginsettings.h>

static const char * exeName="genisoimage";
static const char startString[] = I18N_NOOP("ISO Image");

class IsoImageJob : public KMF::Job
{
    public:
        QString isoImage;
        QString dvdDir;
        QString projDir;
        QString title;
        uint subid;

        void run()
        {
            subid = msgId();
            message(msgId(), KMF::Start, i18n(startString));
            setMaximum(msgId(), 100);
            KProcess *proc = process(msgId());
            *proc << exeName << "-dvd-video" << "-o" << isoImage << dvdDir;
            proc->setWorkingDirectory(dvdDir);
            proc->execute();
            
            if (success() && OutputPluginSettings::autoClean()) {
                KMF::Tools::cleanFiles(projDir + "DVD/AUDIO_TS");
                KMF::Tools::cleanFiles(projDir + "DVD/VIDEO_TS", QStringList() << "*.VOB" << "*.BUP" << "*.IFO");
                KMF::Tools::cleanFiles(projDir + "DVD");
            }
            message(subid, KMF::Done);
            message(msgId(), KMF::Done);
        }

        void output(const QString &line)
        {
            int percent=line.indexOf("%");

            if(-1!=percent)
            {
                subid = msgId();
                setValue(subid, (int)(line.mid(0, percent).toDouble()));
                
            }
        }
};

IsoObject::IsoObject(QObject *parent)
    : DvdDirectoryObject(parent)
{
    setObjectName("iso");
    setTitle(i18n("ISO Image"));
    
    cleanImage = new KAction(KIcon("edit-delete"), i18n("Clean"), this);
    plugin()->actionCollection()->addAction("iso_clean", cleanImage);
    connect(cleanImage, SIGNAL(triggered()), SLOT(clean()));
}

IsoObject::~IsoObject()
{
}

void IsoObject::actions(QList<QAction *> *actionList) const
{
    actionList->append(cleanImage);
}

bool IsoObject::fromXML(const QDomElement &)
{
    return true;
}

bool IsoObject::isUpToDate(const QString &type)
{
    if (type != interface()->lastSubType()) {
        return false;
    }

    QDateTime lastModified = interface()->lastModified(KMF::Any);
    QFileInfo fi(interface()->projectDir()+ interface()->title().replace("/", ".")+".iso");

    if ((fi.exists() == false) || (lastModified > fi.lastModified())) {
        return false;
    }

    return true;
}

bool IsoObject::prepare(const QString &type)
{
    if (DvdDirectoryObject::prepare(type) == false) {
        return false;
    }

    interface()->message(newMsgId(), KMF::Start, i18n(startString));

    if (isUpToDate(type)) {
        interface()->message(msgId(), KMF::Info, i18n("ISO image is up to date"));
    } else   {
        IsoImageJob *job = new IsoImageJob();
        job->isoImage = interface()->projectDir() + interface()->title().replace("/", ".")+".iso";
        job->dvdDir = interface()->projectDir("DVD");
        job->projDir = interface()->projectDir();
        job->title = interface()->title();
        interface()->addJob(job, KMF::All);
    }
    interface()->message(msgId(), KMF::Done);
    return true;
}

bool IsoObject::isValid() const
{
    return DvdDirectoryObject::isValid() && !KStandardDirs::findExe(exeName).isEmpty();
}

void IsoObject::toXML(QDomElement *) const
{
}

QPixmap IsoObject::pixmap() const
{
    return KIconLoader::global()->loadIcon("media-optical-dvd-video", KIconLoader::NoGroup, KIconLoader::SizeLarge);
}

void IsoObject::clean()
{
    DvdDirectoryObject::clean();
    QFile::remove(interface()->projectDir() + interface()->title().replace("/", ".")+".iso");
}

#include "isoobject.moc"
