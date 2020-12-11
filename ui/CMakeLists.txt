cmake_minimum_required(VERSION 3.16)
project(ui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

add_library(ui)
target_sources(ui PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/mainwindow.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/mainwindow.ui
        PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include/ui/mainwindow.h)

target_include_directories(ui PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(ui Qt5::Core Qt5::Widgets)