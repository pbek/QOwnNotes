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

#ifndef LITEHTML_URL_H__
#define LITEHTML_URL_H__

#include <ostream>

#include "litehtml/os_types.h"

// https://datatracker.ietf.org/doc/html/rfc3986

namespace litehtml {

class url {
public:
    url() = default;

    explicit url(const tstring& str);

    url(const tstring& scheme,
        const tstring& authority,
        const tstring& path,
        const tstring& query,
        const tstring& fragment);

    const tstring& string() const
    {
        return str_;
    }

    const tstring& scheme() const
    {
        return scheme_;
    }

    bool has_scheme() const
    {
        return !scheme_.empty();
    }

    const tstring& authority() const
    {
        return authority_;
    }

    bool has_authority() const
    {
        return !authority_.empty();
    }

    const tstring& path() const
    {
        return path_;
    }

    bool has_path() const
    {
        return !path_.empty();
    }

    const tstring& query() const
    {
        return query_;
    }

    bool has_query() const
    {
        return !query_.empty();
    }

    const tstring& fragment() const
    {
        return fragment_;
    }

    bool has_fragment() const
    {
        return !fragment_.empty();
    }

protected:
    tstring str_;

    // Assume URLs are relative by default.  See RFC 3986 Section 4.3 for
    // information on which URLs are considered relative and which URLs are
    // considered absolute:
    //
    //   https://datatracker.ietf.org/doc/html/rfc3986#section-4.3

    bool absolute_ = false;

    tstring scheme_;

    tstring authority_;

    tstring path_;

    tstring query_;

    tstring fragment_;
};

// Returns a URL that is resolved from the reference URL that might be
// relative to the base URL.  For example, given <https://www.twitter.com/> as
// the base URL and </foo> as the relative URL, resolve() will return the URL
// <https://www.twitter.com/foo>.

url resolve(const url& base, const url& reference);

} // namespace litehtml

#endif // LITEHTML_URL_H__
