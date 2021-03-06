# GrandOrgue - free pipe organ simulator
# 
# Copyright 2006 Milan Digital Audio LLC
# Copyright 2009-2018 GrandOrgue contributors (see AUTHORS)
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

cmake_minimum_required(VERSION 2.8)

project(GrandOrgue CXX C)

# Build configuration options
option(RTAUDIO_USE_CORE       "Enable RtAudio support for Core Audio (Rt and PortAudio - OS X only)" ON)
option(RTAUDIO_USE_JACK       "Enable RtAudio support for Jack (Rt and PortAudio)" ON)
option(RTAUDIO_USE_OSS        "Enable RtAudio support for OSS (Rt and PortAudio - Linux only)" OFF)
option(RTAUDIO_USE_ALSA       "Enable RtAudio support for ALSA (Rt and PortAudio - Linux only)" ON)
option(RTAUDIO_USE_DSOUND     "Enable RtAudio support for DirectSound (Rt and PortAudio - Windows only)" ON)
option(RTAUDIO_USE_ASIO       "Enable RtAudio support for ASIO (Rt and PortAudio - Windows only)" ON)
option(RTAUDIO_USE_WMME       "Enable RtAudio support for WMME (PortAudio only - Windows only)" ON)
option(RTAUDIO_USE_WDMKS      "Enable RtAudio support for WDMKS (PortAudio only - Windows only)" ON)
option(RTAUDIO_USE_WASAPI     "Enable RtAudio support for WASAPI (PortAudio only - Windows only)" ON)
option(RTMIDI_USE_CORE        "Enable RtMidi support for Core Audio (OS X only)" ON)
option(RTMIDI_USE_JACK        "Enable RtMidi support for Jack" ON)
option(RTMIDI_USE_ALSA        "Enable RtMidi support for ALSA (Linux only)" ON)
option(RTMIDI_USE_MM          "Enable RtMidi support for MM (Windows only)" ON)
option(INSTALL_DEMO           "Install demo sampleset" ON)
option(USE_INTERNAL_PORTAUDIO "Use builtin PortAudio sources" ON)
option(USE_INTERNAL_WAVPACK   "Use builtin WavPack sources (deprecated)" OFF)
if (WIN32 OR APPLE)
   option(INSTALL_DEPEND      "Copy dependencies (wxWidgets libraries and Translations) on installation" ON)
else ()
   option(INSTALL_DEPEND      "Copy dependencies (wxWidgets libraries and Translations) on installation" OFF)
endif ()

# Project version numbers
set(VERSION_MAJOR 0)
set(VERSION_MINOR 3)
set(VERSION_STAGE 1)
if (NOT VERSION_REVISION)
  set(VERSION_REVISION 0)
endif()
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_STAGE}.${VERSION_REVISION})
set(NUM_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_STAGE}.${VERSION_REVISION})
string(REPLACE "." "," NUM_WIN_VERSION ${NUM_VERSION})

# only use options supported by the compiler
include(CheckIncludeFileCXX)
include(CheckFunctionExists)
include(FindPkgConfig)
include(${CMAKE_SOURCE_DIR}/cmake/AddOption.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/AddCXXOption.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/FindTools.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/BuildLibrary.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/BuildExecutable.cmake)

add_cxx_option(-std=c++11)
CHECK_INCLUDE_FILE_CXX(atomic HAVE_ATOMIC)
CHECK_INCLUDE_FILE_CXX(mutex HAVE_MUTEX)

IF (APPLE)
  set(BININSTDIR "${PROJECT_NAME}.app/Contents/MacOS")
  SET(LIBINSTDIR "${PROJECT_NAME}.app/Contents/MacOS")
  SET(RESOURCEINSTDIR "${PROJECT_NAME}.app/Contents/Resources")
  SET(RPATH_PREFIX "@executable_path")
ELSE()
  set(BININSTDIR "bin")
  IF (WIN32)
    SET(LIBINSTDIR "bin")
  ELSE ()
    SET(LIBINSTDIR "lib" CACHE STRING "library directory")
  ENDIF ()
  SET(RESOURCEINSTDIR "share/${PROJECT_NAME}")
  SET(RPATH_PREFIX "$ORIGIN")
ENDIF()
SET(BINDIR "${CMAKE_BINARY_DIR}/${BININSTDIR}")
SET(LIBDIR "${CMAKE_BINARY_DIR}/${LIBINSTDIR}")
SET(RESOURCEDIR "${CMAKE_BINARY_DIR}/${RESOURCEINSTDIR}")

if (APPLE AND INSTALL_DEPEND STREQUAL "ON")
  set(OBJECT_FIXUP_REQUIRED "ON")
else()
  set(OBJECT_FIXUP_REQUIRED "OFF")
endif()

file(RELATIVE_PATH RPATH "${BINDIR}" "${LIBDIR}")
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_INSTALL_RPATH "${RPATH_PREFIX}/${RPATH}")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

# setup compiler flags for debug vs release compiles
add_option(-Wall)
if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
   add_option(-O3)
   add_option(-g)
   add_definitions(-DNDEBUG)
   add_option(-fomit-frame-pointer)
   add_option(-funroll-loops)
   add_option(-ffast-math)
endif ()

if (APPLE)
   # see the following URL for information about these variables
   # https://developer.apple.com/library/mac/#documentation/General/Reference/InfoPlistKeyReference/Articles/CoreFoundationKeys.html
   set(BUNDLE_CFBundleShortVersionString ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_STAGE})
   set(BUNDLE_CFBundleVersion ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_STAGE}.${VERSION_REVISION})

   set(CPACK_PACKAGE_ICON "${RESOURCEDIR}/GrandOrgue.icns")

   configure_file(${CMAKE_SOURCE_DIR}/src/grandorgue/resource/Info.plist.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Info.plist)
   INSTALL(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Info.plist DESTINATION "${PROJECT_NAME}.app/Contents/")

   set(CPACK_BINARY_DRAGNDROP ON CACHE BOOL INTERNAL)
endif ()
if (WIN32)
   set(CPACK_BINARY_TGZ ON CACHE BOOL INTERNAL)
endif ()

# include RtAudio
add_subdirectory(src/rt)
set(RT_LIBRARIES RtAudio RtMidi)
set(RT_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/rt/include)

# include portaudio
if (USE_INTERNAL_PORTAUDIO)
  add_subdirectory(src/portaudio)
  set(PORTAUDIO_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/portaudio/include)
  set(PORTAUDIO_LIBRARIES PortAudio)
else()
  pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)
endif()

# include FFTW
pkg_check_modules(FFTW REQUIRED fftw3f)

pkg_check_modules(ZLIB REQUIRED zlib)

# include WAVPACK
if (USE_INTERNAL_WAVPACK)
  add_subdirectory(src/wavpack)
  set(WAVPACK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/wavpack/include)
  set(WAVPACK_LIBRARIES WavPack)
else()
  pkg_check_modules(WAVPACK REQUIRED wavpack)
endif()

find_package(wxWidgets REQUIRED html net adv core base)
message(STATUS "  wxWidgets Unicode?          : ${wxWidgets_USE_UNICODE}")
message(STATUS "  wxWidgets Debug?            : ${wxWidgets_USE_DEBUG}")
message(STATUS "  wxWidgets Static linking    : ${wxWidgets_USE_STATIC}")
message(STATUS "  wxWidgets version           : ${wxWidgets_VERSION}")
message(STATUS "  wxWidgets config binary     : ${wxWidgets_CONFIG_EXECUTABLE}")
message(STATUS "  wxWidgets configuration     : ${wxWidgets_CONFIGURATION}")
message(STATUS "============================================================================")

add_subdirectory(src/build)
add_subdirectory(src/images)
add_subdirectory(src/core)
add_subdirectory(src/grandorgue)
add_subdirectory(po)
add_subdirectory(help)
add_subdirectory(sounds)
add_subdirectory(packages)
add_subdirectory(resources)

set(CPACK_PACKAGE_NAME "GrandOrgue")
set(CPACK_PACKAGE_VENDOR "Our Organ")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "GrandOrgue - OpenSource Virtual Pipe Organ Software")
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_STAGE}.${VERSION_REVISION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "GrandOrgue")
set(CPACK_RPM_PACKAGE_LICENSE "GPL v2+")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/license.txt)
set(CPACK_PACKAGE_CONTACT "ourorgan-developers@lists.sourceforge.net")
set(CPACK_STRIP_FILES false)
set(CPACK_PACKAGE_EXECUTABLES "GrandOrgue" "GrandOrgue")
SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
  WriteRegStr HKCR \\\".organ\\\" \\\"\\\" \\\"GrandOrgue.odf\\\"
  WriteRegStr HKCR \\\".orgue\\\" \\\"\\\" \\\"GrandOrgue.package\\\"
  WriteRegStr HKCR \\\"GrandOrgue.odf\\\" \\\"\\\" \\\"GrandOrgue organ definition file\\\"
  WriteRegStr HKCR \\\"GrandOrgue.odf\\\\DefaultIcon\\\" \\\"\\\" \\\"$INSTDIR\\\\bin\\\\GrandOrgue.exe,0\\\"
  WriteRegStr HKCR \\\"GrandOrgue.odf\\\\shell\\\" \\\"\\\" \\\"open\\\"
  WriteRegStr HKCR \\\"GrandOrgue.odf\\\\shell\\\\open\\\\command\\\" \\\"\\\" '$INSTDIR\\\\bin\\\\GrandOrgue.exe \\\"%1\\\"'
  WriteRegStr HKCR \\\"GrandOrgue.package\\\" \\\"\\\" \\\"GrandOrgue organ package\\\"
  WriteRegStr HKCR \\\"GrandOrgue.package\\\\DefaultIcon\\\" \\\"\\\" \\\"$INSTDIR\\\\bin\\\\GrandOrgue.exe,0\\\"
  WriteRegStr HKCR \\\"GrandOrgue.package\\\\shell\\\" \\\"\\\" \\\"open\\\"
  WriteRegStr HKCR \\\"GrandOrgue.package\\\\shell\\\\open\\\\command\\\" \\\"\\\" '$INSTDIR\\\\bin\\\\GrandOrgue.exe \\\"%1\\\"'
")
SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
  ReadRegStr $R0 HKCR \\\".organ\\\" \\\"\\\"
  StrCmp $R0 \\\"GrandOrgue.odf\\\" 0 +2
    DeleteRegKey HKCR \\\".organ\\\"
  ReadRegStr $R0 HKCR \\\".orgue\\\" \\\"\\\"
  StrCmp $R0 \\\"GrandOrgue.package\\\" 0 +2
    DeleteRegKey HKCR \\\".orgue\\\"

  DeleteRegKey HKCR \\\"GrandOrgue.odf\\\"
  DeleteRegKey HKCR \\\"GrandOrgue.package\\\"
")
include(CPack)

message(STATUS "  Project                     : ${PROJECT_NAME}")
message(STATUS "  Description                 : ${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
message(STATUS "  Version                     : ${VERSION}")
message(STATUS "============================================================================")
message(STATUS " ")
