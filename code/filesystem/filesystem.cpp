#include "filesystem.h"
#include <filesystem>

using namespace dl::filesystem;
namespace std_fs = std::filesystem;



const string& filesystem::absolute(const string& path)
{
	return absolute_path = (std_fs::canonical(path) / "").string();
}

string filesystem::adjust_path(const string& path) const
{
	string res = path;
	if (res.find(alias, 0) == 0)
		res.replace(0, alias.length(), absolute_path);
	return res;
}

filesystem::file_type filesystem::open(const string& path, mode mode) const
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

size_t natural_file::read(byte* buf, size_t len)
{
	if (!has_flag(mode::read)) return 0;
	if (pos + len >= end) len = end - (1 + pos);

	handler.seekg(pos - beg);
	handler.read(buf, len);
	seekg(len, seek_dir::cur);
	return len;
}

size_t natural_file::write(const byte* buf, size_t len)
{
	if (!has_flag(mode::write)) return 0;

	handler.write(buf, len);
	pos += len;
	return len;
}

bool filesystem::create(const string& path)
{
	std::string real_path{ adjust_path(path) };
	return std_fs::create_directories(real_path);
}

bool filesystem::remove(const string& path, bool bRecursive)
{
	std::string real_path{ adjust_path(path) };
	if (bRecursive) return std_fs::remove(real_path);
	return std_fs::remove_all(real_path);
}
