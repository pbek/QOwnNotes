// krazy:excludeall=spelling
/**
 * test_dialog.cpp
 *
 * Copyright (C)  2004  Zack Rusin <zack@kde.org>
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
#include "dialogexample.h"

#include "backgroundchecker.h"

#include <qapplication.h>
#include <qdebug.h>
using namespace Sonnet;

TestDialog::TestDialog()
    : QObject(nullptr)
{
}

//@@snippet_begin(dictionary_combo_box_example)
void TestDialog::check(const QString &buffer)
{
    Sonnet::Dialog *dlg = new Sonnet::Dialog(
        new BackgroundChecker(this), nullptr);
    connect(dlg, SIGNAL(done(QString)),
            SLOT(doneChecking(QString)));
    dlg->setBuffer(buffer);
    dlg->show();
}

//@@snippet_end

void TestDialog::doneChecking(const QString &buf)
{
    qDebug() << "Done with :" << buf;
    qApp->quit();
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    TestDialog test;
    test.check(QString::fromLatin1("This is a sample buffer. Whih this thingg will "
                                   "be checkin for misstakes. Whih, Enviroment, govermant. Whih.")
               );

    return app.exec();
}
