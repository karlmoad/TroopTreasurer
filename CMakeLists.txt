cmake_minimum_required(VERSION 3.16)
project(TroopTreasurer CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_BINARY_DIR})
set(CMAKE_CXX_STANDARD 20)

if(CMAKE_SYSTEM_NAME STREQUAL Android)
    set(ANDROID 1)
endif()

if(IOS OR ANDROID)
  message(FATAL_ERROR "Unsupported Operating System")
endif()

if (DEFINED QT_CMAKE_DIR)
    message(STATUS "QT CMAKE DIR.......${QT_CMAKE_DIR}")
else()
    message(FATAL_ERROR "QT_CMAKE_DIR unset! \n")
endif()

set(CMAKE_PREFIX_PATH ${QT_CMAKE_DIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

if (APPLE)
    set(MACOSX_BUNDLE_ICON_FILE scouts.icns)
    set(ICON_RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/scouts.icns)
    set_source_files_properties(${ICON_RESOURCE} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set(BUNDLE_QUALIFIER MACOSX_BUNDLE)
    message(STATUS "Build Type Apple OSX")
endif()

add_subdirectory(ui)
add_executable(TroopTreasurer ${BUNDLE_QUALIFIER} main.cpp ${ICON_RESOURCE})
target_link_libraries(TroopTreasurer Qt5::Core Qt5::Widgets ui)
