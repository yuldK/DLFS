#include "pack.h"
#include <filesystem>
using namespace dl::filesystem;
namespace std_fs = std::filesystem;

string pack::adjust_path(const string& path) const
{
	using namespace std::string_literals;

	string res = path;
	if (res.find(alias, 0) == 0)
		res.replace(0, alias.length(), ""s);
	return res;
}

pack::file_type pack::open(const string& path, mode flags) const
{
	// TODO: adjust path!
	string adj_path{ adjust_path(path) };
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

// must to be not work!
//	pack system's write operation is 
//	just worked in pack generation process...
size_t pack_file::write(const byte* /*buf*/, size_t /*len*/)
{
	return 0;
}

#include <chrono>
#include <vector>

// only used local
namespace 
{
	template<typename T>
	constexpr void casting_assign(byte*& pointer, const T& value)
	{
		*reinterpret_cast<T*>(pointer) = value;
		std::advance(pointer, sizeof(T));
	}

	void casting_assign(byte*& pointer, const char* str, size_t len)
	{
		*reinterpret_cast<uint8_t*>(pointer) = static_cast<uint8_t>(len);
		std::advance(pointer, 1);
		for (size_t i = 0; i < len; ++i) pointer[i] = str[i];
		std::advance(pointer, len);
	}

	template<>
	void casting_assign(byte * &pointer, const string & value)
	{
		return casting_assign(pointer, value.data(), value.length());
	}

	struct fileinfo
	{
		string path;
		size_t size;

		fileinfo(string&& path, size_t size)
			: path{ std::move(path) }
			, size{ size }
		{}
	};

	bool get_filelist(std::vector<fileinfo>& vfilelist, const std_fs::path& base, const std_fs::path& path, bool bRecursive = true)
	{
		if (!std_fs::is_directory(path)) return false;
		for (auto&& entry : std_fs::directory_iterator{ path })
		{
			if (std_fs::is_regular_file(entry.status()))
				vfilelist.emplace_back(	  entry.path().lexically_relative(base).string()
										, std_fs::file_size(entry.path())
				);
			else if (bRecursive && std_fs::is_directory(entry.status()))
				get_filelist(vfilelist, base, entry.path(), bRecursive);
		}
		return true;
	}
}



bool pack::generate(const string& path, const string& alias, const string& out_dir, const string& filter_path)
{
	filter_path;
	if (!std_fs::exists(path)) return false;

	const std_fs::path base_path = path;
	std::vector<fileinfo> filelist;
	if (!get_filelist(filelist, base_path, path)) return false;

	// 1 kb
	byte header[1024];
	byte* hp = header;

	// signiture, 4 byte
	casting_assign(hp, DL_MAGIC_SIGNITURE);

	// alias, 1 byte(len) + length 
	casting_assign(hp, alias);

	// timestamp, 8 byte
	using clock = std::chrono::system_clock;
	casting_assign(hp, clock::to_time_t(clock::now()));

	// file amount, 2 byte
	casting_assign(hp, static_cast<uint16_t>(filelist.size()));

	

	constexpr auto flags	= std::ios::out 
							| std::ios::trunc 
							| std::ios::binary
		;

	std::ofstream out{ out_dir, flags };

	if (!out.is_open()) return false;

	out.write(header, 1024);
	
	using off_type = std::ofstream::off_type;

	off_type off_body = 1024;
	off_type off_raw = off_body;

	size_t best_size = 0;

	constexpr auto body_static_size = sizeof(uint8_t) + sizeof(off_type) + sizeof(size_t);
	for (const auto& p : filelist)
	{
		off_raw += (p.path.length() + body_static_size);
		best_size = std::max(best_size, p.size);
	}

	// { offset(8 byte) + size(8 byte) + length (1 byte) + string length() }
	byte body[body_static_size + 256];
	for (auto&& info : filelist)
	{
		byte* pointer = body;
		std::streamsize count = body_static_size + info.path.length();

		casting_assign(pointer, off_raw);
		casting_assign(pointer, info.size);
		casting_assign(pointer, info.path);

		out.write(body, count);
		off_body += count;
		off_raw += info.size;
	}

	if (out.tellp() != off_body)
	{
		out.clear();
		throw std::exception{ "calculation fail: don't match file size..." };
		return false;
	}

	std::vector<byte> data;
	data.resize(best_size);
	for (auto&& info : filelist)
	{
		std::ifstream in{ base_path / info.path, std::ios::binary | std::ios::in };

		if (in.is_open()) in.read(data.data(), info.size);
		else
		{ 
			throw std::exception{ "do not read file exception" };
			std::fill(data.begin(), std::next(data.begin(), info.size), '\0');
		}

		out.write(data.data(), info.size);
	}

	out.close();
	
	return true;
}


