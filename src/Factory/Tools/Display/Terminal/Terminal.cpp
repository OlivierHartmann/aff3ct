#include "Terminal.hpp"

#include "Tools/Exception/exception.hpp"

#include "Tools/Display/Terminal/Standard/Terminal_std.hpp"


using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Terminal_name   = "Terminal";
const std::string aff3ct::factory::Terminal_prefix = "ter";

Terminal::parameters
::parameters(const std::string &prefix)
: Factory::parameters(Terminal_name, Terminal_name, prefix)
{
}

Terminal::parameters
::parameters(const std::string &name, const std::string &prefix)
: Factory::parameters(name, Terminal_name, prefix)
{
}

Terminal::parameters* Terminal::parameters
::clone() const
{
	return new Terminal::parameters(*this);
}

void Terminal::parameters
::register_arguments(CLI::App &app)
{
	auto sub = CLI::make_subcommand(app, get_prefix(), get_name() + " parameters");

	sub->add_set(
		"--type",
		type,
		{"STD"},
		"Type of the terminal to use to display results.",
		true)
		->group("Standard");

	sub->add_flag(
		"--no",
		disabled,
		"Disable recurrent report but only at the end of each noise point.")
		->group("Standard");

	sub->add_option(
		"--freq",
		frequency,
		"Display frequency (refresh time step for each iteration, 0 = disable display refresh).",
		true)
		->group("Standard");
}

void Terminal::parameters
::callback_arguments()
{
}

void Terminal::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = this->get_prefix();

	headers[p].push_back(std::make_pair("Enabled", this->disabled ? "no" : "yes"));
	headers[p].push_back(std::make_pair("Frequency (ms)", std::to_string(this->frequency.count())));
}


tools::Terminal* Terminal::parameters
::build(const std::vector<std::unique_ptr<tools::Reporter>> &reporters) const
{
	if (this->type == "STD") return new tools::Terminal_std(reporters);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

tools::Terminal* Terminal
::build(const parameters &params, const std::vector<std::unique_ptr<tools::Reporter>> &reporters)
{
	return params.build(reporters);
}