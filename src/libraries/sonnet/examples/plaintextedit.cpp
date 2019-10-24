// krazy:excludeall=spelling
/**
 * test_plaintextedit.cpp
 *
 * Copyright (C)  2015 Laurent Montel <montel@kde.org>
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
// Local
#include <dictionarycombobox.h>
#include <highlighter.h>
#include <spellcheckdecorator.h>

// Qt
#include <QApplication>
#include <QDebug>
#include <QPlainTextEdit>
#include <QVBoxLayout>

//@@snippet_begin(simple_email_example)
class CommentCheckDecorator : public Sonnet::SpellCheckDecorator
{
public:
    CommentCheckDecorator(QPlainTextEdit *edit)
        : Sonnet::SpellCheckDecorator(edit)
    {
    }

protected:
    bool isSpellCheckingEnabledForBlock(const QString &blockText) const override
    {
        qDebug() << blockText;
        return !blockText.startsWith(QLatin1Char('#'));
    }
};
//@@snippet_end

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QWidget window;

    Sonnet::DictionaryComboBox *comboBox = new Sonnet::DictionaryComboBox;

    //@@snippet_begin(simple_textedit_example)
    QPlainTextEdit *textEdit = new QPlainTextEdit;
    textEdit->setPlainText(QString::fromLatin1("This is a sample buffer. Whih this thingg will "
                                               "be checkin for misstakes. Whih, Enviroment, govermant. Whih.")
                           );

    Sonnet::SpellCheckDecorator *installer = new Sonnet::SpellCheckDecorator(textEdit);
    installer->highlighter()->setCurrentLanguage(QStringLiteral("en"));
    //@@snippet_end

    QObject::connect(comboBox, SIGNAL(dictionaryChanged(QString)), installer->highlighter(),
                     SLOT(setCurrentLanguage(QString)));

    QPlainTextEdit *commentTextEdit = new QPlainTextEdit;
    commentTextEdit->setPlainText(
        QStringLiteral("John Doe said:\n# Hello how aree you?\nI am ffine thanks"));

    installer = new CommentCheckDecorator(commentTextEdit);
    installer->highlighter()->setCurrentLanguage(QStringLiteral("en"));
    QObject::connect(comboBox, SIGNAL(dictionaryChanged(QString)), installer->highlighter(),
                     SLOT(setCurrentLanguage(QString)));

    QVBoxLayout *layout = new QVBoxLayout(&window);
    layout->addWidget(comboBox);
    layout->addWidget(textEdit);
    layout->addWidget(commentTextEdit);

    window.show();
    return app.exec();
}
