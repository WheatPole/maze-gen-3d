cmake_minimum_required(VERSION 3.16)

project(3DMaze VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS OpenGL)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(PROJECT_SOURCES
        main.cpp
        basewindow.cpp
        basewindow.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(3DMaze
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        engine/wall.h engine/wall.cpp
        engine/wallfacing.h engine/wallfacing.cpp
        engine/tile.h engine/tile.cpp
        engine/tilebox.h engine/tilebox.cpp
        util/quad.h
        engine/generator.h engine/generator.cpp
        parsing/mesh.h parsing/mesh.cpp
        util/disjointset.h
        parsing/vertexbox.h
        util/triplet.h
        parsing/objparser.h parsing/objparser.cpp
        rendering/view3d.h rendering/view3d.cpp
        rendering/camera.h rendering/camera.cpp
        rendering/model.h rendering/model.cpp
        ui/entrytablemodel.h ui/entrytablemodel.cpp
        ui/entrytable.h ui/entrytable.cpp
        rendering/outlinedmodel.h rendering/outlinedmodel.cpp
        shaders/vshader_outline.glsl



    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET 3DMaze APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(3DMaze SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(3DMaze
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(3DMaze PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    Qt6::Core
    Qt6::Gui
    Qt6::GuiPrivate
    Qt6::OpenGL
    Qt6::OpenGLWidgets
)
target_link_libraries(3DMaze PRIVATE Qt6::Core Qt6::Widgets)

set(shaders_resource_files
    "shaders/fshader.glsl"
    "shaders/vshader.glsl"
)

qt6_add_resources(3DMaze "shaders"
    PREFIX
        "/"
    FILES
        ${shaders_resource_files}
)


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.3DMaze)
endif()
set_target_properties(3DMaze PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS 3DMaze
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(3DMaze)
endif()
