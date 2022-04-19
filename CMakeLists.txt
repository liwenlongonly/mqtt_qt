cmake_minimum_required(VERSION 3.21)
project(mqtt_qt)

set(CMAKE_OSX_ARCHITECTURES x86_64)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set (CMAKE_PREFIX_PATH "/Users/ilong/Qt5.14.2/5.14.2/clang_64/lib/cmake")

include_directories(${PROJECT_SOURCE_DIR}/)
include_directories(${PROJECT_SOURCE_DIR}/mqtt/include/)

link_directories(${PROJECT_SOURCE_DIR}/mqtt/lib)

file(GLOB _Source ${PROJECT_SOURCE_DIR}/*.c ${PROJECT_SOURCE_DIR}/*.cpp)

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(mqtt_qt ${_Source})

target_link_libraries(mqtt_qt
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        paho-mqtt3as
        paho-mqtt3a
        paho-mqtt3cs
        paho-mqtt3c
        )

