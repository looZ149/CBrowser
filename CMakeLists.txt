cmake_minimum_required(VERSION 3.16)
project(CBrowser)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        WebEngineWidgets
        WebEngineCore
        REQUIRED)

add_executable(CBrowser
        main.cpp
        mainWindow.h
        mainWindow.cpp
        bookMarks.cpp
        bookMarks.h)

target_link_libraries(CBrowser PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::WebEngineWidgets
        Qt6::WebEngineCore
)

