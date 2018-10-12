#ifndef TYPE_CHRONO_HPP
#define TYPE_CHRONO_HPP

#include <iostream>
#include <sstream>
#include <chrono>
#include <type_traits>

namespace CLI
{
	template <typename T, typename R>
	std::istringstream &operator>>(std::istringstream &in, std::chrono::duration<T,R> &val)
	{
		long long v;
		in >> v;
		val = std::chrono::duration<T,R>(v);
		return in;
	}

	template <typename T, typename R>
	std::stringstream &operator<<(std::stringstream &in, std::chrono::duration<T,R> &val)
	{
		in << val.count();
		return in;
	}

	namespace detail
	{

		/// Check to see if something is a chrono duration (fail check by default)
		template <typename T> struct is_chrono { static const bool value = false; };

		/// Check to see if something is a chrono duration (true if actually a chrono duration)
		template <class T, class R> struct is_chrono<std::chrono::duration<T,R>> { static bool const value = true; };


		template <typename T, typename std::enable_if<is_chrono<T>::value && std::is_same<T,std::chrono::hours>::value, void*>::type = nullptr>
		constexpr const char *type_name()
		{
			return "TIME [H]";
		}

		template <typename T, typename std::enable_if<is_chrono<T>::value && std::is_same<T,std::chrono::minutes>::value, void*>::type = nullptr>
		constexpr const char *type_name()
		{
			return "TIME [MIN]";
		}

		template <typename T, typename std::enable_if<is_chrono<T>::value && std::is_same<T,std::chrono::seconds>::value, void*>::type = nullptr>
		constexpr const char *type_name()
		{
			return "TIME [SEC]";
		}

		template <typename T, typename std::enable_if<is_chrono<T>::value && std::is_same<T,std::chrono::milliseconds>::value, void*>::type = nullptr>
		constexpr const char *type_name()
		{
			return "TIME [MSEC]";
		}

		template <typename T, typename std::enable_if<is_chrono<T>::value && std::is_same<T,std::chrono::microseconds>::value, void*>::type = nullptr>
		constexpr const char *type_name()
		{
			return "TIME [USEC]";
		}

		template <typename T, typename std::enable_if<is_chrono<T>::value && std::is_same<T,std::chrono::nanoseconds>::value, void*>::type = nullptr>
		constexpr const char *type_name()
		{
			return "TIME [NSEC]";
		}
	}
}

#endif // TYPE_CHRONO_HPP
