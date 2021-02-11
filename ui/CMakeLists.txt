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
            applicationsettings.cpp
            importtemplatetestdialog.cpp importtemplatetestdialog.ui
            newimporttemplatedialog.cpp newimporttemplatedialog.ui
            panelactions.cpp
            importdatadialog.cpp importdatadialog.ui
            informationtabledialog.cpp informationtabledialog.ui
            paymentspanel.cpp paymentspanel.ui
            depositspanel.cpp depositspanel.ui
            fundsregisterpanel.cpp fundsregisterpanel.ui
            paymenteditdialog.cpp paymenteditdialog.ui
            fundsrecordeditdialog.cpp fundsrecordeditdialog.ui
            depositeditdialog.cpp depositeditdialog.ui
            depositeditpicklistdialog.cpp depositeditpicklistdialog.ui
            accountsmanagementpanel.cpp accountsmanagementpanel.ui
            accounteditdialog.cpp accounteditdialog.ui
            accountpickerdialog.cpp accountpickerdialog.ui
            unassociatedsourceaccountdialog.cpp unassociatedsourceaccountdialog.ui
            negativebalancereportpanel.cpp negativebalancereportpanel.ui)

set(HEADERS ui.h
        mainwindow.h
        panelwindow.h
        itemstate.h
        importtemplatepanel.h
        panelfactory.h
        applicationsettingsdialog.h
        databasesettingspane.h
        mainsettingspane.h
        applicationsettings.h
        applicationsettingspane.h
        applicationconstants.h
        importtemplatetestdialog.h
        newimporttemplatedialog.h
        panelactions.h
        importdatadialog.h
        informationtabledialog.h
        paymentspanel.h
        depositspanel.h
        fundsregisterpanel.h
        paymenteditdialog.h
        fundsrecordeditdialog.h
        depositeditdialog.h
        depositeditpicklistdialog.h
        accountsmanagementpanel.h
        accounteditdialog.h
        accountpickerdialog.h
        unassociatedsourceaccountdialog.h
        negativebalancereportpanel.h)

format_source_paths("/src" SOURCES)
format_source_paths("/include/ui" HEADERS)

find_package(Qt5 COMPONENTS Core Widgets Network Sql REQUIRED)

add_library(ui)
target_sources(ui PRIVATE ${SOURCES} PUBLIC ${HEADERS} ${CMAKE_CURRENT_LIST_DIR}/resources.qrc)

target_include_directories(ui PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(ui Qt5::Core Qt5::Widgets Qt5::Network Qt5::Sql objects)