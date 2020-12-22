cmake_minimum_required(VERSION 3.16)
project(keymanager)

include(${CMAKE_SOURCE_DIR}/cmake_modules/SourceFunctions.cmake)

set(SOURCES keymanager_impl.h keymanager_error.cpp)
set(HEADERS keymanager.h)

if (APPLE)
    list(APPEND SOURCES keymanager_osx.cpp)
    find_library(SECURITY_LIB Security)
endif()

if (WIN32)
    list(APPEND SOURCES keymanager_win.cpp)
    set(SECURITY_LIB crypt32)
endif()

format_source_paths("/src" SOURCES)
format_source_paths("/include/keymanager" HEADERS)

find_package(Qt5 COMPONENTS Core REQUIRED)

add_library(keymanager)
target_sources(keymanager PRIVATE ${SOURCES} PUBLIC ${HEADERS})
target_include_directories(keymanager PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(keymanager Qt5::Core ${SECURITY_LIB})