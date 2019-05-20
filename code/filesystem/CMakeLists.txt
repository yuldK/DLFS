set(target filesystem)

make_library(${target} "STATIC")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_sources(${target}
    "Source Files"
    "filesystem.cpp"
)

add_sources(${target}
    "Header Files"
    "filesystem.h"
    "file.h"
    "fs_type.h"
)

add_sources(${target}
    "common"
    "${COMMON_DIR}/common.h"
    "${COMMON_DIR}/bit_op.h"
)
