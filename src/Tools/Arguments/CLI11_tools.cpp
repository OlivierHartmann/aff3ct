#include <sstream>
#include "Tools/Exception/exception.hpp"

#include "CLI11_tools.hpp"

using namespace CLI;


CLI::App* CLI::make_subcommand(CLI::App& app, const std::string& command, const std::string& description)
{
	try
	{
		return app.get_subcommand(command);
	}
	catch (const CLI::OptionNotFound&)
	{
		return app.add_subcommand(command, description);
	}
}


bool CLI::has_subcommand(CLI::App* app, const std::string& subcommand)
{
	if (app == nullptr)
		return false;

	return CLI::has_subcommand(*app, subcommand);
}

bool CLI::has_subcommand(CLI::App& app, const std::string& subcommand)
{
	try
	{
		app.get_subcommand(subcommand);
		return true;
	}
	catch (const CLI::OptionNotFound&)
	{
		return false;
	}
}


bool CLI::has_option(CLI::App& app, const std::string& name, const std::string& prefix)
{
	try
	{
		app.get_option(CLI::add_prefix_to_name(name, prefix));
		return true;
	}
	catch (const CLI::OptionNotFound&)
	{
		return false;
	}
}

bool CLI::has_option(CLI::App* app, const std::string& name, const std::string& prefix)
{
	if (app == nullptr)
		return false;

	return CLI::has_option(*app, name, prefix);
}


CLI::Option* CLI::get_option(CLI::App& app, const std::string& name, const std::string& prefix)
{
	return app.get_option(CLI::add_prefix_to_name(name, prefix));
}

CLI::Option* CLI::get_option(CLI::App* app, const std::string& name, const std::string& prefix)
{
	return CLI::get_option(*app, name, prefix);
}



void CLI::remove_option(CLI::App& app, const std::string& name, const std::string& prefix)
{
	app.remove_option(app.get_option(CLI::add_prefix_to_name(name, prefix)));
}

void CLI::remove_option(CLI::App* app, const std::string& name, const std::string& prefix)
{
	if (app == nullptr)
		return;

	CLI::remove_option(*app, name, prefix);
}


std::string CLI::add_prefix_to_name(const std::string& name, const std::string& prefix)
{
	if (prefix.empty())
		return name;

	std::vector<std::string> snames, lnames;
	std::string pname;
	std::tie(snames, lnames, pname) = detail::get_names(detail::split_names(name));

	if (lnames.empty())
		return name;

	// then reconstruct the name with prefix on long names
	std::string new_name;
	for (auto it = snames.begin(); it != snames.end(); it++)
	{
		if (!new_name.empty())
			new_name += ",";
		new_name += "-" + *it;
	}

	for (auto it = lnames.begin(); it != lnames.end(); it++)
	{
		if (!new_name.empty())
			new_name += ",";
		new_name += "--" + prefix + "-" + *it;
	}

	if (!pname.empty())
	{
		if (!new_name.empty())
			new_name += ",";
		new_name += pname;
	}

	return new_name;
}
