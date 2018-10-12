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
				T val;
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
				T val;
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
					return "Value " + input + " not a stritly positive value";

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
		tname = "Power of two minus one";
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

}

#endif // CLI_VALIDATORS_HPP
