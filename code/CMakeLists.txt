# include root directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${SDKS_ROOT})

set(COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/common/")
set(DLFS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/filesystem/")

add_subdirectory(common)
add_subdirectory(filesystem)
#target_link_libraries(filesystem PUBLIC <mio::mio | mio>)

# main project
add_subdirectory(example)

# dynamic library
#add_subdirectory(module)