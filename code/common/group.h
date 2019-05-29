#ifndef __DL_FS_GROUP__
#define __DL_FS_GROUP__
#pragma once

#include <tuple>

namespace group_compare {

	struct tag_group_compare {};
	struct tag_equal_compare : tag_group_compare {};
	struct tag_unequal_compare : tag_group_compare {};

	struct compare_option {};
	struct compare_all 
		: compare_option 
	{
		constexpr static bool equal_value = true;
		constexpr static bool unequal_value = false;

		template<typename lhs_t, typename rhs_t>
		constexpr static auto equal_op(lhs_t&& lhs, rhs_t&& rhs)
		{ return std::forward<lhs_t>(lhs) && std::forward<rhs_t>(rhs); }

		template<typename lhs_t, typename rhs_t>
		constexpr static auto unequal_op(lhs_t&& lhs, rhs_t&& rhs)
		{
			return std::forward<lhs_t>(lhs) || std::forward<rhs_t>(rhs);
		}
	};
	struct compare_any
		: compare_option
	{
		constexpr static bool equal_value = false;
		constexpr static bool unequal_value = true;

		template<typename lhs_t, typename rhs_t>
		constexpr static auto equal_op(lhs_t&& lhs, rhs_t&& rhs)
		{ return std::forward<lhs_t>(lhs) || std::forward<rhs_t>(rhs); }

		template<typename lhs_t, typename rhs_t>
		constexpr static auto unequal_op(lhs_t&& lhs, rhs_t&& rhs)
		{ return std::forward<lhs_t>(lhs) && std::forward<rhs_t>(rhs); }

	};

	template<class option, class ...Ts>
	class group
	{
		static_assert(std::is_base_of_v<compare_option, option>, "group option is must be base of compare option!");
		using compare_type = option;

		template<class option, class ...Args>
		friend constexpr auto make_group(Args&& ...);

		template<class ...Args>
		friend constexpr auto make_group_any(Args&& ...);

		template<class ...Args>
		friend constexpr auto make_group_all(Args&& ...);

		template<typename rhs_t, class ...Args>
		friend constexpr bool operator!=(rhs_t&&, const group<Args...>&);

		template<typename rhs_t, class ...Args>
		friend constexpr bool operator!=(rhs_t&&, group<Args...>&&);

		template<typename rhs_t, class ...Args>
		friend constexpr bool operator==(rhs_t&&, const group<Args...>&);

		template<typename rhs_t, class ...Args>
		friend constexpr bool operator==(rhs_t&&, group<Args...>&&);

	private:
		std::tuple<Ts...> groups;

	private:
		constexpr explicit group(Ts&& ...args)
			: groups{ std::forward<Ts>(args)... }
		{
		}

		template <typename rhs_t>
		constexpr bool compare_core(tag_equal_compare, rhs_t&&) const noexcept { return compare_type::equal_value; }

		template <typename rhs_t, typename Ty, typename... Args>
		constexpr bool compare_core(tag_equal_compare, rhs_t&& rhs, Ty&& ty, Args&& ... args) const noexcept
		{
			return compare_type::equal_op((rhs == std::forward<Ty>(ty)), compare_core(tag_equal_compare{}, std::forward<rhs_t>(rhs), std::forward<Args>(args)...));
		}

		template <typename rhs_t>
		constexpr bool compare_core(tag_unequal_compare, rhs_t&&) const noexcept { return compare_type::unequal_value; }

		template <typename rhs_t, typename Ty, typename... Args>
		constexpr bool compare_core(tag_unequal_compare, rhs_t&& rhs, Ty&& ty, Args&& ... args) const noexcept
		{
			return compare_type::unequal_op((rhs != std::forward<Ty>(ty)), compare_core(tag_unequal_compare{}, std::forward<rhs_t>(rhs), std::forward<Args>(args)...));
		}

		template <typename is_equal_comp, typename rhs_t, typename... Args, int... Is>
		constexpr bool compare(rhs_t&& rhs, const std::tuple<Args...>& tup, std::index_sequence<Is...>) const noexcept
		{
			return compare_core(is_equal_comp{}, std::forward<rhs_t>(rhs), std::get<Is>(tup)...);
		}

		template <typename is_equal_comp, typename rhs_t, typename... Args>
		constexpr bool compare(rhs_t&& rhs, const std::tuple<Args...>& tup) const noexcept
		{
			return compare<is_equal_comp>(
				  std::forward<rhs_t>(rhs)
				, tup
				, std::make_index_sequence<sizeof...(Args)>{}
			);
		}

		template <typename is_equal_comp, typename rhs_t>
		constexpr bool compare(rhs_t&& rhs) const noexcept
		{
			return compare<is_equal_comp>(std::forward<rhs_t>(rhs), groups);
		}
	};



	template<class option, class ...Args>
	constexpr auto make_group(Args&& ...args)
	{
		return group<option, Args...>(std::forward<Args>(args)...);
	}

	template<class ...Args>
	constexpr auto make_group_any(Args&& ...args)
	{
		return group<compare_any, Args...>(std::forward<Args>(args)...);
	}

	template<class ...Args>
	constexpr auto make_group_all(Args&& ...args)
	{
		return group<compare_all, Args...>(std::forward<Args>(args)...);
	}

	template<typename rhs_t, class ...Args>
	constexpr bool operator!=(rhs_t&& rhs, group_compare::group<Args...>&& lhs)
	{
		return lhs.compare<group_compare::tag_unequal_compare>(std::forward<rhs_t>(rhs));
	}
	template<typename rhs_t, class ...Args>
	constexpr bool operator!=(rhs_t&& rhs, const group_compare::group<Args...>& lhs)
	{
		return lhs.compare<group_compare::tag_unequal_compare>(std::forward<rhs_t>(rhs));
	}

	template<typename rhs_t, class ...Args>
	constexpr bool operator==(rhs_t&& rhs, group_compare::group<Args...>&& lhs)
	{
		return lhs.compare<group_compare::tag_equal_compare>(std::forward<rhs_t>(rhs));
	}
	template<typename rhs_t, class ...Args>
	constexpr bool operator==(rhs_t&& rhs, const group_compare::group<Args...>& lhs)
	{
		return lhs.compare<group_compare::tag_equal_compare>(std::forward<rhs_t>(rhs));
	}
}

#endif
