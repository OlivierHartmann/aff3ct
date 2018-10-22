#include <algorithm>
#include <iostream>
#include <utility>
#include <sstream>
#include <vector>
#include <map>

#include "Tools/Display/rang_format/rang_format.h"
#include "Tools/general_utils.h"
#include "Tools/Exception/exception.hpp"
#include "Tools/Arguments/Help_formatter.hpp"

#include "Factory.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Factory_name       = "Factory";
const std::string aff3ct::factory::Factory_short_name = "Factory";
const std::string aff3ct::factory::Factory_prefix     = "fac";

Factory::parameters
::parameters(const std::string &name, const std::string &short_name, const std::string &prefix)
: name(name), short_name(short_name), prefix(prefix)
{
}

std::string Factory::parameters
::get_name() const
{
	return this->name;
}
std::string Factory::parameters
::get_short_name() const
{
	return this->short_name;
}

std::string Factory::parameters
::get_prefix() const
{
	return this->prefix;
}

std::vector<std::string> Factory::parameters
::get_names() const
{
	std::vector<std::string> n;
	n.push_back(this->name);
	return n;
}
std::vector<std::string> Factory::parameters
::get_short_names() const
{
	std::vector<std::string> sn;
	sn.push_back(this->short_name);
	return sn;
}

std::vector<std::string> Factory::parameters
::get_prefixes() const
{
	std::vector<std::string> p;
	p.push_back(this->prefix);
	return p;
}

void Factory::parameters
::no_argflag(bool v)
{
	no_argflag_ = v;
}

bool Factory::parameters
::no_argflag() const
{
	return no_argflag_;
}

void Factory
::register_arguments(const std::vector<Factory::parameters*> &params, CLI::App &app)
{
	for (auto *p : params)
		p->register_arguments(app);
}

void Factory
::callback_arguments(const std::vector<Factory::parameters*> &params)
{
	for (auto *p : params)
		p->callback_arguments();
}

tools::Argument_map_group Factory
::create_groups(const std::vector<Factory::parameters*> &params)
{
	// create groups of arguments
	tools::Argument_map_group grps;
	for (auto *p : params)
	{
		auto prefixes    = p->get_prefixes   ();
		auto short_names = p->get_short_names();

		if (prefixes.size() != short_names.size())
		{
			std::stringstream message;
			message << "'prefixes.size()' has to be equal to 'short_names.size()' ('prefixes.size()' = "
			        << prefixes.size() << ", 'short_names.size()' = " << short_names.size() << ").";
			throw tools::runtime_error(__FILE__, __LINE__, __func__, message.str());
		}

		for (size_t i = 0; i < prefixes.size(); i++)
			grps[prefixes[i]] = short_names[i] + " parameter(s)";
	}

	return grps;
}

void aff3ct::factory::Header
::print_parameters(bool grp_key, const std::string& grp_name, const header_list& header, int max_n_chars,
                   std::ostream& stream)
{
	if (grp_key)
	{
		stream << rang::tag::comment << "* " << rang::style::bold << rang::style::underline << grp_name << rang::style::reset << " ";
		for (auto i = 0; i < 46 - (int)grp_name.size(); i++) std::cout << "-";
		stream << std::endl;
	}
	else // sub group
	{
		stream << rang::tag::comment << "   " << rang::style::bold << rang::style::underline << grp_name << rang::style::reset << " ";
		for (auto i = 0; i < 45 - (int)grp_name.size(); i++) std::cout << "-";
		stream << std::endl;
	}

	std::vector<std::string> dup;
	for (auto i = 0; i < (int)header.size(); i++)
	{
		auto h_fs = header[i].first + header[i].second;
		if (std::find(dup.begin(), dup.end(), h_fs) == dup.end())
		{
			stream << rang::tag::comment << "   ** " << rang::style::bold << header[i].first << rang::style::reset;
			for (auto j = 0; j < max_n_chars - (int)header[i].first.size(); j++) stream << " ";
			stream << " = " << header[i].second << std::endl;

			dup.push_back(h_fs);
		}
	}
}

void aff3ct::factory::Header
::print_parameters(const std::vector<Factory::parameters*> &params, const bool full, std::ostream& stream)
{
	// find first the longest header to display it properly
	int max_n_chars = 0;
	for (auto *p : params)
	{
		std::map<std::string,aff3ct::factory::header_list> headers;
		p->get_headers(headers, full);

		for (auto &h : headers)
			if (full || (h.second.size() && (h.second[0].first != "Type" || h.second[0].second != "NO")))
				aff3ct::factory::Header::compute_max_n_chars(h.second, max_n_chars);
	}


	// then display the header
	std::vector<aff3ct::factory::header_list> dup_h;
	std::vector<std::string                 > dup_n;
	for (auto *p : params)
	{
		std::map<std::string,aff3ct::factory::header_list> headers;

		p->get_headers(headers, full);

		auto prefixes    = p->get_prefixes   ();
		auto names       = p->get_names      ();
		auto short_names = p->get_short_names();

		if (prefixes.size() != names.size() && prefixes.size() != short_names.size())
		{
			std::stringstream message;
			message << "'prefixes.size()' has to be equal to 'names.size()' and 'short_names.size()' ('prefixes.size()' = "
			        << prefixes.size() << ", 'names.size()' = " << names.size() << " and 'short_names.size()' = " << short_names.size() << ").";
			throw tools::runtime_error(__FILE__, __LINE__, __func__, message.str());
		}

		// bool print_first_title = false;
		// for (size_t i = 1; i < short_names.size(); i++)
		// {
		// 	auto& h = headers[short_names[i]];
		// 	auto key = tools::split(prefixes[i], '-');

		// 	if (key[0] == prefixes[0] && h.size())
		// 	{
		// 		print_first_title = true;
		// 		break;
		// 	}
		// }



		// check if there is something to display
		bool print_first_title = false;
		for (auto &h : headers)
			if (h.second.size())
			{
				print_first_title = true;
				break;
			}

		for (size_t i = 0; i < names.size(); i++)
		{
			auto& sn = short_names[i];
			auto& n  = names      [i];
			auto& h  = headers    [n];
			auto print_head = (i == 0) ? print_first_title || h.size() : h.size();

			if (full || (h.size() && (h[0].first != "Type" || h[0].second != "NO")))
			{
				if (print_head && (std::find(dup_h.begin(), dup_h.end(), h) == dup_h.end() ||
				                   std::find(dup_n.begin(), dup_n.end(), n) == dup_n.end()))
				{
					auto is_top_module = prefixes[i].empty();
					auto name = sn;
					if (!is_top_module)
						name = n;

					Header::print_parameters(is_top_module, name, h, max_n_chars, stream);

					dup_h.push_back(h);
					dup_n.push_back(n);
				}
			}
		}
	}
}

void aff3ct::factory::Header
::compute_max_n_chars(const header_list& header, int& max_n_chars)
{
	for (unsigned i = 0; i < header.size(); i++)
		max_n_chars = std::max(max_n_chars, (int)header[i].first.length());
}


std::unique_ptr<CLI::App> aff3ct::factory::Factory
::make_argument_handler()
{
	std::unique_ptr<CLI::App> app(new CLI::App{"A Fast Forward Error Correction Toolbox!"});

	app->require_subcommand(0,0); // require at least one subcommand
	app->fallthrough(true); // allow main argument (as help or version flags) to be given any where in the command
	app->allow_extras(); // allow extra arguments for multiple subcommands


	app->formatter(std::make_shared<CLI::Help_formatter>());
	app->get_formatter()->label("REQUIRED", "{R}");

	// remove help flags because they shorcut everything
	app->set_help_flag();
	app->set_help_all_flag();

	return app;
}
