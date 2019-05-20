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
            friend class file;
        public:

        public:
            basic_file_system() = default;
            virtual ~basic_file_system() = default;

            virtual mount_type type() const = 0;

            virtual string absolute(const string& path) const = 0;

            // virtual path
            virtual void set_alias(const string& path) { alias = path; }
            virtual string get_alias() const { return alias; }

            virtual file open(const string& path, mode mode) const = 0;
            virtual bool close(file& file) const = 0;

            virtual size_t size(file& file) = 0;
            virtual size_t read(file& file, size_t len, byte* buf) = 0;
            virtual size_t write(file& file, size_t len, byte* buf) = 0;


            virtual bool create(const string& path) = 0;
            virtual bool remove(const string& path, bool bRecursive = false) = 0;

        protected:
            string absolute_path;
            string alias;
        };

    }
}

#endif
