#include "Monitor_EXIT.hpp"

#include "Tools/Exception/exception.hpp"

#include "Module/Monitor/EXIT/Monitor_EXIT.hpp"


using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Monitor_EXIT_name   = "Monitor EXIT";
const std::string aff3ct::factory::Monitor_EXIT_prefix = "mnt";

Monitor_EXIT::parameters
::parameters(const std::string &prefix)
: Monitor::parameters(Monitor_EXIT_name, prefix)
{
}

Monitor_EXIT::parameters* Monitor_EXIT::parameters
::clone() const
{
	return new Monitor_EXIT::parameters(*this);
}

void Monitor_EXIT::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	Monitor::parameters::register_arguments(app);

	CLI::add_option(app, p, naf,
		"-K,--info-bits",
		size,
		"Number of bits to check.")
		->required()
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_option(app, p, naf,
		"-F,--fra",
		n_frames,
		"Set the number of inter frame level to process.",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_option(app, p, naf,
		"-n,--trials",
		n_trials,
		"Number of frames to simulate per sigma A value.",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");
}

void Monitor_EXIT::parameters
::callback_arguments()
{
	Monitor::parameters::callback_arguments();
}

void Monitor_EXIT::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_name();

	Monitor::parameters::get_headers(headers, full);

	headers[p].push_back(std::make_pair("Number of trials", std::to_string(this->n_trials)));
	if (full) headers[p].push_back(std::make_pair("Size (K)", std::to_string(this->size)));
	if (full) headers[p].push_back(std::make_pair("Inter frame level", std::to_string(this->n_frames)));
}

template <typename B, typename R>
module::Monitor_EXIT<B,R>* Monitor_EXIT::parameters
::build() const
{
	if (this->type == "STD") return new module::Monitor_EXIT<B,R>(this->size, this->n_trials, this->n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename R>
module::Monitor_EXIT<B,R>* Monitor_EXIT
::build(const parameters& params)
{
	return params.template build<B,R>();
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Monitor_EXIT<B_32,R_32>* aff3ct::factory::Monitor_EXIT::parameters::build<B_32,R_32>() const;
template aff3ct::module::Monitor_EXIT<B_64,R_64>* aff3ct::factory::Monitor_EXIT::parameters::build<B_64,R_64>() const;
template aff3ct::module::Monitor_EXIT<B_32,R_32>* aff3ct::factory::Monitor_EXIT::build<B_32,R_32>(const aff3ct::factory::Monitor_EXIT::parameters&);
template aff3ct::module::Monitor_EXIT<B_64,R_64>* aff3ct::factory::Monitor_EXIT::build<B_64,R_64>(const aff3ct::factory::Monitor_EXIT::parameters&);
#else
template aff3ct::module::Monitor_EXIT<B,R>* aff3ct::factory::Monitor_EXIT::parameters::build<B,R>() const;
template aff3ct::module::Monitor_EXIT<B,R>* aff3ct::factory::Monitor_EXIT::build<B,R>(const aff3ct::factory::Monitor_EXIT::parameters&);
#endif
// ==================================================================================== explicit template instantiation
