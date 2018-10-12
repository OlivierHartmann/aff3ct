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


bool CLI::has_option(CLI::App& app, const std::string& option)
{
	try
	{
		app.get_option(option);
		return true;
	}
	catch(const CLI::OptionNotFound&)
	{
		return false;
	}
}

bool CLI::has_option(CLI::App* app, const std::string& option)
{
	if (app == nullptr)
		return false;

	return CLI::has_option(*app, option);
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
	catch(const CLI::OptionNotFound&)
	{
		return false;
	}
}


void CLI::remove_option(CLI::App& app, const std::string& option)
{
	app.remove_option(app.get_option(option));
}

void CLI::remove_option(CLI::App* app, const std::string& option)
{
	if (app == nullptr)
		return;

	CLI::remove_option(*app, option);
}
