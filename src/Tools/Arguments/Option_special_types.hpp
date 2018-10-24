#ifndef OPTION_SPECIAL_TYPES_HPP__
#define OPTION_SPECIAL_TYPES_HPP__

#include <cstdint>
#include <sstream>
#include <iostream>
#include <type_traits>

namespace CLI
{
	// /************************************************************ BOOLEAN INT */
	// struct boolean
	// {
	// 	bool value;
	// };

	// inline std::istringstream& operator>>(std::istringstream &in, boolean &val)
	// {
	// 	int8_t v;
	// 	in >> v;
	// 	if (v != 1 && v != 0)
	// 		in.str("error"); // having still text in 'in' is considered as an error by CLI11
	// 	else
	// 		val.value = v;
	// 	return in;
	// }

	// inline std::stringstream& operator<<(std::stringstream &in, boolean &val)
	// {
	// 	in << val.value;
	// 	return in;
	// }
}

#endif // OPTION_SPECIAL_TYPES_HPP__