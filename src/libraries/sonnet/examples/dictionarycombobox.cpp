/*
    Copyright (c) 2008 Volker Krause <vkrause@kde.org>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include "dictionarycombobox.h"

#include <qapplication.h>
#include <QDebug>
#include <QHBoxLayout>
#include <QPushButton>

using namespace Sonnet;

class DictionaryComboBoxTest : public QWidget
{
    Q_OBJECT
public:
    DictionaryComboBoxTest()
    {
        QHBoxLayout *topLayout = new QHBoxLayout(this);
        dcb = new DictionaryComboBox(this);
        topLayout->addWidget(dcb, 1);
        connect(dcb, SIGNAL(dictionaryChanged(QString)), SLOT(dictChanged(QString)));
        connect(dcb, SIGNAL(dictionaryNameChanged(QString)), SLOT(dictNameChanged(QString)));
        QPushButton *btn = new QPushButton(QStringLiteral("Dump"), this);
        topLayout->addWidget(btn);
        connect(btn, SIGNAL(clicked()), SLOT(dump()));
    }

public Q_SLOTS:
    void dump()
    {
        qDebug() << "Current dictionary: " << dcb->currentDictionary();
        qDebug() << "Current dictionary name: " << dcb->currentDictionaryName();
    }

    void dictChanged(const QString &name)
    {
        qDebug() << "Current dictionary changed: " << name;
    }

    void dictNameChanged(const QString &name)
    {
        qDebug() << "Current dictionary name changed: " << name;
    }

private:
    DictionaryComboBox *dcb;
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    DictionaryComboBoxTest *test = new DictionaryComboBoxTest();
    test->show();

    return app.exec();
}

#include "dictionarycombobox.moc"
