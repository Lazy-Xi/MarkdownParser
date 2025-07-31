cmake_minimum_required(VERSION 3.16)

set(RESOURCE_PATH ${CMAKE_SOURCE_DIR})
configure_file(config.h.in config.h)

qt_add_executable(${PROJECT_NAME}_samples "main.cpp")
target_include_directories(${PROJECT_NAME}_samples 
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(${PROJECT_NAME}_samples PUBLIC ${PROJECT_NAME})

add_custom_target(run_samples ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory 
        ${CMAKE_SOURCE_DIR}/samples_output
    COMMAND ${CMAKE_COMMAND} -E env
        "PATH=${CMAKE_PREFIX_PATH}/bin;$ENV{PATH}"
        ${PROJECT_NAME}_samples
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running conversion samples..."
)

add_dependencies(run_samples ${PROJECT_NAME}_samples)