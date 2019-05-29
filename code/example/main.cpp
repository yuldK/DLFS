#include "pch.h"
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <common/common.h>
#include <filesystem/fsfwd.h>
#include <common/bit_op.h>

#include <filesystem/pack.h>
#include <filesystem/filesystem.h>

// 실제 사용 시 dlfs::open 등의 방식으로 열 생각
// singleton 사용을 지양하고 가능하면 전역 함수의 방식으로 진행할 것
// 초기화를 하지 않으면 실행이 안 되게 하는 프로세스는
// 함수 포인터를 할당하지 않다가 초기화 함수에서 할당하는 건 어떨까?

#include <filesystem>
std::filesystem::path current_path;

void read_operation(dl::filesystem::filesystem& fs, const std::string& path)
{
	using namespace fs;

	auto fp = fs.open(path, fs::mode::read | fs::mode::binary);

	std::vector<fs::byte> buf;
	buf.resize(fp->size());
	fp->read(buf.data());
	std::cout.write(reinterpret_cast<char*>(buf.data()), buf.size());
	std::cout << std::endl;
	fs.close(fp);
}

void write_operation(dl::filesystem::filesystem& fs, const std::string& path)
{
	using namespace fs;
	using namespace std::string_literals;

	constexpr auto flags = mode::create 
						 | mode::write 
						 | mode::binary
						 ;

	auto fp = fs.open(path, flags);

	std::string buf{ "hello, world!\nMy Name is yul."s };

	fp->write(reinterpret_cast<fs::byte*>(buf.data()), buf.length());
	fs.close(fp);
}

void pack_read()
{
    auto fail_exit = []() { std::cout << "fail to load pack..." << std::endl; };

    std::unique_ptr<fs::pack> pack;
    try {
        pack = std::make_unique<fs::pack>("bin/Debug/sample.pack");
    } catch (...) {
        return fail_exit();
    }
   
    auto fp = pack->open("/code/CMakeLists.txt");
    if (!fp) return fail_exit();

    std::string str;
    str.resize(fp->size());

    fp->read(reinterpret_cast<fs::byte*>(str.data()));

    std::cout << str << std::endl;
}

#include "filesystem/path_util.h"
#include <string_view>

int main()
{
	using namespace fs;
	current_path = std::filesystem::current_path().parent_path().parent_path();
	SetCurrentDirectory(current_path.string().c_str());

	filesystem fs;
	fs.absolute(current_path.string());
	fs.set_alias("/");
	fs.remove("/sample/");
	fs.create("/sample/");
	// read operation
	read_operation(fs, "/sample.pack");
	// write operation
	write_operation(fs, "/sample/sample.out");
	read_operation(fs, "/sample/sample.out");
	
	fs.remove("/sample/");

	pack::generate("code", "/code/", "bin/Debug/sample.pack");

    pack_read();
}
