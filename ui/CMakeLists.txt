cmake_minimum_required(VERSION 3.16)
project(ui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

function(format_source_paths PATH_VALUE LIST_VAR)
    foreach(ENTRY ${${LIST_VAR}})
        list(APPEND TEMP_LIST ${CMAKE_CURRENT_LIST_DIR}${PATH_VALUE}/${ENTRY})
    endforeach()
    set(${LIST_VAR} ${TEMP_LIST} PARENT_SCOPE)
endfunction()

set(SOURCES mainwindow.cpp mainwindow.ui importtemplatepanel.cpp importtemplatepanel.ui )
set(HEADERS mainwindow.h panelwindow.h itemstate.h importtemplatepanel.h)

format_source_paths("/src" SOURCES)
format_source_paths("/include/ui" HEADERS)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

add_library(ui)
target_sources(ui PRIVATE ${SOURCES} PUBLIC ${HEADERS} ${CMAKE_CURRENT_LIST_DIR}/resources.qrc)

target_include_directories(ui PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(ui Qt5::Core Qt5::Widgets)