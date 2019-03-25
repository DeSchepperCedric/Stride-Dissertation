cmake_minimum_required(VERSION 3.1)

project(visual LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Quick Positioning Location REQUIRED)

set(SRC
        "main.cpp"
        "backend.cpp"
        "backend.h"
        "qml.qrc")

#"../main/cpp/viewers/QtViewer.cpp"
#"../main/cpp/viewers/QtViewer.h"

add_executable(${PROJECT_NAME} ${SRC})

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen)
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Quick Qt5::Positioning Qt5::Location)
install(TARGETS ${PROJECT_NAME} DESTINATION ${BIN_INSTALL_LOCATION})