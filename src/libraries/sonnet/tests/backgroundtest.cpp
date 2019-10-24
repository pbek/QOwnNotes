/**
 * backgroundtest.cpp
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
#include "backgroundtest.h"

#include "speller.h"
using namespace Sonnet;

#include <qapplication.h>
#include <qdebug.h>

const char *text
    =
        "Rationale \
========= \
 \
This code is intended to provide an implementation of the W3C's XPath \
specification for KHTML. XPath isn't particularly useful on its own, however\
it is an essential building block for the implementation of other standards \
like XSLT and XQuery. XPath is supported to a greater or lesser extent by both\
IE and Mozilla so it is likely to become increasingly important over the next\
few years.\
 \
Why write another XPath implementation? \
======================================= \
 \
The are already a number of XPath implementations available under free \
licenses including Mozilla's, libxml2, Xerces and probably others, so it is \
reasonable to ask why there should be another one. \
 \
It would certainly be possible to integrate one of these implementations into\
KHTML, but it would actually be quite a lot of work. I looked at all of the\
implementations mentioned with a view to using this approach before I decided\
to start from scratch.\
 \
Mozilla XPath\
-------------\
 \
Seems to be incomplete, and though the code was originally standalone it now\
seems to be tied to the mozilla codebase quite tightly. This makes porting it\
a lot of work, and makes working with the mozilla team on fixing bugs\
etc. hard as the code bases would have diverged quite a lot.\
 \
Xerces XPath (C++ version)\
--------------------------\
 \
The Xerces code seemed pretty clean and was reasonably understandable, however\
it doesn't seem to be used very much which greatly reduces the utility. As\
with the mozilla code, porting it to use KHTML's DOM would take a fair bit of \
work. The main issue here being that Xerces is based around pointers to Nodes\
rather than implicitly shared Node objects.\
 \
libxml2 \
------- \
 \
This is the most obvious library to reuse as it is currently used to generate\
the KDE documentation, and is also a very complete and fast\
implementation. The down side of using this code is that it would either need\
"
        "a new DOM implementation in KHTML (which used the libxml2 structures), a \
wrapper library that made on of the DOM trees support the API of the other, or\
binding layer that parsed the XML twice and somehow maintained a mapping\
between the two DOM trees. Unfortunately the documentation of this library is\
less than great, which would add to the problems.\
 \
The C++ wrappers to libxml2 are considerably closer to what I was looking\
for. They are well documented and have a well structured API. Unfortunately\
using this library still requires some mechanism to integrate the two\
underlying DOM implementations.\
 \
KHTML XPath\
----------- \
 \
There are some advantages to the XPath implementation Zack and I are working\
on, namely: \
 \
- Ease of integration with the rest of kjs/khtml.\
- Use of dom2 traversal (which will also be available to use directly).\
- C++ rather than C and a wrapper (reducing the overheads).\
- The code is clean and uses familiar types and idioms. \
 \
We intend the code to be build on top of the DOM api rather than tying it\
directly to the Qt or KHTML XML implementations. This will allow us to take\
advantage of any improvements that might be made to the underlying parser\
etc. The DOM2 traversal APIs provide a set of classes that map almost directly \
to the XPath location steps, since we need to implement these facilities\
anyway writing the relatively small amount of code needed to support XPath\
seems sensible.\
 \
Building \
========\
 \
This code needs to live in a subdir off the khtml directory in kdelibs. The\
subdir should be called 'xpath'. The easiest way to regenerate the makefiles\
is to go to the root of the kdelibs tree and run: \
  create_makefiles khtml/xpath\
 \
This code is intended to compile, but not to work.\
 \
Usage \
===== \
 \
./test_xpath simple.xml\
./test_values\
./test_functions \
 \
Notes\
===== \
 \
apidoc                  Duh! It's the docs \
working                 Stuff that I'm mining for ideas\
\
Discussion\
========== \
 \
If you want to talk about this code feel free to mail us.";

BackgroundTest::BackgroundTest()
    : QObject(0)
    , m_speller("en")
{
    m_checker = new BackgroundChecker(m_speller, this);
    connect(m_checker, SIGNAL(done()),
            SLOT(slotDone()));
    connect(m_checker, SIGNAL(misspelling(QString,int)),
            SLOT(slotMisspelling(QString,int)));
    m_len = strlen(text);
    m_checker->setText(text);
    m_checker->speller().setLanguage("en");
    m_timer.start();
}

void BackgroundTest::slotDone()
{
    qDebug() << "Text of length" << m_len << "checked in" << m_timer.elapsed() << "msec.";
    QApplication::exit();
}

void BackgroundTest::slotMisspelling(const QString &word, int start)
{
    qDebug() << "Misspelling" << word << "at" << start;
    m_checker->continueChecking();
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    BackgroundTest test;

    return app.exec();
}
