/**
 * test_highlighter.cpp
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
#include "highlighter.h"
#include "highlighterexample.h"

#include <qapplication.h>
#include <QDebug>
#include <QTextEdit>
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>

TestSpell::TestSpell()
    : QTextEdit()
{
    hl = new Sonnet::Highlighter(this);
}

void TestSpell::contextMenuEvent(QContextMenuEvent *e)
{
    qDebug() << "TestSpell::contextMenuEvent";
    QMenu *popup = createStandardContextMenu();
    QMenu *subMenu = new QMenu(popup);
    subMenu->setTitle(QStringLiteral("Text highlighting"));
    connect(subMenu, SIGNAL(triggered(QAction*)), this, SLOT(slotActivate()));
    QAction *action = new QAction(QStringLiteral("active or not"), popup);
    popup->addSeparator();
    popup->addMenu(subMenu);
    subMenu->addAction(action);
    popup->exec(e->globalPos());
    delete popup;
}

void TestSpell::slotActivate()
{
    qDebug() << "Activate or not highlight :";
    hl->setActive(!hl->isActive());
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QTextEdit *test = new TestSpell();
    test->show();

    return app.exec();
}
