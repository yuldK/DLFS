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
            unknown = 0b0000'0000,
            read    = 0b0000'0001,
            write   = 0b0000'0010,
            append  = 0b0000'0100,
            create  = 0b0000'1000,
			binary	= 0b0001'0000,
			at_end	= 0b0010'0000,
        };

        enum class mount_type 
            : uint8_t {
            file,
            pack,
        };

        enum class seek_dir 
            : uint8_t {
            beg,
            cur,
            end,
        };
    }
}

// use bit operation
use_bit_op(dl::filesystem::mode);

namespace dl {
	namespace filesystem {
		//	app		(append) Set the stream's position indicator to the end of the stream before each output operation.
		//	ate		(at end) Set the stream's position indicator to the end of the stream on opening.
		//	binary	(binary) Consider stream as binary rather than text.
		//	in		(input) Allow input operations on the stream.
		//	out		(output) Allow output operations on the stream.
		//	trunc	(truncate) Any current content is discarded, assuming a length of zero on opening.
		constexpr inline std::ios::openmode convert_openmode(mode m)
		{
			return (bit_op::is_include(m, mode::read)	? std::ios::in		: 0)
				 | (bit_op::is_include(m, mode::write)	? std::ios::out		: 0)
				 | (bit_op::is_include(m, mode::append)	? std::ios::app		: 0)
				 | (bit_op::is_include(m, mode::at_end)	? std::ios::ate		: 0)
				 | (bit_op::is_include(m, mode::create)	? std::ios::trunc	: 0)
				 | (bit_op::is_include(m, mode::binary)	? std::ios::binary	: 0)
				;
		}
	}
}
#endif
