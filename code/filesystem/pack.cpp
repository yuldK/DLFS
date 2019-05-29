#include "pack.h"
#include "path_util.h"
#include <filesystem>

using namespace dl::filesystem;
namespace std_fs = std::filesystem;



path_type pack::adjust_path(const path_type& path) const
{
	return alias_relative(path, absolute_path, alias);
}

pack::file_type pack::open(const path_type& path, mode flags) const
{
	// TODO: adjust path!
    string adj_path{ adjust_path(path) };
	if (0 == filelist.count(adj_path))
	{
		return file_type{ nullptr };
	}

	auto&& info = filelist.at(adj_path);

	pack_file* res = new pack_file	{	raw.file_handle(), 
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

pack_file::size_type pack_file::read(fs::byte* buf, size_type len)
{
	if (!has_flag(mode::read)) return 0;
	
	const auto localend = end - beg;
	if (pos + len >= localend) len = localend - (1 + pos);
	
    memcpy(reinterpret_cast<char*>(buf), &handler[pos], len);

	return len;
}

#include <chrono>
#include <vector>

// only used local
namespace 
{
	template<typename T>
	void write_buffer(fs::byte*& pointer, const T& value)
    {
        *reinterpret_cast<T*>(pointer) = value;
        std::advance(pointer, sizeof(T));
    }

    void write_buffer(fs::byte*& pointer, const char* str, size_t len)
    {
        write_buffer(pointer, static_cast<uint8_t>(len));
        memcpy(pointer, str, len);
        std::advance(pointer, len);
    }

    template<>
    void write_buffer(fs::byte*& pointer, const string& value)
	{
		return write_buffer(pointer, value.data(), value.length());
	}



    template<typename T>
    T read_buffer(const char*& pointer)
    {
        T value = *reinterpret_cast<const T*>(pointer);
        std::advance(pointer, sizeof(T));
        return value;
    }
    
    void read_buffer(fs::byte* value, uint8_t len, const char*& pointer)
    {
        memcpy(value, pointer, len);
        std::advance(pointer, len);
    }

    fs::string read_buffer(uint8_t len, const char*& pointer)
    {
        fs::string res{ pointer, pointer + len };
        std::advance(pointer, len);
        return res;
    }

    template<typename T>
    void read_buffer(T& value, const char*& pointer)
    {
        value = read_buffer<T>(pointer);
    }

    template<>
    void read_buffer(string& value, const char*& pointer)
    {
        value = std::move(read_buffer(read_buffer<uint8_t>(pointer), pointer));
    }



	struct file_info
	{
		std_fs::path real;
		string path;
		size_t size;

		file_info(std_fs::path&& real, string&& path, size_t size)
			: real{ std::move(real) }
			, path{ std::move(path) }
			, size{ size }
		{}
	};

	bool get_filelist(std::vector<file_info>& vfilelist, const path_type& base, const path_type& alias, const std_fs::path& path, bool bRecursive = true)
	{
		if (!std_fs::is_directory(path)) return false;
		for (auto&& entry : std_fs::directory_iterator{ path })
		{
			if (std_fs::is_regular_file(entry.status()))
				vfilelist.emplace_back(	  std_fs::absolute(entry.path())
										, alias_relative(entry.path().generic_string(), base, alias)
										, std_fs::file_size(entry.path())
				);
			else if (bRecursive && std_fs::is_directory(entry.status()))
				get_filelist(vfilelist, base, alias, entry.path(), bRecursive);
		}
		return true;
	}
}



using off_type = pack_file::offset_type;
constexpr auto body_static_size = sizeof(uint8_t) + sizeof(off_type) + sizeof(size_t);

pack::pack(const path_type& path)
{
    absolute_path = path;
    open();
}

void pack::open()
{
    if (!std_fs::exists(absolute_path)) throw std::exception{ "no file exists!" };

    std::error_code err;
    raw.map(remove_slash(absolute_path), err);

    if (err) throw std::exception{ "do not open an invalid file!" };
    const char* rp = &raw[0];
    // read header
    if (read_buffer<uint32_t>(rp) != DL_MAGIC_SIGNITURE) throw std::exception{ "signiture dismatched!" };

    // get alias
    read_buffer(alias, rp);

    // get timestamp
    __time64_t timestamp;
    read_buffer(timestamp, rp);

    uint16_t filelist_amount;
    read_buffer(filelist_amount, rp);

    // jump to read body
    rp = &raw[1024];

    for (size_t i = 0; i < filelist_amount; ++i) {
        record_info info;
        string key_path;

        read_buffer(info.offset, rp);
        read_buffer(info.size, rp);
        read_buffer(key_path, rp);

        filelist.insert(std::make_pair(std::move(key_path), std::move(info)));
    }
}

bool pack::generate(const path_type& path, const path_type& alias, const path_type& out_dir, const path_type& filter_path)
{
    filter_path;
    if (!std_fs::exists(path)) return false;

    std::vector<file_info> filelist;
    if (!get_filelist(filelist, path, alias, path)) return false;

    // 1 kb
    fs::byte header[1024];
    fs::byte* hp = header;

    // signiture, 4 byte
    write_buffer(hp, DL_MAGIC_SIGNITURE);

    // alias, 1 byte(len) + length 
    write_buffer(hp, alias);

    // timestamp, 8 byte
    using clock = std::chrono::system_clock;
    write_buffer(hp, clock::to_time_t(clock::now()));

    // file amount, 2 byte
    write_buffer(hp, static_cast<uint16_t>(filelist.size()));



    constexpr auto flags = std::ios::out
        | std::ios::trunc
        | std::ios::binary
        ;

    std::ofstream out{ out_dir, flags };

    if (!out.is_open()) return false;


    out.write(reinterpret_cast<char*>(header), 1024);

    off_type off_body = 1024;
    off_type off_raw = off_body;

	size_t best_size = 0;

	for (const auto& p : filelist)
	{
		off_raw += (p.path.length() + body_static_size);
		best_size = std::max(best_size, p.size);
	}

	// { offset(8 byte) + size(8 byte) + length (1 byte) + string length() }
	fs::byte body[body_static_size + 256];
	for (auto&& info : filelist)
	{
		fs::byte* pointer = body;
        pack_file::size_type count = body_static_size + info.path.length();

		write_buffer(pointer, off_raw);
		write_buffer(pointer, info.size);
		write_buffer(pointer, info.path);

		out.write(reinterpret_cast<const char*>(body), count);
		off_body += count;
		off_raw += info.size;
	}

	if (out.tellp() != off_body)
	{
		out.clear();
		throw std::exception{ "calculation fail: don't match file size..." };
	//	return false;
	}

	std::vector<fs::byte> data;
	data.resize(best_size);
	for (auto&& info : filelist)
	{
		std::ifstream in{ info.real, std::ios::binary | std::ios::in };

		if (in.is_open()) in.read(reinterpret_cast<char*>(data.data()), info.size);
		else
		{ 
			throw std::exception{ "do not read file exception" };
		//	std::fill(data.begin(), std::next(data.begin(), info.size), '\0');
		}

		out.write(reinterpret_cast<const char*>(data.data()), info.size);
	}

	out.close();
	
	return true;
}


