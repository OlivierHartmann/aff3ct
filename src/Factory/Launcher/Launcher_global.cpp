#include <sstream>
#include <date.h>
#include <rang.hpp>

#include "Tools/general_utils.h"
#include "Tools/Exception/exception.hpp"
#include "Tools/version.h"

#include "Launcher_global.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Launcher_global_name   = "Launcher global";
const std::string aff3ct::factory::Launcher_global_prefix = "lch";

factory::Launcher_global::parameters
::parameters(const std::string &prefix, const std::string &name)
: Launcher_global::parameters(name, name, prefix)
{
}

factory::Launcher_global::parameters
::parameters(const std::string &name, const std::string &short_name, const std::string &prefix)
: Factory::parameters(name, short_name, prefix)
{
}

factory::Launcher_global::parameters* factory::Launcher_global::parameters
::clone() const
{
	return new Launcher_global::parameters(*this);
}

void factory::Launcher_global::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	if (!CLI::has_option(app, "--help", p, naf))
	CLI::add_flag(app, p, naf,
		"-h,--help",
		help,
		"Print the help.")
		->group("Standard");

	if (!CLI::has_option(app, "--Help", p, naf))
	CLI::add_flag(app, p, naf,
		"-H,--Help",
		advanced_help,
		"Print this help with the advanced arguments.")
		->group("Standard");

	CLI::add_flag(app, p, naf,
		"-v,--version",
		display_version,
		"Print informations about the code version.")
		->group("Standard");

#ifdef ENABLE_BACK_TRACE
	CLI::add_flag(app, p, naf,
		"--disable-bt",
		disable_bt,
		"Disable the backtrace when displaying exception.")
		->group("Advanced");

#ifndef NDEBUG
	CLI::add_flag(app, p, naf,
		"--full-bt",
		enable_full_bt,
		"Enable full backtrace with file names and lines (may take additional time).")
		->group("Advanced");
#endif
#endif

	auto no_legend_option =
	CLI::add_flag(app, p, naf,
		"--no-legend",
		hide_legend,
		"Do not display any legend when launching the simulation.")
		->group("Advanced");

	CLI::add_flag(app, p, naf,
		"--full-legend",
		full_legend,
		"Display fully the legend when launching the simulation.")
		->excludes(no_legend_option)
		->group("Advanced");

#ifdef ENABLE_COOL_BASH
	CLI::add_flag(app, p, naf,
		"--no-colors",
		disable_colors,
		"Disable the colors in the shell.")
		->group("Advanced");
#endif
}

void factory::Launcher_global::parameters
::callback_arguments()
{
	tools::exception::no_backtrace    =  disable_bt;
	tools::exception::no_addr_to_line = !enable_full_bt;

#ifdef ENABLE_COOL_BASH
	if (disable_colors)
		rang::setControlMode(rang::control::Off);
#else
	rang::setControlMode(rang::control::Off);
#endif
}

void factory::Launcher_global::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_short_name();

	using namespace date;
	std::stringstream date;
	date << std::chrono::system_clock::now();
	auto split_date = tools::split(date.str(), '.');
	headers[p].push_back(std::make_pair("Date (UTC)", split_date[0]));
	if (version() != "GIT-NOTFOUND")
		headers[p].push_back(std::make_pair("Git version", version()));
}
