#ifndef __DL_FS_BIT_OP__
#define __DL_FS_BIT_OP__

#pragma once

#include <type_traits>

namespace dl {

    // 2019.05.10 | yul
    // bit operation helper
    // usage : 
    //  Create the following structure for the enum value 
    //  you want to use for the bit operation:
    //
    //      template<> bit_op::use<ENUM>{};
    //
    //  This step can be done simply through a macro:
    //
    //      bit_op::use_if_op(ENUM)
    //
    namespace bit_op {
        template <typename E>
        struct use {
            using specialized = void;
        };

        template <typename E, typename = void>
        struct use_eval : std::bool_constant<std::is_enum_v<E>> 
        {};

        template <typename E>
        struct use_eval<E, typename use<E>::specialized> : std::false_type 
        {};

        template <typename E>
        constexpr bool use_eval_v = use_eval<E>::value;

        template <typename E> // enum sturcture
        using return_t = std::enable_if_t<use_eval_v<std::decay_t<E>>, std::decay_t<E>>;

        template <typename E>
        constexpr bool is_include(E value, E eval)
        {
            static_assert(use_eval_v<E>, "Template arguments for bit_op use only enum types!");
            return (value & eval) == eval;
        }
    }
}

template <typename E> // enum sturcture
inline constexpr auto operator^(E lhs, E rhs) -> dl::bit_op::return_t<E>
{
    using decay_E = std::decay_t<E>;
    using raw_type = std::underlying_type_t<decay_E>;
    return static_cast<decay_E>(static_cast<raw_type>(lhs) ^ static_cast<raw_type>(rhs));
}

template <typename E> // enum sturcture
inline constexpr auto operator&(E lhs, E rhs) -> dl::bit_op::return_t<E>
{
    using decay_E = std::decay_t<E>;
    using raw_type = std::underlying_type_t<decay_E>;
    return static_cast<decay_E>(static_cast<raw_type>(lhs) & static_cast<raw_type>(rhs));
}

template <typename E> // enum sturcture
inline constexpr auto operator|(E lhs, E rhs) -> dl::bit_op::return_t<E>
{
    using decay_E = std::decay_t<E>;
    using raw_type = std::underlying_type_t<decay_E>;
    return static_cast<decay_E>(static_cast<raw_type>(lhs) | static_cast<raw_type>(rhs));
}

#define use_bit_op(E) template <> struct dl::bit_op::use<E> {};

#endif
