// **************************************************************************
//   Copyright (C) 2006 by Petri Damsten
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
// **************************************************************************

#ifndef RUN_H
#define RUN_H

#include <QtCore/QObject>
#include <QtCore/QProcess>
#include <QtCore/QString>

#include <kdemacros.h>

class KDE_EXPORT Run : public QProcess
{
    Q_OBJECT

    public:
        explicit Run(QString command = QString(), QString dir = QString());
        explicit Run(QStringList command, QString dir = QString());
        ~Run();

        void setCommand(QString command);

        void setCommand(QStringList command);

        bool run();

        QString output()
        {
            return m_output;
        };
        void checkIfScript();

    signals:
        void line(const QString &line);

    protected slots:
        virtual void stdout();

        void exit(int exitCode, QProcess::ExitStatus exitStatus);

    private:
        QString m_program;
        QStringList m_arguments;
        QString m_output;
        int m_outputIndex;
};

#endif
