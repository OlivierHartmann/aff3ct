#ifndef OPTION_FUNCTIONS_HXX__
#define OPTION_FUNCTIONS_HXX__

#include "Tools/Exception/exception.hpp"

namespace CLI
{
template <typename T>
CLI::Option* add_option(CLI::App& app, const std::string& prefix, bool no_argflag, const std::string& name,
                        T& variable, const std::string& description, bool defaulted)
{
	auto opt_name = CLI::add_prefix_to_name(name, prefix, no_argflag);

	try
	{
		auto opt = app.get_option(opt_name);
		return opt;
	}
	catch (const CLI::OptionNotFound&)
	{
		// then need to create the option
		return app.add_option(opt_name, variable, description, defaulted);
	}
}

template <typename T>
CLI::Option* add_option(CLI::App& app, const std::string& prefix, bool no_argflag, const std::string& name,
                        T& variable, const std::string& description)
{
	auto opt_name = CLI::add_prefix_to_name(name, prefix, no_argflag);

	try
	{
		auto opt = app.get_option(opt_name);
		return opt;
	}
	catch (const CLI::OptionNotFound&)
	{
		// then need to create the option
		return app.add_option(opt_name, variable, description);
	}
}


template <typename T>
CLI::Option* add_option(CLI::App& app, const std::string& prefix, bool no_argflag, const std::string& name,
                        std::vector<T>& variable, const std::string& description, bool defaulted)
{
	auto opt_name = CLI::add_prefix_to_name(name, prefix, no_argflag);

	try
	{
		auto opt = app.get_option(opt_name);
		return opt;
	}
	catch (const CLI::OptionNotFound&)
	{
		// then need to create the option

		CLI::callback_t fun = [&variable](CLI::results_t res)
		{
			const std::string head      = "{([";
			const std::string queue     = "})]";
			const std::string separator = ",;";

			auto str_vector = aff3ct::tools::split(res[0], head, queue, separator);

			bool retval = true;
			variable.clear();
			for(const auto &a : str_vector) {
				variable.emplace_back();
				retval &= detail::lexical_cast(a, variable.back());
			}

			return (!variable.empty()) && retval;
		};

		auto opt = app.add_option(opt_name, fun, description, defaulted);

		if(defaulted)
		{
			std::stringstream out;

			out << "{";
			for (unsigned i = 0; i < variable.size(); i++)
				out << variable[i] << (i + 1 == variable.size() ? "" : ",");
			out << "}";

			opt->default_str(out.str());
		}

		opt->type_name("AF VECTOR");

		return opt;
	}
}

template <typename T>
CLI::Option* add_option(CLI::App& app, const std::string& prefix, bool no_argflag, const std::string& name,
                        std::vector<T>& variable, const std::string& description)
{
	auto opt_name = CLI::add_prefix_to_name(name, prefix, no_argflag);

	try
	{
		auto opt = app.get_option(opt_name);
		return opt;
	}
	catch (const CLI::OptionNotFound&)
	{
		// then need to create the option

		CLI::callback_t fun = [&variable](CLI::results_t res)
		{
			const std::string head      = "{([";
			const std::string queue     = "})]";
			const std::string separator = ",;";

			auto str_vector = aff3ct::tools::split(res[0], head, queue, separator);

			bool retval = true;
			variable.clear();
			for(const auto &a : str_vector) {
				variable.emplace_back();
				retval &= detail::lexical_cast(a, variable.back());
			}

			return (!variable.empty()) && retval;
		};

		auto opt = app.add_option(opt_name, fun, description);
		opt->type_name("AF VECTOR");

		return opt;
	}
}



template <typename T>
CLI::Option* add_flag(CLI::App& app, const std::string& prefix, bool no_argflag, const std::string& name,
                      T& variable, const std::string& description)
{
	auto opt_name = CLI::add_prefix_to_name(name, prefix, no_argflag);

	try
	{
		auto opt = app.get_option(opt_name);
		return opt;
	}
	catch (const CLI::OptionNotFound&)
	{
		// then need to create the option
		return app.add_flag(opt_name, variable, description);
	}
}




template <typename T>
CLI::Option* add_set(CLI::App& app, const std::string& prefix, bool no_argflag, const std::string& name,
                     T& variable, const std::set<T> &&options, const std::string& description, bool defaulted)
{
	auto opt_name = CLI::add_prefix_to_name(name, prefix, no_argflag);

	try
	{
		auto opt = app.get_option(opt_name);
		return opt;
	}
	catch (const CLI::OptionNotFound&)
	{
		// then need to create the option
		return app.add_set(opt_name, variable, std::move(options), description, defaulted);
	}
}

template <typename T>
CLI::Option* add_set(CLI::App& app, const std::string& prefix, bool no_argflag, const std::string& name,
                     T& variable, const std::set<T> &options, const std::string& description, bool defaulted)
{
	auto opt_name = CLI::add_prefix_to_name(name, prefix, no_argflag);

	try
	{
		auto opt = app.get_option(opt_name);
		return opt;
	}
	catch (const CLI::OptionNotFound&)
	{
		// then need to create the option
		return app.add_set(opt_name, variable, options, description, defaulted);
	}
}

template <typename T>
CLI::Option* add_set(CLI::App& app, const std::string& prefix, bool no_argflag, const std::string& name,
                     T& variable, const std::set<T> &&options, const std::string& description)
{
	auto opt_name = CLI::add_prefix_to_name(name, prefix, no_argflag);

	try
	{
		auto opt = app.get_option(opt_name);
		return opt;
	}
	catch (const CLI::OptionNotFound&)
	{
		// then need to create the option
		return app.add_set(opt_name, variable, std::move(options), description);
	}
}

template <typename T>
CLI::Option* add_set(CLI::App& app, const std::string& prefix, bool no_argflag, const std::string& name,
                     T& variable, const std::set<T> &options, const std::string& description)
{
	auto opt_name = CLI::add_prefix_to_name(name, prefix, no_argflag);

	try
	{
		auto opt = app.get_option(opt_name);
		return opt;
	}
	catch (const CLI::OptionNotFound&)
	{
		// then need to create the option
		return app.add_set(opt_name, variable, options, description);
	}
}

}

#endif // OPTION_FUNCTIONS_HXX__
