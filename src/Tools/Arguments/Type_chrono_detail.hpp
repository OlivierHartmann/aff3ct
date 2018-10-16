#ifndef TYPE_CHRONO_DETAIL_HPP
#define TYPE_CHRONO_DETAIL_HPP

#include <iostream>
#include <sstream>
#include <chrono>
#include <type_traits>

namespace CLI
{
	namespace detail
	{
		template <>
		constexpr const char *type_name<std::chrono::hours>()
		{
			return "TIME [H]";
		}

		template <>
		constexpr const char *type_name<std::chrono::minutes>()
		{
			return "TIME [MIN]";
		}

		template <>
		constexpr const char *type_name<std::chrono::seconds>()
		{
			return "TIME [SEC]";
		}

		template <>
		constexpr const char *type_name<std::chrono::milliseconds>()
		{
			return "TIME [MSEC]";
		}

		template <>
		constexpr const char *type_name<std::chrono::microseconds>()
		{
			return "TIME [USEC]";
		}

		template <>
		constexpr const char *type_name<std::chrono::nanoseconds>()
		{
			return "TIME [NSEC]";
		}
	}
}

#endif // TYPE_CHRONO_DETAIL_HPP
