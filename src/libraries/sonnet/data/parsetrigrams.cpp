/**
 * parsetrigrams.cpp
 *
 * Parse a set of trigram files into a QHash, and serialize to stdout.
 *
 * Copyright 2006 Jacob Rideout <kde@jacobrideout.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#include <QString>
#include <QTextStream>
#include <QDir>
#include <QFile>
#include <QHash>
#include <QRegExp>
#include <QDataStream>

int main(int argc, char **argv)
{
    if (argc < 2) {
        return 1;
    }

    QFile sout;
    sout.open(stdout, QIODevice::WriteOnly);
    QDataStream out(&sout);

    QString path = QLatin1String(argv[1]);
    QDir td(path);

    QHash< QString, QHash<QString, int> > models;

    for (const QString &fname : td.entryList(QDir::Files)) {
        QFile fin(td.filePath(fname));
        fin.open(QFile::ReadOnly | QFile::Text);
        QTextStream stream(&fin);
        stream.setCodec("UTF-8");

        while (!stream.atEnd())
        {
            QString line = stream.readLine();
            QRegExp rx(QStringLiteral("(.{3})\\s+(.*)"));
            int pos = rx.indexIn(line);
            if (pos != -1) {
                models[fname][line.left(3)] = rx.cap(2).toInt();
            }
        }
    }

    out << models;
}
