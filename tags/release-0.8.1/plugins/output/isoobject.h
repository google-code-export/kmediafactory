#ifndef ISOOBJECT_H
#define ISOOBJECT_H

#include <QtGui/QPixmap>

#include "dvddirectoryobject.h"

class KUrl;
class KAction;

class IsoObject : public DvdDirectoryObject
{
    Q_OBJECT

    public:
        enum { TotalPoints = 100};

        IsoObject(QObject *parent = 0);
        ~IsoObject();

        virtual void toXML(QDomElement *element) const;
        virtual bool fromXML(const QDomElement &element);
        virtual void actions(QList<QAction *> *actionList) const;
        virtual bool prepare(const QString &type);
        virtual bool isValid() const;

    public slots:
        // Help Kross plugin declaring these as slots
        virtual QPixmap pixmap() const;
        void clean();

    private:
        bool isUpToDate(const QString &type);

        bool saveDocument(const KUrl &url);
        bool saveDocumentData(QDomElement *docElem);
        bool saveGeneralDocumentData(QDomElement *part);
        void saveDocumentDataOptions(QDomElement &optionsElem);
        void saveDocumentDataHeader(QDomElement &headerElem);
        void addFiles(const QString &directory, QDomDocument *doc, QDomElement *parent);
        void saveDataItem(const QFileInfo *fi, QDomDocument *doc, QDomElement *parent);

        QString m_rootDir;
        KAction *cleanImage;
};

#endif
