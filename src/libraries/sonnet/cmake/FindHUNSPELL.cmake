# - Try to find HUNSPELL
# Once done this will define
#
#  HUNSPELL_FOUND - system has HUNSPELL
#  HUNSPELL_INCLUDE_DIRS - the HUNSPELL include directory
#  HUNSPELL_LIBRARIES - The libraries needed to use HUNSPELL

# Copyright (c) 2017, Pino Toscano <pino@kde.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the University nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

find_package(PkgConfig)
pkg_check_modules(PKG_HUNSPELL QUIET hunspell)

find_path(HUNSPELL_INCLUDE_DIRS
          NAMES hunspell.hxx
          PATH_SUFFIXES hunspell
          HINTS ${PKG_HUNSPELL_INCLUDE_DIRS}
)
find_library(HUNSPELL_LIBRARIES
             NAMES ${PKG_HUNSPELL_LIBRARIES} hunspell hunspell-1.6 hunspell-1.5 hunspell-1.4 hunspell-1.3 hunspell-1.2 libhunspell
             HINTS ${PKG_HUNSPELL_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HUNSPELL
                                  REQUIRED_VARS HUNSPELL_LIBRARIES HUNSPELL_INCLUDE_DIRS
                                  VERSION_VAR PKG_HUNSPELL_VERSION
)

mark_as_advanced(HUNSPELL_INCLUDE_DIRS HUNSPELL_LIBRARIES)
