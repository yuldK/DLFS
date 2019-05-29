#include "filesystem.h"
#include "path_util.h"
#include <filesystem>

using namespace dl::filesystem;
namespace std_fs = std::filesystem;



const path_type& filesystem::absolute(const path_type& path)
{
	return absolute_path = add_slash(path, '/');
}

path_type filesystem::adjust_path(const path_type& path) const
{
	return alias_absolute(path, absolute_path, alias);
}

filesystem::file_type filesystem::open(const path_type& path, mode mode) const
{
	std::string real_path{ adjust_path(path) };
	if (!bit_op::is_include(mode, mode::write) 
	&&  !std_fs::exists(real_path)) {
		return file_type{};
	}
	
	natural_file* res = new natural_file{ real_path, mode };
	
	if (!res->handler.is_open()) {
		return file_type{};
	}
	
	return std::unique_ptr<natural_file>{ res };
}

bool filesystem::close(file_type& file) const
{
	if (file->type != mount_type::file) return false;

	auto raw = static_cast<natural_file*>(file.get());
	raw->handler.close();
	return true;
}

natural_file::size_type natural_file::read(byte* buf, size_type len)
{
	if (!has_flag(mode::read)) return 0;
	if (pos + len >= end) len = end - (1 + pos);

	handler.seekg(pos - beg);
	handler.read(reinterpret_cast<char*>(buf), len);
	seekg(len, seek_dir::cur);
	return len;
}

natural_file::size_type natural_file::write(const byte* buf, size_type len)
{
	if (!has_flag(mode::write)) return 0;

	handler.write(reinterpret_cast<const char*>(buf), len);
	pos += len;
	return len;
}

bool filesystem::create(const path_type& path)
{
	auto real_path{ adjust_path(path) };
	return std_fs::create_directories(real_path);
}

bool filesystem::remove(const path_type& path, bool bRecursive)
{
	auto real_path{ adjust_path(path) };
	if (bRecursive) return std_fs::remove(real_path);
	return std_fs::remove_all(real_path);
}
