#ifndef __DL_FILESYSTEM__
#define __DL_FILESYSTEM__
#pragma once

#include <common/common.h>
#include "file.h"
#include "fsfwd.h"
#include <string>
#include <vector>
#include <filesystem>

namespace dl { 
    namespace filesystem {

        class basic_file_system {
        public:
			using file_type = std::unique_ptr<basic_file>;

        public:
            basic_file_system() = default;
            virtual ~basic_file_system() = default;

            virtual mount_type type() const = 0;

            virtual const path_type& absolute(const path_type& path) = 0;

            // virtual path
            virtual void set_alias(const path_type& path) { alias = path; }
            virtual path_type get_alias() const { return alias; }

            virtual path_type adjust_path(const path_type& path) const = 0;

            virtual file_type open(const path_type& path, mode mode) const = 0;
            virtual bool close(file_type& file) const = 0;

            virtual bool create(const path_type& path) = 0;
            virtual bool remove(const path_type& path, bool bRecursive = false) = 0;

        protected:
            path_type absolute_path;
            path_type alias;
        };



		class natural_file
			: public basic_file
		{
		public:
			friend class filesystem;
			using openmode = std::ios::openmode;

		public:
            size_type read(byte* buf, size_type len = npos) override;
            size_type write(const byte* buf, size_type len) override;

			bool is_open() const override { return handler.is_open(); }

		private:
			std::fstream handler;

			natural_file(const path_type& path, mode flags = mode::read)
				: handler{ path, convert_openmode(flags) }
				, basic_file
				  { 
				  	mount_type::file
				  	, flags
				  	, bit_op::is_include(flags, mode::write) 
						? 0 : std::filesystem::file_size(path)
				  }
			{}
		};

		class filesystem 
			: public basic_file_system 
		{
		public:
			using file_type = std::unique_ptr<basic_file>;

		public:
			filesystem() = default;
			~filesystem() override = default;

			mount_type type() const override { return mount_type::file; }

			const path_type& absolute(const path_type& path) override;
			path_type adjust_path(const path_type& path) const override;

			file_type open(const path_type& path, mode mode) const override;
			bool close(file_type& file) const override;

			bool create(const path_type& path) override;
			bool remove(const path_type& path, bool bRecursive = false) override;

		};
    }
}

#endif
