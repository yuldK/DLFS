#include "pack.h"

using namespace dl::filesystem;

pack::file_type pack::open(const string& path, mode /*mode*/) const
{
	// TODO: adjust path!
	string adj_path = path;
	if (0 != filelist.count(adj_path))
	{
		return file_type{ nullptr };
	}

	auto&& info = filelist.at(adj_path);

	pack_file* res = new pack_file{ this };
	res->filename = std::move(adj_path);
	res->size = info.size;
	res->pos = info.offset;

	return std::unique_ptr<pack_file>{ res };
}

bool dl::filesystem::pack::close(file_type& file) const
{
	file = nullptr;
	return true;
}

size_t dl::filesystem::pack::size(const file_type& file) const
{
	return file->size;
}

size_t dl::filesystem::pack::read(file_type& file, size_t len, byte* buf)
{
	std::lock_guard<std::mutex> guard{cs};
	if (raw.flags() & std::ios::in) return 0;

	raw.seekg(file->pos);
	
	if (file->pos + len > file->end) {
		len = file->end - file->pos;
	}

	raw.read(buf, len);
	return len;
}

size_t dl::filesystem::pack::write(file_type& file, size_t len, const byte* buf)
{
	std::lock_guard<std::mutex> guard{ cs };
	if (raw.flags() & std::ios::out) return 0;

	// not implemented ...
	raw.seekg(file->pos);

	if (file->pos + len > file->end) {
		len = file->end - file->pos;
	}

	raw.write(buf, len);
	return len;
}



