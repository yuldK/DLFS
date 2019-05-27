#ifndef __DL_FS_PACK__
#define __DL_FS_PACK__
#pragma once

#include <filesystem.h>

#include <mutex>
#include <memory>
#include <fstream>
#include <unordered_map>

namespace dl {
	namespace filesystem {

		class pack : public basic_file_system {
			friend class pack_file;
		public:
		public:
			pack() = default;
			virtual ~pack() = default;

			mount_type type() const override { return mount_type::pack; }

			const string& absolute(const string& path) override { return absolute_path = path; }

			// virtual path
		//	virtual void set_alias(const string& path) { alias = path; }
		//	virtual string get_alias() const { return alias; }

			file_type open(const string& path, mode mode) const override;
			bool close(file_type& file) const override;

			size_t size(const file_type& file) const override;
			size_t read(file_type& file, size_t len, byte* buf) override;
			size_t write(file_type& file, size_t len, const byte* buf) override;


			virtual bool create(const string& path) = 0;
			virtual bool remove(const string& path, bool bRecursive = false) = 0;

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
