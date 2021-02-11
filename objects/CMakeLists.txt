cmake_minimum_required(VERSION 3.16)
project(objects)

set(CMAKE_AUTOMOC ON)

include(${CMAKE_SOURCE_DIR}/cmake_modules/SourceFunctions.cmake)

set(SOURCES settingsmanager.cpp
            objecterror.cpp
            settingsmodel.cpp
            templatemappingmodel.cpp
            csvmodel.cpp
            argument.cpp
            expression.cpp
            utility.cpp
            expressionfactory.cpp
            importspecification.cpp
            importspecificationruntime.cpp
            datatablemodel.cpp
            schema.cpp
            jsontablemodel.cpp
            dataimportcontroller.cpp
            datatype.cpp
            payment.cpp
            paymentsmodel.cpp
            deposit.cpp
            fundsrecord.cpp
            depositsmodel.cpp
            fundsrecordsmodel.cpp
            depositfundsmodel.cpp
            dataobject.cpp
            account.cpp
            accountsmodel.cpp
            hierarchyitem.cpp)

set(HEADERS settingsmanager.h
            objecterror.h
            settingsmodel.h
            templatemappingmodel.h
            csvmodel.h
            argument.h
            expression.h
            utility.h
            batchexpression.h
            hashingexpression.h
            uuidexpression.h
            valueexpression.h
            expressionfactory.h
            importspecification.h
            importspecificationruntime.h
            datatablemodelimpl.h
            datatablemodel.h
            schema.h
            jsontablemodel.h
            dataimportcontroller.h
            datatype.h
            transactions.h
            payment.h
            dates.h
            paymentsmodel.h
            change.h
            changequeue.h
            deposit.h
            fundsrecord.h
            depositsmodel.h
            fundsrecordsmodel.h
            depositfundsmodel.h
            dataobject.h
            account.h
            accountsmodel.h
            hierarchyitem.h)

format_source_paths("/src" SOURCES)
format_source_paths("/include/objects" HEADERS)

find_package(Qt5 COMPONENTS Core Widgets Sql Network REQUIRED)

add_library(objects)
target_sources(objects PRIVATE ${SOURCES} PUBLIC ${HEADERS})
target_include_directories(objects PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(objects Qt5::Core Qt5::Widgets Qt5::Sql Qt5::Network keymanager)