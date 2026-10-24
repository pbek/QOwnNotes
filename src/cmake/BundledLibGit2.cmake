# Keep dependency options local rather than changing QOwnNotes' build options.
function(qon_add_bundled_libgit2)
  if(CMAKE_VERSION VERSION_LESS 3.21)
    message(FATAL_ERROR "Bundled libgit2 requires CMake 3.21 or newer")
  endif()
  include(FetchContent)
  set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
  # libgit2 uses both option() and set(... CACHE ...). Preserve our local
  # overrides for both without forcing dependency options into the cache.
  set(CMAKE_POLICY_DEFAULT_CMP0126 NEW)
  if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
  endif()
  set(BUILD_SHARED_LIBS OFF)
  set(BUILD_TESTS OFF)
  set(BUILD_CLI OFF)
  set(BUILD_EXAMPLES OFF)
  set(STATIC_CRT OFF)
  # QOwnNotes uses libgit2 for local repositories; remote commands use Git.
  # Bundle the remaining dependencies to avoid extra release DLLs/dylibs.
  set(USE_SSH OFF)
  set(USE_HTTPS OFF)
  set(USE_GSSAPI OFF)
  set(USE_NTLMCLIENT OFF)
  set(USE_BUNDLED_ZLIB ON)
  set(REGEX_BACKEND builtin)
  set(USE_SHA1 CollisionDetection)
  set(USE_SHA256 builtin)
  set(USE_ICONV OFF)
  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Libgit2 probes struct stat before adding _GNU_SOURCE to its build flags.
    # Match those flags so strict C mode exposes nanosecond timestamps.
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
  endif()
  FetchContent_Declare(
    libgit2
    URL https://codeload.github.com/libgit2/libgit2/tar.gz/refs/tags/v1.9.7
    URL_HASH
      SHA256=1a4fbe7589e814777ae76b64734ad80f4ecad22cd33a22682a2aaea4ae5375e7)
  FetchContent_MakeAvailable(libgit2)
  # Release packages need the linked code, not libgit2's development package.
  set_property(DIRECTORY "${libgit2_SOURCE_DIR}" PROPERTY EXCLUDE_FROM_ALL TRUE)
  target_include_directories(
    libgit2package INTERFACE "$<BUILD_INTERFACE:${libgit2_SOURCE_DIR}/include>")
endfunction()

qon_add_bundled_libgit2()
