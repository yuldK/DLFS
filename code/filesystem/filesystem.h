#ifndef __DL_FILESYSTEM__
#define __DL_FILESYSTEM__
#pragma once

#include <common/common.h>
#include "file.h"
#include "fs_type.h"
#include <string>
#include <vector>

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

            virtual file_type open(const string& path, mode mode) const = 0;
            virtual bool close(file_type& file) const = 0;

            virtual size_t size(const file_type& file) const = 0;
			virtual size_t read(file_type& file, byte* buf) { return read(file, file->size, buf); }
			virtual size_t read(file_type& file, size_t len, byte* buf) = 0;
            virtual size_t write(file_type& file, size_t len, const byte* buf) = 0;


            virtual bool create(const string& path) = 0;
            virtual bool remove(const string& path, bool bRecursive = false) = 0;

        protected:
            string absolute_path;
            string alias;
        };

		class filesystem : public basic_file_system {
		public:
			using file_type = std::unique_ptr<basic_file>;
			using basic_file_system::read;

		public:
			filesystem() = default;
			~filesystem() override = default;

			mount_type type() const override { return mount_type::file; }

			const string& absolute(const string& path) override;

			// virtual path
		//	virtual void set_alias(const string& path) { alias = path; }
		//	virtual string get_alias() const { return alias; }

			file_type open(const string& path, mode mode) const override;
			bool close(file_type& file) const override;

			size_t size(const file_type& file) const override;
			size_t read(file_type& file, size_t len, byte* buf) override;
			size_t write(file_type& file, size_t len, const byte* buf) override;


			bool create(const string& path) override;
			bool remove(const string& path, bool bRecursive = false) override;

		};
    }
}

#endif
