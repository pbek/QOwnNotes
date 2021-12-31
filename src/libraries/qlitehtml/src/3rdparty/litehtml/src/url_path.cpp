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

#include "litehtml/url_path.h"

namespace litehtml {

bool is_url_path_absolute(const tstring& path)
{
    return path.length() > 0 && path[0] == _t('/');
}

tstring url_path_directory_name(const tstring& path)
{
    size_t offset = path.find_last_of(_t('/'));
    if (offset == tstring::npos) {
        return _t(".");
    } else {
        return path.substr(0, offset + 1);
    }
}

tstring url_path_base_name(const tstring& path)
{
    size_t offset = path.find_last_of(_t('/'));
    if (offset == tstring::npos) {
        return path;
    } else {
        return path.substr(offset + 1);
    }
}

tstring url_path_append(const tstring& base, const tstring& path)
{
    tstring result(base);

    // Only append a separator if both base and path are not empty and if the
    // last character of base is not already a separator.
    if (!result.empty() && !path.empty() && result.back() != _t('/')) {
        result.append(1, _t('/'));
    }

    result.append(path);

    return result;
}

tstring url_path_resolve(const tstring& base, const tstring& path)
{

    // If the possibly relative path is an absolute path then it is not
    // relative and the base path is irrelevant.
    if (is_url_path_absolute(path)) {
        return path;
    }

    return url_path_append(url_path_directory_name(base), path);
}

} // namespace litehtml
