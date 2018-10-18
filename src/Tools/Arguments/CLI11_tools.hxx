#ifndef CLI11_TOOLS_HXX__
#define CLI11_TOOLS_HXX__

#include "Tools/Exception/exception.hpp"

namespace CLI
{
template <typename T>
CLI::Option* add_option(CLI::App& app, const std::string& prefix, const std::string& name,
                        T& variable, const std::string& description, bool defaulted)
{
	auto opt_name = CLI::add_prefix_to_name(name, prefix);

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
CLI::Option* add_option(CLI::App& app, const std::string& prefix, const std::string& name,
                        T& variable, const std::string& description)
{
	auto opt_name = CLI::add_prefix_to_name(name, prefix);

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
CLI::Option* add_flag(CLI::App& app, const std::string& prefix, const std::string& name,
                      T& variable, const std::string& description)
{
	auto opt_name = CLI::add_prefix_to_name(name, prefix);

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
CLI::Option* add_set(CLI::App& app, const std::string& prefix, const std::string& name,
                     T& variable, const std::set<T> &&options, const std::string& description, bool defaulted)
{
	auto opt_name = CLI::add_prefix_to_name(name, prefix);

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
CLI::Option* add_set(CLI::App& app, const std::string& prefix, const std::string& name,
                     T& variable, const std::set<T> &options, const std::string& description, bool defaulted)
{
	auto opt_name = CLI::add_prefix_to_name(name, prefix);

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
CLI::Option* add_set(CLI::App& app, const std::string& prefix, const std::string& name,
                     T& variable, const std::set<T> &&options, const std::string& description)
{
	auto opt_name = CLI::add_prefix_to_name(name, prefix);

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
CLI::Option* add_set(CLI::App& app, const std::string& prefix, const std::string& name,
                     T& variable, const std::set<T> &options, const std::string& description)
{
	auto opt_name = CLI::add_prefix_to_name(name, prefix);

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

#endif // CLI11_TOOLS_HXX__
