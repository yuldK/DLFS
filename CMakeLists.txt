cmake_minimum_required(VERSION 3.11)

include(cmake/macros.cmake)

include(cmake/platform/windows.cmake)

set(ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

set(DEFAULT_OUTPUT_DIRECTORY "${ROOT}/bin/")
set(DEFAULT_OUTPUT_DIRECTORY_CONFIG "${DEFAULT_OUTPUT_DIRECTORY}$(Configuration)")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${DEFAULT_OUTPUT_DIRECTORY}$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Profile>:Profile>$<$<CONFIG:Release>:Release>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${DEFAULT_OUTPUT_DIRECTORY}$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Profile>:Profile>$<$<CONFIG:Release>:Release>")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${DEFAULT_OUTPUT_DIRECTORY}$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Profile>:Profile>$<$<CONFIG:Release>:Release>")

link_directories(${DEFAULT_OUTPUT_DIRECTORY})

set(SOLUTION_NAME "dlfs")

project(${SOLUTION_NAME})

add_subdirectory(sdks)
set(SDKS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/sdks/")
SET (MIO_INCLUDE_PATH ${SDKS_ROOT}/mio/single_include/mio)

add_subdirectory(code)
