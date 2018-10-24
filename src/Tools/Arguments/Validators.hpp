#ifndef CLI_VALIDATORS_HPP
#define CLI_VALIDATORS_HPP

#include <string>
#include <chrono>
#include <vector>

#include <CLI/CLI.hpp>

#include "Tools/Math/utils.h"

namespace CLI
{

struct PositiveRange : public Validator
{
	/// This produces a strictly positive range with max inclusive limit (0 == infinite).
	template <typename T> explicit PositiveRange(T max)
	{
		std::stringstream out;
		if (max == (T)0)
		{
			out << detail::type_name<T>() << " in [0 - inf[";

			tname = out.str();
			func = [](const std::string &input) {
				auto val = (T)0;
				detail::lexical_cast(input, val);
				if (val < (T)0)
					return "Value " + input + " not a positive value";

				return std::string();
			};

		}
		else
		{
			out << detail::type_name<T>() << " in [0 - " << max << "]";

			tname = out.str();
			func = [max](const std::string &input) {
				auto val = (T)0;
				detail::lexical_cast(input, val);
				if (val < (T)0 || val > max)
					return "Value " + input + " not a positive value with maximum " + std::to_string(max) + ".";

				return std::string();
			};
		}
	}
};

struct StrictlyPositiveRange : public Validator
{
	/// This produces a strictly positive range with max inclusive limit (0 == infinite).
	template <typename T> explicit StrictlyPositiveRange(T max)
	{
		std::stringstream out;
		if (max == (T)0)
		{
			out << detail::type_name<T>() << " in ]0 - inf[";

			tname = out.str();
			func = [](const std::string &input) {
				auto val = (T)0;
				detail::lexical_cast(input, val);
				if (val <= (T)0)
					return "Value " + input + " not a strictly positive value";

				return std::string();
			};

		}
		else
		{
			out << detail::type_name<T>() << " in ]0 - " << max << "]";

			tname = out.str();
			func = [max](const std::string &input) {
				auto val = (T)0;
				detail::lexical_cast(input, val);
				if (val <= (T)0 || val > max)
					return "Value " + input + " not a strictly positive value with maximum " + std::to_string(max) + ".";

				return std::string();
			};
		}
	}
};

struct NotNullSymetricRange : public Validator
{
	/// This produces a range with non null value with max inclusive limit (0 == infinite).
	template <typename T> explicit NotNullSymetricRange(T max)
	{
		std::stringstream out;
		if (max == (T)0)
		{
			out << detail::type_name<T>() << " in ]inf - 0 [ U ]0 - inf[";

			tname = out.str();
			func = [](const std::string &input) {
				auto val = (T)0;
				detail::lexical_cast(input, val);
				if (val == (T)0)
					return "Value " + input + " is null.";

				return std::string();
			};

		}
		else
		{
			out << detail::type_name<T>() << " in [" << max << " - 0 [ U ]0 - " << max << "]";

			tname = out.str();
			func = [max](const std::string &input) {
				auto val = (T)0;
				detail::lexical_cast(input, val);
				if (val <= (T)0 || val > max)
					return "Value " + input + " is null and can have a maximum of +-" + std::to_string(max) + ".";

				return std::string();
			};
		}
	}
};



template <typename T>
struct Power_of_two_minus_one : public CLI::Validator
{
	Power_of_two_minus_one()
	{
		std::stringstream out;
		out << detail::type_name<T>() << " in {Powers of two minus one}";
		tname = out.str();

		func = [](const std::string &str)
		{
			auto val = (T)0;
			detail::lexical_cast(str, val);

			if(aff3ct::tools::is_power_of_2(val + (T)1))
				return std::string();
			else
				return std::string("Value is not a power of two minus one.");
		};
	}
};


struct Boolean : public CLI::Validator
{
	Boolean()
	{
		std::stringstream out;
		out << detail::type_name<bool>() << " in {true, 1, false, 0}";

		tname = out.str();
		func = [](const std::string &str)
		{
			auto val = false;

			if(detail::lexical_cast(str, val))
				return std::string();
			else
				return std::string("Value is not a boolean.");
		};
	}
};



/// Check for an existing file (returns error message if check fails)
struct OutputFile : public Validator {
	OutputFile() {
		tname = "FILE";
		func = [](const std::string &filename) {
			return std::string();
		};
	}
};



struct VectorLength : public CLI::Validator
{
	/// This produces a length check with a default null minimum and a maximum 'max' (0 == infinite).
	VectorLength(size_t max) : VectorLength(0, max)
	{
	}

	/// This produces a length check with minimum (0 == no min) and a maximum 'max' (0 == infinite).
	VectorLength(size_t min, size_t max)
	{
		std::stringstream out;

		if (min == 0 && max == 0)
		{
			out << "VECTOR without length limit.";
			tname = out.str();

			func = [](const std::string &str)
			{
				return std::string();
			};
		}
		else if (max == 0)
		{
			out << "VECTOR of a minimum length of " << min;
			tname = out.str();

			func = [min](const std::string &str)
			{
				const std::string head      = "{([";
				const std::string queue     = "})]";
				const std::string separator = ",;";

				auto str_vector = aff3ct::tools::split(str, head, queue, separator);

				if (str_vector.size() < min)
				{
					std::stringstream out;
					out << "Vector is too short : " << str_vector.size() << " instead of a minimum of " << min;
					return out.str();
				}
				else
					return std::string();
			};
		}
		else if (min == 0)
		{
			out << "VECTOR of a maximum length of " << max;
			tname = out.str();

			func = [max](const std::string &str)
			{
				const std::string head      = "{([";
				const std::string queue     = "})]";
				const std::string separator = ",;";

				auto str_vector = aff3ct::tools::split(str, head, queue, separator);

				if (str_vector.size() > max)
				{
					std::stringstream out;
					out << "Vector is too long : " << str_vector.size() << " instead of a maximum of " << max;
					return out.str();
				}
				else
					return std::string();
			};

		}
		else
		{
			out << "VECTOR of a length between " << min << " and " << max;
			tname = out.str();

			func = [min, max](const std::string &str)
			{
				const std::string head      = "{([";
				const std::string queue     = "})]";
				const std::string separator = ",;";

				auto str_vector = aff3ct::tools::split(str, head, queue, separator);

				if (str_vector.size() < min || str_vector.size() > max)
				{
					std::stringstream out;
					out << "Vector's size is not good : " << str_vector.size() << " instead of a length between "
					    << min << " and " << max;
					return out.str();
				}
				else
					return std::string();
			};

		}
	}
};

}

#endif // CLI_VALIDATORS_HPP
