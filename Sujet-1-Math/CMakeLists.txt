cmake_minimum_required(VERSION 3.16)

project(R5A12 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Linux: install  package.
find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include_directories(".")

add_executable(R5A12 main.cpp GameWindow.h GameWindow.cpp)
target_link_libraries(R5A12 Qt6::Widgets)
