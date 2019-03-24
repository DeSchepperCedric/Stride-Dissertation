set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${Qt5Widgets_INCLUDE_DIRS} ${QTQml_INCLUDE_DIRS})


set(QTSRC
        main.cpp
        mainwindow.cpp

        mainwindow.ui
        )

#get_filename_component(QRC ${RESOURCES} REALPATH)
set(QRC ${QRC} PARENT_SCOPE)

add_library(libstrideqt ${QTSRC})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen)
#target_link_libraries(libstrideqt libstride researchlib Qt5::Widgets Qt5::Quick Qt5::QuickWidgets Qt5::Qml)
target_link_libraries(libstrideqt Qt5 Qt5::Widgets Qt5::Quick Qt5QuickWidgets Qt5::Qml)
set(LIBS ${LIBS} libstrideqt)

add_executable(qt ${QTSRC})
target_link_libraries(qt Qt5::Widgets)