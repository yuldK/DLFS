#ifndef __DL_FS_PACK__
#define __DL_FS_PACK__
#pragma once

#include "filesystem.h"

#include <mutex>
#include <memory>
#include <fstream>
#include <unordered_map>

#define NOMINMAX
#include <mio/single_include/mio/mio.hpp>

namespace dl {
	namespace filesystem {

		class pack_file
			: public basic_file
		{
		public:
			friend class pack;
			using openmode = std::ios::openmode;

		public:
			size_type read(fs::byte* buf, size_type len = npos) override;

            // must to be not work!
            //	pack system's write operation is 
            //	just worked in pack generation process...
            size_type write(const fs::byte*, size_type) override { return 0; }

			bool is_open() const override { return true; }

		private:
			mio::mmap_source handler;
			pack_file(mio::file_handle_type handle
					, mode flags
					, offset_type begin
					, size_type size) 
				: handler{ handle, begin, size }
				, basic_file{ mount_type::pack, flags, begin + size, begin }
			{}
		};

		class pack 
            : public basic_file_system 
        {
		public:
			friend class pack_file;

		public:
            pack(const path_type& path);
			virtual ~pack() = default;

			mount_type type() const override { return mount_type::pack; }

            // do not change absolute path
            const path_type& absolute(const path_type& /*path*/) override { return absolute_path; }
            virtual void set_alias(const path_type& /*path*/) override {}

            path_type adjust_path(const path_type& path) const override;

			file_type open(const path_type& path, mode mode = mode::read | mode::binary) const override;
			bool close(file_type& file) const override;

			bool create(const path_type& path) override { path; return false; }
			bool remove(const path_type& path, bool bRecursive = false) override { path; bRecursive; return false; }

			static bool generate( const path_type& path
								, const path_type& alias
								, const path_type& out
								, const path_type& filter_path = path_type{}
			);

        private:
            void open();

		private:
			mio::mmap_source raw;

			struct record_info {
                pack_file::offset_type offset;
                pack_file::size_type size;
			};
			std::unordered_map<string, record_info> filelist;

		};

	}
}

#endif
