#ifndef __DL_FS_TYPE__
#define __DL_FS_TYPE__
#pragma once

#include <string>
#include <cstdint>
#include <type_traits>
#include <common/bit_op.h>

namespace dl {
    namespace filesystem {

        using byte = char;
        using string = std::string;
        constexpr uint32_t DL_MAGIC_SIGNITURE{ 'PROY' };

        enum class mode : char {
            unknown = 0b00000,
            read    = 0b00001,
            write   = 0b00010,
            append  = 0b00100,
            create  = 0b01000,
        };

        enum class mount_type 
            : uint8_t {
            file,
            pack,
        };

        enum class seek_pos 
            : uint8_t {
            beg,
            cur,
            end,
        };
    }
}

// use bit operation
use_bit_op(dl::filesystem::mode);

#endif
