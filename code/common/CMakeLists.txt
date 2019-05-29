set(target common)

make_library(${target} "STATIC")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_sources(${target}
    "Source Files"
    "common.cpp"
)

add_sources(${target}
    "Header Files"
    "common.h"
    "bit_op.h"
	"group.h"
)
