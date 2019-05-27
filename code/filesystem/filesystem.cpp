#include "filesystem.h"
#include <filesystem>

using namespace dl::filesystem;
namespace std_fs = std::filesystem;



std::string replace(const std::string& str, const std::string& from, const std::string& to)
{
	string res = str;
	size_t start_pos = 0; //string 처음부터 검사
	//from을 찾을 수 없을 때까지
	if ((start_pos = res.find(from, start_pos)) != std::string::npos)  
	{
		res.replace(start_pos, from.length(), to);
		// 중복검사를 피하고 from.length() > to.length()인 경우를 위해서
		start_pos += to.length(); 
	}
	return res;
}



const string& filesystem::absolute(const string& path)
{
	return absolute_path = (std_fs::canonical(path) / "").string();
}

filesystem::file_type filesystem::open(const string& path, mode mode) const
{
	// TODO: adjust path!
	
	std::string real_path{ replace(path, alias, absolute_path) };
	if (!std_fs::exists(real_path)) {
		return file_type{};
	}
	
	natural_file* res = new natural_file{ real_path, mode };
	
	if (!res->handler.is_open()) {
		return file_type{};
	}
	
	res->filename = std::move(real_path);
	res->size = std_fs::file_size(res->filename);
	res->pos = 0;
	
	res->beg = 0;
	res->end = res->size;
	
	return std::unique_ptr<natural_file>{ res };
}

bool filesystem::close(file_type& file) const
{
	if (file->type != mount_type::file) return false;

	auto raw = static_cast<natural_file*>(file.get());
	raw->handler.close();
	return true;
}

size_t filesystem::size(const file_type& file) const
{
	return file->size;
}

size_t filesystem::read(file_type& file, size_t len, byte* buf)
{
	if (file->type != mount_type::file) return 0;

	auto& handler = static_cast<natural_file*>(file.get())->handler;
//	if ((handler.flags() & std::ios::in) == 0) return 0;
	handler.read(buf, len);
	return len;
}

size_t filesystem::write(file_type& file, size_t len, const byte* buf)
{
	if (file->type != mount_type::file) return 0;

	auto& handler = static_cast<natural_file*>(file.get())->handler;
//	if ((handler.flags() & std::ios::out) == 0) return 0;
	
	handler.write(buf, len);
	return len;
}

bool filesystem::create(const string& path)
{
	return std_fs::create_directories(path);
}

bool filesystem::remove(const string& path, bool bRecursive)
{
	if (bRecursive) return std_fs::remove(path);
	return std_fs::remove_all(path);
}
