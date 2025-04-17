cmake_minimum_required(VERSION 3.16)
project(qtClient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets WebSockets Test)
qt_standard_project_setup()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(qtClient
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/gameboard.cpp
        src/gameboard.h
)

target_link_libraries(qtClient PRIVATE Qt6::Widgets Qt6::WebSockets)

add_executable(testGameBoard
        test/test_gameboard.cpp
        src/gameboard.cpp
        src/gameboard.h
)

target_link_libraries(testGameBoard PRIVATE Qt6::Widgets Qt6::Test)

add_executable(testMainWindow
        test/test_mainwindow.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/gameboard.cpp
        src/gameboard.h
)

target_link_libraries(testMainWindow PRIVATE Qt6::Widgets Qt6::WebSockets Qt6::Test)