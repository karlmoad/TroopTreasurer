cmake_minimum_required(VERSION 3.16)
project(objects)

include(${CMAKE_SOURCE_DIR}/cmake_modules/SourceFunctions.cmake)

set(SOURCES settingsmanager.cpp)
set(HEADERS settingsmanager.h)

format_source_paths("/src" SOURCES)
format_source_paths("/include/objects" HEADERS)

find_package(Qt5 COMPONENTS Core REQUIRED)

add_library(objects)
target_sources(objects PRIVATE ${SOURCES} PUBLIC ${HEADERS})
target_include_directories(objects PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(objects Qt5::Core keymanager)