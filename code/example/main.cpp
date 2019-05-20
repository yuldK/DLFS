#include "pch.h"
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <common/common.h>
#include <filesystem/fs_type.h>
#include <common/bit_op.h>


int main()
{
    using namespace fs;
    constexpr mode val = mode::read ^ mode::write;
    constexpr mode val2 = val | mode::write;

    std::cout << std::boolalpha << dl::bit_op::is_include(val2, mode::read) << std::endl;
//    decltype(mode::value_type{} | mode::value_type{}) a;
//    mode my{ mode::write };
//    constexpr mode p = ( mode::read | mode::write );
//    mode q { mode::read | mode::write };
//    my &= mode::read;
// 
}
