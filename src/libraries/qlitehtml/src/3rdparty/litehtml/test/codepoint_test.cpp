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

#include "litehtml/codepoint.h"

#include <iostream>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

using namespace litehtml;

// It's impractical to test every single ASCII codepoint in the codepoint
// tests.  Instead, the tests focus on a subset of the codepoints where
// problems are more likely to occur (e.g., near the start and end of ranges
// of similar codepoints).

TEST(CodepointTest, URLReserved)
{
    std::vector<std::pair<tchar_t, bool>> testcases = {
        { '!', true },
        { '"', false },
        { '#', true },
        { '$', true },
        { '%', false },
        { '&', true },
        { '\'', true },
        { '(', true },
        { ')', true },
        { '*', true },
        { ',', true },
        { '+', true },
        { ',', true },
        { '-', false },
        { '.', false },
        { '/', true },
        { '0', false },
        { '1', false },
        { '8', false },
        { '9', false },
        { ':', true },
        { ';', true },
        { '<', false },
        { '=', true },
        { '>', false },
        { 'A', false },
        { 'B', false },
        { 'G', false },
        { 'H', false },
        { 'Y', false },
        { 'Z', false },
        { '[', true },
        { '\\', false },
        { ']', true },
        { 'a', false },
        { 'b', false },
        { 'g', false },
        { 'h', false },
        { 'y', false },
        { 'z', false },
        { '{', false },
        { '|', false },
        { '}', false },
    };

    for (auto testcase : testcases) {
        EXPECT_EQ(testcase.second, is_url_reserved_codepoint(testcase.first));
    }
}

TEST(CodepointTest, URLScheme)
{
    std::vector<std::pair<tchar_t, bool>> testcases = {
        { '!', false },
        { '"', false },
        { '#', false },
        { '$', false },
        { '%', false },
        { '&', false },
        { '\'', false },
        { '(', false },
        { ')', false },
        { '*', false },
        { ',', false },
        { '+', true },
        { ',', false },
        { '-', true },
        { '.', true },
        { '/', false },
        { '0', true },
        { '1', true },
        { '8', true },
        { '9', true },
        { ':', false },
        { ';', false },
        { '<', false },
        { '=', false },
        { '>', false },
        { 'A', true },
        { 'G', true },
        { 'H', true },
        { 'Y', true },
        { 'Z', true },
        { '[', false },
        { '\\', false },
        { ']', false },
        { 'a', true },
        { 'b', true },
        { 'g', true },
        { 'h', true },
        { 'y', true },
        { 'z', true },
        { '{', false },
        { '|', false },
        { '}', false },
    };

    for (auto testcase : testcases) {
        EXPECT_EQ(testcase.second, is_url_scheme_codepoint(testcase.first));
    }
}
