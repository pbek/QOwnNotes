# Sonnet

Multi-language spell checker

## Introduction

Sonnet is a plugin-based spell checking library for Qt-based
applications. It supports several different plugins, including
HSpell, Enchant, ASpell and HUNSPELL.

It also supports automated language detection, based on a
combination of different algorithms.

The simplest way to use Sonnet in your application is to use the
SpellCheckDecorator class on your QTextEdit.

## Example

    #include <QTextEdit>
    #include <spellcheckdecorator.h>

    MyFoo::MyFoo(QWidget *parent) : QWidget(parent)
    {
        QTextEdit *textEdit = new QTextEdit(this);
        Sonnet::SpellCheckDecorator *decorator = new Sonnet::SpellCheckDecorator(textEdit);
    }

