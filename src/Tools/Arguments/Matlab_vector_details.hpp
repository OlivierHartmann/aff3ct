#ifndef CLI_MATLAB_VECTOR_DETAILS_HPP
#define CLI_MATLAB_VECTOR_DETAILS_HPP

#include "Tools/String_splitter/String_splitter.hpp"
#include "Tools/general_utils.h"

namespace CLI
{
	namespace detail
	{
		constexpr const char* matvec_typename = "MATLAB VECTOR STYLE";

		template <>
		constexpr const char *type_name<Matlab_vector<int>>()
		{
			return matvec_typename;
		}

		template <>
		constexpr const char *type_name<Matlab_vector<float>>()
		{
			return matvec_typename;
		}
	}


	inline std::vector<std::string> split_D1(const std::string& val)
	{
		const std::string head      = "{([";
		const std::string queue     = "})]";
		const std::string separator = ",;";

		return aff3ct::tools::split(val, head, queue, separator);
	}

	inline std::vector<std::string> split_D2(const std::string& val)
	{
		const std::string head      = "";
		const std::string queue     = "";
		const std::string separator = ":";

		return aff3ct::tools::split(val, head, queue, separator);
	}

	template <typename T>
	Matlab_vector<T>& Matlab_vector<T>::operator=(const std::string& description)
	{
			*this = split_D1(description);

			return *this;
	}

	template <typename T>
	Matlab_vector<T>& Matlab_vector<T>::operator=(const std::vector<std::string>& description)
	{
		std::vector<Container> range_description(description.size());

		for (size_t i = 0; i < description.size(); i++)
		{
			auto vec_D2 = split_D2(description[i]);
			range_description[i].resize(vec_D2.size());

			for (size_t j = 0; j < vec_D2.size(); j++)
				detail::lexical_cast(vec_D2[j], range_description[i][j]);
		}

		vec_ = aff3ct::tools::generate_range(range_description);

		return *this;
	}


}

#endif // CLI_MATLAB_VECTOR_DETAILS_HPP