# - Try to find the Enchant spell checker
# Once done this will define
#
#  ENCHANT_FOUND - system has ENCHANT
#  ENCHANT_INCLUDE_DIR - the ENCHANT include directory
#  ENCHANT_LIBRARIES - Link these to use ENCHANT
#  ENCHANT_DEFINITIONS - Compiler switches required for using ENCHANT

# Copyright (c) 2006, Zack Rusin, <zack@kde.org>
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


if (ENCHANT_INCLUDE_DIR AND ENCHANT_LIBRARIES)

  # in cache already
  set(ENCHANT_FOUND TRUE)

else (ENCHANT_INCLUDE_DIR AND ENCHANT_LIBRARIES)
  if (NOT WIN32)
    # use pkg-config to get the directories and then use these values
    # in the FIND_PATH() and FIND_LIBRARY() calls
    find_package(PkgConfig)
    pkg_check_modules(PC_ENCHANT QUIET enchant)
    set(ENCHANT_DEFINITIONS ${PC_ENCHANT_CFLAGS_OTHER})
  endif (NOT WIN32)

  find_path(ENCHANT_INCLUDE_DIR 
            NAMES enchant++.h
            HINTS ${PC_ENCHANT_INCLUDEDIR}
                  ${PC_ENCHANT_INCLUDE_DIRS}
            PATH_SUFFIXES enchant )

  find_library(ENCHANT_LIBRARIES NAMES enchant
               HINTS ${PC_ENCHANT_LIBDIR}
                      ${PC_ENCHANT_LIBRARY_DIRS} )

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(ENCHANT  DEFAULT_MSG  ENCHANT_INCLUDE_DIR ENCHANT_LIBRARIES )

  mark_as_advanced(ENCHANT_INCLUDE_DIR ENCHANT_LIBRARIES)

endif (ENCHANT_INCLUDE_DIR AND ENCHANT_LIBRARIES)
