cmake_minimum_required(VERSION 3.5)

project(DGFEM-CAA-UI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_DIR "/opt/Qt/5.15.2/gcc_64/lib/cmake/Qt5")
set(CMAKE_PREFIX_PATH "/opt/Qt/5.15.2/gcc_64")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets core gui opengl xml  sql svg charts webenginewidgets) #Qt${QT_VERSION_MAJOR}::PrintSupport)
#find_package(Qt5 REQUIRED COMPONENTS PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets PrintSupport)

find_package(nlohmann_json 3.2.0 REQUIRED)

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/mainwindow.ui
        3rdParty/qcustomplot/qcustomplot.cpp
        3rdParty/qcustomplot/qcustomplot.h
#        3rdParty/qcustomplot/examples/text-document-integration/qcpdocumentobject.cpp
#        3rdParty/qcustomplot/examples/text-document-integration/qcpdocumentobject.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(DGFEM-CAA-UI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET DGFEM-CAA-UI APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(DGFEM-CAA-UI SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(DGFEM-CAA-UI
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(DGFEM-CAA-UI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::PrintSupport -lqtermwidget5)

set_target_properties(DGFEM-CAA-UI PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER https://skhelladi.github.io/DGFEM-CAA/
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS DGFEM-CAA-UI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(DGFEM-CAA-UI)
endif()
