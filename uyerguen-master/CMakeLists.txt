cmake_minimum_required(VERSION 3.17)
project(EIS_Draw)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread" )


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(EIS_Draw main.cpp MainWindow.cpp MainWindow.h shape.h Polygon.cpp Polygon.h Rechteck.cpp Rechteck.h Ellipse.cpp Ellipse.h m_label.cpp m_label.h shape_text.cpp shape_text.h)
target_link_libraries (EIS_Draw Qt5::Widgets Qt5::Core)