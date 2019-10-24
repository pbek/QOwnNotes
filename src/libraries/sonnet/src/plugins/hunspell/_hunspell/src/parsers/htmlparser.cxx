/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * Copyright (C) 2002-2017 Németh László
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Hunspell is based on MySpell which is Copyright (C) 2002 Kevin Hendricks.
 *
 * Contributor(s): David Einstein, Davide Prina, Giuseppe Modugno,
 * Gianluca Turconi, Simon Brouwer, Noll János, Bíró Árpád,
 * Goldman Eleonóra, Sarlós Tamás, Bencsáth Boldizsár, Halácsy Péter,
 * Dvornik László, Gefferth András, Nagy Viktor, Varga Dániel, Chris Halls,
 * Rene Engelhard, Bram Moolenaar, Dafydd Jones, Harri Pitkänen
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <ctype.h>

#include "../hunspell/csutil.hxx"
#include "htmlparser.hxx"

#ifndef W32
using namespace std;
#endif

static const char* PATTERN[][2] = {{"<script", "</script>"},
                                   {"<style", "</style>"},
                                   {"<code", "</code>"},
                                   {"<samp", "</samp>"},
                                   {"<kbd", "</kbd>"},
                                   {"<var", "</var>"},
                                   {"<listing", "</listing>"},
                                   {"<address", "</address>"},
                                   {"<pre", "</pre>"},
                                   {"<!--", "-->"},
                                   {"<[cdata[", "]]>"},  // XML comment
                                   {"<", ">"}};

#define PATTERN_LEN (sizeof(PATTERN) / (sizeof(char*) * 2))

static const char* PATTERN2[][2] = {
    {"<img", "alt="},  // ALT and TITLE attrib handled spec.
    {"<img", "title="},
    {"<a ", "title="}};

#define PATTERN_LEN2 (sizeof(PATTERN2) / (sizeof(char*) * 2))

static const char* (*PATTERN3)[2] = NULL;

#define PATTERN_LEN3 0

HTMLParser::HTMLParser(const char* wordchars)
  : XMLParser(wordchars) {
}

HTMLParser::HTMLParser(const w_char* wordchars, int len)
  : XMLParser(wordchars, len) {
}

bool HTMLParser::next_token(std::string& t) {
  return XMLParser::next_token(PATTERN, PATTERN_LEN, PATTERN2, PATTERN_LEN2, PATTERN3, PATTERN_LEN3, t);
}

HTMLParser::~HTMLParser() {}
