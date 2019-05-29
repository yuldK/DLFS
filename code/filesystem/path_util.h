#ifndef __DL_PATH_UTIL__
#define __DL_PATH_UTIL__
#pragma once

#include "fsfwd.h"
#include <algorithm>
#include <string_view>
#include <common/group.h>
#include <stack>

namespace dl {
	namespace path_util {

		using filesystem::string;
		using std::string_view;

		constexpr inline auto preferred_seperator = group_compare::make_group_any('\\', '/');

		// canonicalize path to preferred seperator is '/'
		inline string& canonical(string& path)
		{
			using namespace std::string_view_literals;

			// set preferred seperator '/'
			std::replace(std::begin(path), std::end(path), '\\', '/');
			
			// remove duplicated sep
			size_t off = 0;
			do {
				off = path.find('/', off);
				if (off == string::npos) break;

				while (++off < path.length() && path.at(off) == '/') 
				{
					path.erase(std::next(std::begin(path), off));
				}
			} while (true);

			std::stack<std::string_view> stack;

			off = 0;
			while (off < path.length())
			{
				auto beg = off;
				auto end = std::min(path.length(), path.find('/', beg + 1));
				stack.push(std::string_view{ &path[beg], end - beg });

				off = end;

				if (stack.top() == "/."sv)
				{
					stack.pop();
				}
				else if (stack.top() == "/.."sv)
				{
					stack.pop();
					stack.pop();
				}
			}
			
			string res;
			while (!stack.empty())
			{
				res.insert(0, stack.top());
				stack.pop();
			}

			return path = std::move(res);
		}

		inline string canonical(string_view path)
		{
			string res{ path };
			canonical(res);
			return res;
		}

		inline string& add_slash(string& path, string::value_type sep, size_t insert_pos = string::npos)
		{
			insert_pos = std::min(insert_pos, path.length());
			path.insert(std::next(std::begin(path), insert_pos), sep);

			return canonical(path);
		}

		inline string add_slash(string_view path, string::value_type sep, size_t insert_pos = string::npos)
		{
			string res{ path };
			insert_pos = std::min(insert_pos, path.length());
			res.insert(std::next(std::begin(res), insert_pos), sep);
			
			return canonical(res);
		}

		inline string& remove_slash(string& path, size_t insert_pos = string::npos)
		{
			insert_pos = std::min(insert_pos, path.length() - 1);
			if (path[insert_pos] == preferred_seperator) path.erase(std::next(std::begin(path), insert_pos));

			return canonical(path);
		}

		inline string remove_slash(string_view path, size_t insert_pos = string::npos)
		{

			string res{ path };
			insert_pos = std::min(insert_pos, path.length() - 1);
			if (res[insert_pos] == preferred_seperator) res.erase(std::next(std::begin(res), insert_pos));

			return canonical(res);
		}
		
		inline string alias_relative(string_view path, string_view base, string_view alias)
		{

			if (base[base.length() - 1] == preferred_seperator) base.remove_suffix(1);
			if (alias[alias.length() - 1] == preferred_seperator) alias.remove_suffix(1);

			if (!base.empty())
			{
				auto it_base = std::mismatch(std::begin(base), std::end(base)
											, std::begin(path), std::end(path)).first;
				if (it_base == std::end(base)) path.remove_prefix(base.length());
			}

			if (!alias.empty())
			{
				auto it_alias = std::mismatch(std::begin(alias), std::end(alias)
											, std::begin(path), std::end(path)).first;
				if (it_alias == std::end(alias)) return string{ path };
			}

			return string{}.insert(0, alias).append(path);
		}
		
		inline string alias_absolute(string_view path, string_view base, string_view alias)
		{

			if (base[base.length() - 1] == preferred_seperator) base.remove_suffix(1);
			if (alias[alias.length() - 1] == preferred_seperator) alias.remove_suffix(1);

			if (!alias.empty())
			{
				auto it_alias = std::mismatch(std::begin(alias), std::end(alias)
											, std::begin(path), std::end(path)).first;
				if (it_alias == std::end(alias)) path.remove_prefix(base.length());
			}

			if (!base.empty())
			{
				auto it_base = std::mismatch(std::begin(base), std::end(base)
											, std::begin(path), std::end(path)).first;
				if (it_base == std::end(base)) return string{ path };
			}

			return string{}.insert(0, base).append(path);
		}
	}

	namespace filesystem {
		using namespace path_util;
	}
}

#endif
