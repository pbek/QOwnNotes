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

#include "litehtml/tstring_view.h"

#include <iostream>

#include <gtest/gtest.h>

using namespace litehtml;

TEST(TStringViewTest, DefaultConstructor)
{
    tstring_view view;

    EXPECT_EQ(nullptr, view.data());
    EXPECT_EQ(0, view.size());
    EXPECT_TRUE(view.empty());
}

TEST(TStringViewTest, Constructor)
{
    constexpr size_t offset = 5;
    constexpr size_t length = 10;

    tstring string = _t("the quick brown fox jumps over the lazy dog");
    tstring_view view(string.data() + offset, length);

    EXPECT_EQ(string.data() + offset, view.data());
    EXPECT_EQ(length, view.size());
    EXPECT_FALSE(view.empty());

    for (size_t i = 0; i < view.size(); i++) {
        EXPECT_EQ(string[offset + i], view[i]);
    }
}

TEST(TStringViewTest, RangeForLoop)
{
    constexpr size_t offset = 5;
    constexpr size_t length = 10;

    tstring string = _t("the quick brown fox jumps over the lazy dog");
    tstring_view view(string.data() + offset, length);

    for (auto c : view) {
        // TODO: How can we automatically (rather than manually) verify the
        // iterator is working properly here?
        std::cout << c << std::endl;
    }
}
