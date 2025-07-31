cmake_minimum_required(VERSION 3.16)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Test
)

set(RESOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
configure_file(config.h.in config.h)

set(TESTS_HEADERS
    "include/test_Section.h"
)

set(TESTS_SOURCES
    main.cpp
    "src/test_Section.cpp"
)

qt_add_executable(${PROJECT_NAME}_tests ${TESTS_HEADERS} ${TESTS_SOURCES})

target_include_directories(${PROJECT_NAME}_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME}_tests
    PRIVATE
        ${PROJECT_NAME}
        Qt${QT_VERSION_MAJOR}::Test
)

add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)
