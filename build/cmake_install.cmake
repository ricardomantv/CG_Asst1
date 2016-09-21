# Install script for directory: /Users/ricardo/Documents/school/junior/fall/computer graphics/asst1_drawsvg/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Users/ricardo/Documents/school/junior/fall/computer graphics/asst1_drawsvg/src/../")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/ricardo/Documents/school/junior/fall/computer graphics/asst1_drawsvg/src/hardware/libdrawsvghdwr_osx.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/Users/ricardo/Documents/school/junior/fall/computer graphics/asst1_drawsvg/src/hardware" TYPE STATIC_LIBRARY FILES "/Users/ricardo/Documents/school/junior/fall/computer graphics/asst1_drawsvg/build/libdrawsvghdwr_osx.a")
  if(EXISTS "$ENV{DESTDIR}/Users/ricardo/Documents/school/junior/fall/computer graphics/asst1_drawsvg/src/hardware/libdrawsvghdwr_osx.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/ricardo/Documents/school/junior/fall/computer graphics/asst1_drawsvg/src/hardware/libdrawsvghdwr_osx.a")
    execute_process(COMMAND "/Library/Developer/CommandLineTools/usr/bin/ranlib" "$ENV{DESTDIR}/Users/ricardo/Documents/school/junior/fall/computer graphics/asst1_drawsvg/src/hardware/libdrawsvghdwr_osx.a")
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE EXECUTABLE FILES "/Users/ricardo/Documents/school/junior/fall/computer graphics/asst1_drawsvg/build/drawsvg")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/./drawsvg" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/./drawsvg")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/ricardo/Documents/school/junior/fall/computer graphics/asst1_drawsvg/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/./drawsvg")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Library/Developer/CommandLineTools/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/./drawsvg")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/Users/ricardo/Documents/school/junior/fall/computer graphics/asst1_drawsvg/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
