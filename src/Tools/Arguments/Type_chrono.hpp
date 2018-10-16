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
		T v;
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
}

#endif // TYPE_CHRONO_HPP
