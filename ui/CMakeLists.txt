cmake_minimum_required(VERSION 3.16)
project(ui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include(${CMAKE_SOURCE_DIR}/cmake_modules/SourceFunctions.cmake)

set(SOURCES mainwindow.cpp mainwindow.ui
            importtemplatepanel.cpp importtemplatepanel.ui
            panelfactory.cpp panelwindow.cpp
            applicationsettingsdialog.cpp applicationsettingsdialog.ui
            databasesettingspane.cpp databasesettingspane.ui
            mainsettingspane.cpp mainsettingspane.ui
            applicationsettings.cpp)

set(HEADERS mainwindow.h
        panelwindow.h
        itemstate.h
        importtemplatepanel.h
        panelfactory.h
        applicationsettingsdialog.h
        databasesettingspane.h
        mainsettingspane.h
        applicationsettings.h
        applicationsettingspane.h
        applicationconstants.h)

format_source_paths("/src" SOURCES)
format_source_paths("/include/ui" HEADERS)

find_package(Qt5 COMPONENTS Core Widgets Network Sql REQUIRED)

add_library(ui)
target_sources(ui PRIVATE ${SOURCES} PUBLIC ${HEADERS} ${CMAKE_CURRENT_LIST_DIR}/resources.qrc)

target_include_directories(ui PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(ui Qt5::Core Qt5::Widgets Qt5::Network Qt5::Sql objects)