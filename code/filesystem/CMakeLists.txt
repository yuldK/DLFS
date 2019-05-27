set(target filesystem)

make_library(${target} "STATIC")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_sources(${target}
    "pack"
    "pack.cpp"
    "pack.h"
)

add_sources(${target}
    "filesystem"
    "filesystem.cpp"
    "filesystem.h"
)

add_sources(${target}
    "Source Files"
)

add_sources(${target}
    "Header Files"
    "file.h"
    "fs_type.h"
)

add_sources(${target}
    "common"
    "${COMMON_DIR}/common.h"
    "${COMMON_DIR}/bit_op.h"
)
