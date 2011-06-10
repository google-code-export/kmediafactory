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
    
    cleanAll = new KAction(KIcon("edit-delete"), i18n("Clean Temporary Items"), this);
    plugin()->actionCollection()->addAction("iso_clean_all", cleanAll);
    connect(cleanAll, SIGNAL(triggered()), SLOT(clean()));
}

IsoObject::~IsoObject()
{
}

void IsoObject::actions(QList<QAction *> *actionList) const
{
    actionList->append(cleanAll);
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
        job->projDir = interface()->projectDir("DVD");
        job->title = interface()->title();
        interface()->addJob(job, KMF::All);
    }

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
    
    // Remove all other files...
    KMF::Tools::cleanFiles(interface()->projectDir() + "menus", QStringList() << "*.png" << "*.mpg" << "*.pnm" << "*.xml");
    KMF::Tools::cleanFiles(interface()->projectDir() + "media", QStringList() << "*.mpg" << "*.pnm" << "*.xml");
}

#include "isoobject.moc"
