#ifndef __DL_FS_PACK__
#define __DL_FS_PACK__
#pragma once

#include "filesystem.h"

#include <mutex>
#include <memory>
#include <fstream>
#include <unordered_map>

namespace dl {
	namespace filesystem {

		class pack_file
			: public basic_file
		{
		public:
			friend class pack;
			using openmode = std::ios::openmode;

		public:
			size_t read(byte* buf, size_t len = npos) override;
			size_t write(const byte* buf, size_t len) override;

			bool is_open() const override { return true; }

		private:
			pack& handler;
			pack_file(pack& p
					, mode flags
					, offset_type begin
					, offset_type end) 
				: handler{ p }
				, basic_file{ mount_type::pack, flags, end, begin }
			{}
		};

		class pack : public basic_file_system {
			friend class pack_file;
		public:
		public:
			pack() = default;
			virtual ~pack() = default;

			mount_type type() const override { return mount_type::pack; }

			const string& absolute(const string& path) override { return absolute_path = path; }
			string adjust_path(const string& path) const override;

			file_type open(const string& path, mode mode) const override;
			bool close(file_type& file) const override;

			bool create(const string& path) override { path; return false; }
			bool remove(const string& path, bool bRecursive = false) override { path; bRecursive; return false; }

			static bool generate( const string& path
								, const string& alias
								, const string& out
								, const string& filter_path = string{}
			);

		private:
			std::fstream raw;
			std::mutex cs;

			struct file_info {
				size_t offset;
				size_t size;
			};
			std::unordered_map<string, file_info> filelist;

		};

	}
}


#endif
