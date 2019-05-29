#ifndef __DL_FS_FILE__
#define __DL_FS_FILE__
#pragma once

#include <iosfwd>
#include <fstream>
#include "fsfwd.h"
#include <mutex>

#if defined(_DEBUG) && 0
#define USE_FILENAME 
#endif

namespace dl {
    namespace filesystem {

        class basic_file {
		public:
			using size_type = std::streamsize;
			using offset_type = std::streamoff;

			constexpr static offset_type npos =
				std::numeric_limits<offset_type>::max();

			friend class pack;
			friend class filesystem;
			friend class basic_file_system;
			
		public:
			virtual size_type read(byte* buf, size_type len = npos) = 0;
            virtual size_type write(const byte* buf, size_type len) = 0;

			virtual bool is_open() const = 0;

			virtual size_type size() const { return end - beg; }
			virtual bool eof() const { return pos >= end; }

			bool has_flag(mode mode) const { return ((flags & mode) != mode::unknown); }
			
			virtual bool seekg(offset_type Off, seek_dir dir = seek_dir::beg)
			{
				if (!has_flag(mode::read)) return false;
				switch (dir)
				{
				case seek_dir::beg:
					if (beg + Off >= end)	return false;
					pos = beg + Off;		return true;
				case seek_dir::cur:
					if (pos + Off >= end)	return false;
					pos = pos + Off;		return true;
				case seek_dir::end:
					if (beg + Off >= end)	return false;
					pos = end - (1 + Off);	return true;
				}
				return true;
			}

			virtual offset_type tellg() const 
			{ 
				if (!has_flag(mode::read)) return false;
				return pos - beg;
			}
			
		protected:
#ifdef USE_FILENAME
			basic_file(mount_type		type
					 , mode				flags
					 , const string&	fn
					 , offset_type		end
					 , offset_type		begin = 0
			)
				: type{ type }
				, flags{ flags }
				, beg{ begin }
				, end{ end }
				, filename{ fn }
			{}
#endif
			basic_file(mount_type	type
					 , mode			flags
					 , offset_type	end
					 , offset_type	begin = 0
			)
				: type{ type }
				, flags{ flags }
				, beg{ begin }
				, end{ end }
			{}

		protected:
			const mount_type type;
			const mode flags = mode::unknown;

#ifdef USE_FILENAME
            string filename;
#endif
			offset_type pos = 0;

			const offset_type beg = 0;
			const offset_type end = 0;
        };
    }
}

#endif
