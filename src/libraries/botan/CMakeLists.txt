cmake_minimum_required(VERSION 3.5)
project(Botan)

option(BUILD_WITH_SYSTEM_BOTAN
       "Build using system installed Botan 2 crypto library" OFF)

add_library(botan STATIC "")
add_library(Botan::Botan ALIAS botan)

target_sources(botan PRIVATE botanwrapper.h botanwrapper.cpp)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

target_link_libraries(botan PUBLIC Qt${QT_VERSION_MAJOR}::Core)

target_include_directories(botan PUBLIC ${CMAKE_CURRENT_LIST_DIR})

if(BUILD_WITH_SYSTEM_BOTAN)
  find_package(Botan3 REQUIRED)
  if(NOT TARGET Botan3::Botan3)
    message(FATAL_ERROR "Could not find system Botan 3 library using PkgConfig")
  endif()

  target_link_libraries(botan PUBLIC Botan3::Botan3)

  target_compile_definitions(botan PUBLIC USE_SYSTEM_BOTAN)

  # Instead of put next lines into else() block it is better to just return
  return()
endif()

target_sources(botan PRIVATE botan_all.cpp botan_all.h)

# add some Botan defines for Linux
if(UNIX)
  # Append some definitions as PRIVATE only if they used only in cpp and
  # botan_internal header
  target_compile_definitions(botan PRIVATE BOTAN_HAS_ENTROPY_SRC_DEV_RANDOM)
endif()
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  target_compile_definitions(
    botan PRIVATE BOTAN_TARGET_OS_HAS_CLOCK_GETTIME
                  BOTAN_TARGET_OS_HAS_POSIX_MLOCK BOTAN_TARGET_OS_HAS_POSIX1)
endif()
if(APPLE)
  target_compile_definitions(botan PRIVATE BOTAN_TARGET_OS_HAS_POSIX1)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_definitions(botan PUBLIC BOTAN_BUILD_COMPILER_IS_GCC)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_definitions(botan PUBLIC BOTAN_BUILD_COMPILER_IS_CLANG)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  target_compile_definitions(botan PRIVATE BOTAN_BUILD_COMPILER_IS_INTEL)
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "(i386|i686|x86_64|AMD64)")
  if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    target_compile_definitions(botan PUBLIC BOTAN_TARGET_CPU_IS_X86_FAMILY
                                            BOTAN_TARGET_ARCH_IS_X86_32)
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_compile_definitions(
      botan PUBLIC BOTAN_TARGET_CPU_IS_X86_FAMILY BOTAN_TARGET_ARCH_IS_X86_64
                   BOTAN_TARGET_CPU_HAS_NATIVE_64BIT)
  endif()
endif()

if(WIN32)
  target_compile_definitions(
    botan PRIVATE BOTAN_TARGET_OS_IS_WINDOWS BOTAN_TARGET_OS_HAS_WIN32
                  BOTAN_HAS_ENTROPY_SRC_WIN32)
  if(MSVC)
    # TODO Find analog for: QMAKE_CXXFLAGS_EXCEPTIONS_ON = -EHs
    target_compile_options(botan PUBLIC "-EHs" "-wd4251" "-wd4290" "-wd4250")
    target_compile_definitions(botan PRIVATE _SCL_SECURE_NO_WARNINGS
                                             _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(botan PUBLIC BOTAN_BUILD_COMPILER_IS_MSVC)
    if(NOT (MSVC_VERSION LESS 1900))
      # For included library binary ABI does not matter so disable warning
      target_compile_definitions(botan PRIVATE _ENABLE_EXTENDED_ALIGNED_STORAGE)
    endif()
  else()
    target_compile_options(botan PRIVATE "-fpermissive" "-finline-functions"
                                         "-Wno-long-long")
  endif()
endif()

if(UNIX AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set_target_properties(botan PROPERTIES POSITION_INDEPENDENT_CODE ON)
  target_compile_options(botan PRIVATE "-fpermissive" "-finline-functions"
                                       "-Wno-long-long")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "(Linux|FreeBSD)")
  target_link_libraries(botan PRIVATE rt)
  target_compile_definitions(botan PRIVATE BOTAN_TARGET_OS_HAS_GETAUXVAL)
endif()
