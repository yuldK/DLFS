#ifndef __DL_FS_FILE__
#define __DL_FS_FILE__
#pragma once

#include <fstream>
#include "fs_type.h"
#include <mutex>

namespace dl {
    namespace filesystem {

		class pack;

		// 실제로 pack에서 호출할 땐 pack의
		// file stream을 가지도록 한다.
		// read 연산일 경우 여러 군데에서 읽어도 되는지
		// 관련 레퍼런스를 조사한다.
		// 읽는 건 휘발성 연산이라고 생각.
		// eof 구현할 것. eof 뿐 아니라 C style의 필요 함수 wrapping
        class basic_file {
		public:
			using size_type = size_t;
			using offset_type = size_t;

			friend class pack;
			friend class filesystem;
			friend class basic_file_system;
			
		protected:
			basic_file(mount_type type)
				: type{ type } 
			{}

        protected:
			const mount_type type;

			// 꼭 있어야 하는걸까?
            string filename;

            size_type size = 0;
			offset_type pos = 0;

			// pack offset
			offset_type beg = 0;
			// beg + size
			offset_type end = 0;
        };

		class natural_file 
			: public basic_file 
		{
		public:
			friend class filesystem;
			using openmode = std::ios::openmode;
			// IO operation
		private:
			std::fstream handler;
			natural_file(const string& path, mode flags = mode::read)
				: handler{ path, convert_openmode(flags) }
				, basic_file{ mount_type::file }
			{}
		};

		class pack_file 
			: public basic_file
		{
		public:
			friend class pack;
			// IO operation
		private:
			const pack* const handler;
			pack_file(const pack* p) 
				: handler{ p }
				, basic_file{ mount_type::pack }
			{}
		};

    }
}

#endif
