cmake_minimum_required(VERSION 3.5)

project(crypto_chat VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/images/appicon.rc")

set(PROJECT_SOURCES
        main.cpp
        resources.qrc

        chatwindow.cpp
        chatwindow.h
        chatwindow.ui
        ui_chatwindow.h

        colordialog.h
        colordialog.cpp
        colordialog.ui
        ui_colordialog.h

        namechangedialog.h
        namechangedialog.cpp
        namechangedialog.ui
        ui_namechangedialog.h

        logindialog.h
        logindialog.cpp
        logindialog.ui
        ui_logindialog.h

        intervaldialog.h
        intervaldialog.cpp
        intervaldialog.ui
        ui_intervaldialog.h

        threadfunctions.h
        threadfunctions.cpp

        roomdialog.h
        roomdialog.cpp
        roomdialog.ui
        ui_roomdialog.h

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(crypto_chat
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET crypto_chat APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(crypto_chat SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(crypto_chat
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(crypto_chat PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(crypto_chat PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS crypto_chat
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(crypto_chat)
endif()

find_package(Qt6 REQUIRED COMPONENTS Network)
target_link_libraries(crypto_chat PRIVATE Qt6::Network)

find_package(Qt6 REQUIRED COMPONENTS Core5Compat)
target_link_libraries(crypto_chat PRIVATE Qt6::Core5Compat)
