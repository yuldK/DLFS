#include "pack.h"

using namespace dl::filesystem;

pack::file_type pack::open(const string& path, mode flags) const
{
	// TODO: adjust path!
	string adj_path = path;
	if (0 != filelist.count(adj_path))
	{
		return file_type{ nullptr };
	}

	auto&& info = filelist.at(adj_path);

	pack_file* res = new pack_file	{	const_cast<pack&>(*this), 
										flags, 
										info.offset, 
										info.size
									};

	return std::unique_ptr<pack_file>{ res };
}

bool pack::close(file_type& file) const
{
	file = nullptr;
	return true;
}

size_t pack_file::read(byte* buf, size_t len)
{
	std::lock_guard<std::mutex> guard{ handler.cs };
	if (!has_flag(mode::read)) return 0;
	
	const auto localend = end - beg;
	if (pos + len >= localend) len = localend - (1 + pos);
	
	handler.raw.seekg(pos + beg);
	handler.raw.read(buf, len);
	seekg(len, seek_dir::cur);
	return len;
}

size_t pack_file::write(const byte* /*buf*/, size_t len)
{
	std::lock_guard<std::mutex> guard{ handler.cs };
	if (!has_flag(mode::write)) return 0;

	// not implemented ...
//	handler.append(buf, len);
	return len;
}



