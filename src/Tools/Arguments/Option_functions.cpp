#include <sstream>
#include "Tools/Exception/exception.hpp"
#include "Tools/String_splitter/String_splitter.hpp"

#include "Option_functions.hpp"

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


bool CLI::has_option(CLI::App& app, const std::string& name, const std::string& prefix, bool no_argflag)
{
	try
	{
		app.get_option(CLI::add_prefix_to_name(name, prefix, no_argflag));
		return true;
	}
	catch (const CLI::OptionNotFound&)
	{
		return false;
	}
}

bool CLI::has_option(CLI::App* app, const std::string& name, const std::string& prefix, bool no_argflag)
{
	if (app == nullptr)
		return false;

	return CLI::has_option(*app, name, prefix, no_argflag);
}


CLI::Option* CLI::get_option(CLI::App& app, const std::string& name, const std::string& prefix, bool no_argflag)
{
	return app.get_option(CLI::add_prefix_to_name(name, prefix, no_argflag));
}

CLI::Option* CLI::get_option(CLI::App* app, const std::string& name, const std::string& prefix, bool no_argflag)
{
	return CLI::get_option(*app, name, prefix, no_argflag);
}



void CLI::remove_option(CLI::App& app, const std::string& name, const std::string& prefix, bool no_argflag)
{
	app.remove_option(app.get_option(CLI::add_prefix_to_name(name, prefix, no_argflag)));
}

void CLI::remove_option(CLI::App* app, const std::string& name, const std::string& prefix, bool no_argflag)
{
	if (app == nullptr)
		return;

	CLI::remove_option(*app, name, prefix, no_argflag);
}


std::string CLI::add_prefix_to_name(const std::string& name, const std::string& prefix, bool no_argflag)
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

	if (!no_argflag || (lnames.empty() && pname.empty()))
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


CLI::Option* CLI::add_option(CLI::App& app, const std::string& prefix, bool no_argflag, const std::string& name,
                             std::vector<std::vector<bool>>& pattern, const std::string& description, bool defaulted)
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

		CLI::callback_t fun = [&pattern](CLI::results_t res) {

			const std::string head      = "{([";
			const std::string queue     = "})]";
			const std::string separator = ",;";

			auto str_pattern = aff3ct::tools::split(res[0], head, queue, separator);
			pattern.resize(str_pattern.size());

			for (unsigned i = 0; i < str_pattern.size(); i++)
			{
				pattern[i].resize(str_pattern[i].size());

				for (unsigned j = 0; j < str_pattern[i].size(); j++)
					if (str_pattern[i][j] != '1' && str_pattern[i][j] != '0')
						return false;
					else
						pattern[i][j] = str_pattern[i][j] == '1';
			}

			return true;
		};

		auto opt = app.add_option(opt_name, fun, description, defaulted);

		if(defaulted)
		{
			std::stringstream out;

			out << "{";
			for (unsigned i = 0; i < pattern.size(); i++)
			{
				for (unsigned j = 0; j < pattern[i].size(); j++)
					out << pattern[i][j];

				out << (i + 1 == pattern.size() ? "" : ",");
			}
			out << "}";

			opt->default_str(out.str());
		}

		opt->type_name("BOOLEAN PATTERN");

		return opt;
	}
}

CLI::Option* CLI::add_option(CLI::App& app, const std::string& prefix, bool no_argflag, const std::string& name,
                             std::vector<std::vector<bool>>& pattern, const std::string& description)
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

		CLI::callback_t fun = [&pattern](CLI::results_t res) {

			const std::string head      = "{([";
			const std::string queue     = "})]";
			const std::string separator = ",;";

			auto str_pattern = aff3ct::tools::split(res[0], head, queue, separator);
			pattern.resize(str_pattern.size());

			for (unsigned i = 0; i < str_pattern.size(); i++)
			{
				pattern[i].resize(str_pattern[i].size());

				for (unsigned j = 0; j < str_pattern[i].size(); j++)
					if (str_pattern[i][j] != '1' && str_pattern[i][j] != '0')
						return false;
					else
						pattern[i][j] = str_pattern[i][j] == '1';
			}

			return true;
		};

		auto opt = app.add_option(opt_name, fun, description);
		opt->type_name("BOOLEAN PATTERN");

		return opt;
	}
}
