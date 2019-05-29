set(target filesystem)

make_library(${target} "STATIC")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# use mio
include_directories(${MIO_INCLUDE_PATH})
message (STATUS "mio include path is ${MIO_INCLUDE_PATH}")

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
    "fsfwd.h"
	"path_util.h"
)

add_sources(${target}
    "common"
    "${COMMON_DIR}/common.h"
    "${COMMON_DIR}/bit_op.h"
)
