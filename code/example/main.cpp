#include "pch.h"
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <common/common.h>
#include <filesystem/fs_type.h>
#include <common/bit_op.h>

#include <filesystem/filesystem.h>

// 실제 사용 시 dlfs::open 등의 방식으로 열 생각
// singleton 사용을 지양하고 가능하면 전역 함수의 방식으로 진행할 것
// 초기화를 하지 않으면 실행이 안 되게 하는 프로세스는
// 함수 포인터를 할당하지 않다가 초기화 함수에서 할당하는 건 어떨까?

#include <filesystem>

int main()
{
    using namespace fs;
    constexpr mode val = mode::read ^ mode::write;
    constexpr mode val2 = val | mode::write;

//    std::cout << std::boolalpha << dl::bit_op::is_include(val2, mode::read) << std::endl;
//    decltype(mode::value_type{} | mode::value_type{}) a;
//    mode my{ mode::write };
//    constexpr mode p = ( mode::read | mode::write );
//    mode q { mode::read | mode::write };
//    my &= mode::read;
// 
	filesystem fs;
	fs.absolute(std::filesystem::current_path().string());
	fs.set_alias("/");
	
	auto fp = fs.open("/sample.pack", fs::mode::read);
	char buf[1024];
	buf[fs.read(fp, buf)] = '\0';
	std::cout << buf << std::endl;
	fs.close(fp);
}
