cmake_minimum_required(VERSION 3.1)

project(visualization LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Quick Location Positioning Widgets REQUIRED)

set(SOURCE_FILES
        main.cpp
        mapcontroller.cpp
        mapcontroller.h
        location.h

        ../main/cpp/geopop/io/EpiJSONReader.cpp
        )

set(QML
        main.qml
        mapcircle.qml)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} "qml.qrc")
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Quick Qt5::Positioning Qt5::Widgets libstride)
#install(FILES ${QML} DESTINATION ${BIN_INSTALL_LOCATION})
#install(TARGETS ${PROJECT_NAME} DESTINATION ${BIN_INSTALL_LOCATION})
