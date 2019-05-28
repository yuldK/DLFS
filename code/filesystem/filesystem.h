#ifndef __DL_FILESYSTEM__
#define __DL_FILESYSTEM__
#pragma once

#include <common/common.h>
#include "file.h"
#include "fs_type.h"
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

            virtual const string& absolute(const string& path) = 0;
			
            // virtual path
            virtual void set_alias(const string& path) { alias = path; }
            virtual string get_alias() const { return alias; }

			virtual string adjust_path(const string& path) const = 0;

            virtual file_type open(const string& path, mode mode) const = 0;
            virtual bool close(file_type& file) const = 0;

            virtual bool create(const string& path) = 0;
            virtual bool remove(const string& path, bool bRecursive = false) = 0;

        protected:
            string absolute_path;
            string alias;
        };



		class natural_file
			: public basic_file
		{
		public:
			friend class filesystem;
			using openmode = std::ios::openmode;

		public:
			size_t read(byte* buf, size_t len = npos) override;
			size_t write(const byte* buf, size_t len) override;

			bool is_open() const override { return handler.is_open(); }

		private:
			std::fstream handler;

			natural_file(const string& path, mode flags = mode::read)
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

			const string& absolute(const string& path) override;
			string adjust_path(const string& path) const override;

			file_type open(const string& path, mode mode) const override;
			bool close(file_type& file) const override;

			bool create(const string& path) override;
			bool remove(const string& path, bool bRecursive = false) override;

		};
    }
}

#endif
