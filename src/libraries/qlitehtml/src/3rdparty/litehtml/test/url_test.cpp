// Copyright (C) 2020-2021 Primate Labs Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the names of the copyright holders nor the names of their
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "litehtml/url.h"

#include <iostream>

#include <gtest/gtest.h>

using namespace litehtml;

namespace {

struct url_parse_testcase {
    tstring str;
    tstring scheme;
    tstring authority;
    tstring path;
    tstring query;
    tstring fragment;
};

struct url_resolve_testcase {
    tstring base;
    tstring reference;
    tstring expected;
};

} // namespace

TEST(URLTest, DefaultConstructor)
{
    url u;

    EXPECT_TRUE(u.scheme().empty());
    EXPECT_TRUE(u.authority().empty());
    EXPECT_TRUE(u.path().empty());
    EXPECT_TRUE(u.query().empty());
    EXPECT_TRUE(u.fragment().empty());
}

TEST(URLTest, Parse)
{
    std::vector<url_parse_testcase> testcases = {

        // Example from RFC 3986 that includes a scheme, an authority, a path,
        // a query, and a fragment.
        { _t("foo://example.com:8042/over/there?name=ferret#nose"),
            _t("foo"), _t("example.com:8042"), _t("/over/there"), _t("name=ferret"), _t("nose") },

        // Example from RFC 3986 that only includes a scheme and a path.
        { _t("urn:example:animal:ferret:nose"),
            _t("urn"), _t(""), _t("example:animal:ferret:nose"), _t(""), _t("") },

        { _t("http://www.litehtml.com/"),
            _t("http"), _t("www.litehtml.com"), _t("/"), _t(""), _t("") },

        { _t("https://www.slashdot.org/"),
            _t("https"), _t("www.slashdot.org"), _t("/"), _t(""), _t("") },

        { _t("https://www.slashdot.org"),
            _t("https"), _t("www.slashdot.org"), _t(""), _t(""), _t("") },

        { _t("https://news.slashdot.org/story/21/09/24/2157247/"),
            _t("https"), _t("news.slashdot.org"), _t("/story/21/09/24/2157247/"), _t(""), _t("") },

        { _t("https://www.cbc.ca/news/politics/spavor-kovrig-return-1.6189516"),
            _t("https"), _t("www.cbc.ca"), _t("/news/politics/spavor-kovrig-return-1.6189516"), _t(""), _t("") },

        { _t("https://twitter.com/geekbench/status/1412433598200823810"),
            _t("https"), _t("twitter.com"), _t("/geekbench/status/1412433598200823810"), _t(""), _t("") },

        { _t("https://browser.geekbench.com/v5/cpu/search?q=ryzen"),
            _t("https"), _t("browser.geekbench.com"), _t("/v5/cpu/search"), _t("q=ryzen"), _t("") },

        { _t("https://datatracker.ietf.org/doc/html/rfc3986#section-2.2"),
            _t("https"), _t("datatracker.ietf.org"), _t("/doc/html/rfc3986"), _t(""), _t("section-2.2") },

        { _t("file:///home/litehtml/build/hipster.html"),
            _t("file"), _t(""), _t("/home/litehtml/build/hipster.html") },

        { _t("/home/litehtml/Projects/litehtml/build/hipster.html"),
            _t(""), _t(""), _t("/home/litehtml/Projects/litehtml/build/hipster.html") },
    };

    for (auto& testcase : testcases) {
        url u(testcase.str);

        EXPECT_EQ(testcase.scheme, u.scheme());
        EXPECT_EQ(testcase.authority, u.authority());
        EXPECT_EQ(testcase.path, u.path());
        EXPECT_EQ(testcase.query, u.query());
        EXPECT_EQ(testcase.fragment, u.fragment());
    }
}

TEST(URLTest, Build)
{
    std::vector<url_parse_testcase> testcases = {

        // Example from RFC 3986 that includes a scheme, an authority, a path,
        // a query, and a fragment.
        { _t("foo://example.com:8042/over/there?name=ferret#nose"),
            _t("foo"), _t("example.com:8042"), _t("/over/there"), _t("name=ferret"), _t("nose") },

        // Example from RFC 3986 that only includes a scheme and a path.
        { _t("urn:example:animal:ferret:nose"),
            _t("urn"), _t(""), _t("example:animal:ferret:nose"), _t(""), _t("") },

        { _t("http://www.litehtml.com/"),
            _t("http"), _t("www.litehtml.com"), _t("/"), _t(""), _t("") },

        { _t("https://www.slashdot.org/"),
            _t("https"), _t("www.slashdot.org"), _t("/"), _t(""), _t("") },

        { _t("https://www.slashdot.org"),
            _t("https"), _t("www.slashdot.org"), _t(""), _t(""), _t("") },

        { _t("https://news.slashdot.org/story/21/09/24/2157247/"),
            _t("https"), _t("news.slashdot.org"), _t("/story/21/09/24/2157247/"), _t(""), _t("") },

        { _t("https://www.cbc.ca/news/politics/spavor-kovrig-return-1.6189516"),
            _t("https"), _t("www.cbc.ca"), _t("/news/politics/spavor-kovrig-return-1.6189516"), _t(""), _t("") },

        { _t("https://twitter.com/geekbench/status/1412433598200823810"),
            _t("https"), _t("twitter.com"), _t("/geekbench/status/1412433598200823810"), _t(""), _t("") },

        { _t("https://browser.geekbench.com/v5/cpu/search?q=ryzen"),
            _t("https"), _t("browser.geekbench.com"), _t("/v5/cpu/search"), _t("q=ryzen"), _t("") },

        { _t("https://datatracker.ietf.org/doc/html/rfc3986#section-2.2"),
            _t("https"), _t("datatracker.ietf.org"), _t("/doc/html/rfc3986"), _t(""), _t("section-2.2") },

        // Disabled since the url class does not regenerate the same URL for
        // this test case (it does not emit the double slash at the start of
        // the authority).  How do we determine which schemes require the double
        // slash and which ones do not?

        // { _t("file:///home/litehtml/build/hipster.html"),
        //    _t("file"), _t(""), _t("/home/litehtml/build/hipster.html") },

        { _t("/home/litehtml/Projects/litehtml/build/hipster.html"),
            _t(""), _t(""), _t("/home/litehtml/Projects/litehtml/build/hipster.html") },
    };

    for (auto& testcase : testcases) {
        url u(testcase.scheme,
            testcase.authority,
            testcase.path,
            testcase.query,
            testcase.fragment);
        EXPECT_EQ(testcase.str, u.string());
    }

}

TEST(URLTest, Resolve)
{
    std::vector<url_resolve_testcase> testcases = {
        { _t("https://www.twitter.com/"), _t("/foo"),
            _t("https://www.twitter.com/foo") },

        { _t("https://www.twitter.com/"), _t("https://www.facebook.com/"),
            _t("https://www.facebook.com/") },

        { _t("https://www.example.com/index.html"), _t("about.html"),
            _t("https://www.example.com/about.html") },
    };

    for (auto& testcase : testcases) {
        url u = resolve(url(testcase.base), url(testcase.reference));
        url expected(testcase.expected);

        EXPECT_EQ(expected.scheme(), u.scheme());
        EXPECT_EQ(expected.authority(), u.authority());
        EXPECT_EQ(expected.path(), u.path());
        EXPECT_EQ(expected.query(), u.query());
        EXPECT_EQ(expected.fragment(), u.fragment());
    }
}
