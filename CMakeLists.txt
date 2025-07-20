cmake_minimum_required(VERSION 3.16)
project(MarkdownParser LANGUAGES CXX)

include(cmake/qt.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH "T:/Qt/6.9.0/msvc2022_64")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Core
)
qt_standard_project_setup()

option(BUILD_SHARED_LIBS "Build shared library" ON)
option(BUILD_TESTING "Build tests" ON)

set(DEFAULT_CSS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/default.css)
set(DEFAULT_CSS_HEADER ${CMAKE_CURRENT_BINARY_DIR}/default_style_sheet.h)

add_custom_target(generate_style_sheet ALL
    COMMAND ${CMAKE_COMMAND} -DINPUT="${DEFAULT_CSS_PATH}" -DOUTPUT"=${DEFAULT_CSS_HEADER}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_style_sheet.cmake"
    COMMENT "Generating default_style_sheet.h"
)

set(HEADERS
    "include/export.h"
    "include/InlineParser.h"
    "include/MarkdownParser.h"
    "include/SectionFactory.h"
    "include/SectionList.h"
    "include/SectionParser.h"
    "include/SectionType.h"
    "include/section/AbstractSection.h"
    "include/section/CodeBlockSection.h"
    "include/section/HorizontalRuleSection.h"
    "include/section/HtmlSection.h"
    "include/section/OrderedListSection.h"
    "include/section/QuoteSection.h"
    "include/section/TableSection.h"
    "include/section/TextSection.h"
    "include/section/TitleSection.h"
    "include/section/UnorderedListSection.h"
)

set(SOURCES
    "src/InlineParser.cpp"
    "src/MarkdownParser.cpp"
    "src/SectionFactory.cpp"
    "src/SectionList.cpp"
    "src/SectionParser.cpp"
    "src/section/AbstractSection.cpp"
    "src/section/CodeBlockSection.cpp"
    "src/section/HorizontalRuleSection.cpp"
    "src/section/HtmlSection.cpp"
    "src/section/OrderedListSection.cpp"
    "src/section/QuoteSection.cpp"
    "src/section/TableSection.cpp"
    "src/section/TextSection.cpp"
    "src/section/TitleSection.cpp"
    "src/section/UnorderedListSection.cpp"
)

if(BUILD_SHARED_LIBS)
    qt_add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
    target_compile_definitions(${PROJECT_NAME} PRIVATE MARKDOWNTOHTML_EXPORTS)
else()
    qt_add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})
endif()

add_dependencies(${PROJECT_NAME} generate_style_sheet)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Core)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if(BUILD_TESTING)
    set(RESOURCE_PATH ${CMAKE_SOURCE_DIR})
    configure_file(tests/config.h.in config.h)

    qt_add_executable(${PROJECT_NAME}_test "tests/main.cpp")
    target_include_directories(${PROJECT_NAME}_test PRIVATE ${CMAKE_BINARY_DIR})
    target_link_libraries(${PROJECT_NAME}_test PUBLIC ${PROJECT_NAME})
    
    add_custom_target(run_tests ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory 
            ${CMAKE_SOURCE_DIR}/tests_output
        COMMAND ${CMAKE_COMMAND} -E env
            "PATH=${CMAKE_PREFIX_PATH}/bin;$ENV{PATH}"
            ${PROJECT_NAME}_test
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running conversion tests..."
    )
    add_dependencies(run_tests ${PROJECT_NAME}_test)
endif()
