set(target module)

make_library(${target} "SHARED")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_pch(${target} pch.cpp pch.h)

add_sources(${target}
    "Source Files"
    "dllmain.cpp"
)

add_sources(${target}
    "Header Files"
    "module.h"
)

target_link_libraries(${target}
    PUBLIC
        ${WINLIBS}
)
