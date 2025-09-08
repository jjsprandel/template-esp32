# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS
    "$ENV{PROJECT_DIR}/projects/project1/src/app/main"
)

set(COMPONENT_DIRS 
    "$ENV{IDF_PATH}/components"
    "$ENV{PROJECT_DIR}/projects/project1/src/app"
    "$ENV{EXTRA_COMPONENT_DIRS}"
)

add_custom_target(all_in_one
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -S ${CMAKE_SOURCE_DIR} -B ${CMAKE_BINARY_DIR} -G Ninja
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running full build (configure + compile)"
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(project1)
