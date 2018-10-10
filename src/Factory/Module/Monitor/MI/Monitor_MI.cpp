#include "Tools/Exception/exception.hpp"

#include "Module/Monitor/MI/Monitor_MI.hpp"

#include "Monitor_MI.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Monitor_MI_name   = "Monitor MI";
const std::string aff3ct::factory::Monitor_MI_prefix = "mnt";

Monitor_MI::parameters
::parameters(const std::string &prefix)
: Monitor::parameters(Monitor_name, prefix)
{
}

Monitor_MI::parameters* Monitor_MI::parameters
::clone() const
{
	return new Monitor_MI::parameters(*this);
}

void Monitor_MI::parameters
::register_arguments(CLI::App &app)
{
	Monitor::parameters::register_arguments(app);

	auto sub = CLI::make_subcommand(app, get_prefix(), get_name() + " parameters");

	sub->add_option(
		"-N,--fra-size",
		N,
		"The frame size for the mutual information computation.")
		->required()
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	if (!CLI::has_option(sub, "--fra"))
	sub->add_option(
		"-F,--fra",
		n_frames,
		"Set the number of inter frame level to process.",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	sub->add_option(
		"--trials",
		n_trials,
		"Number of frames to simulate per noise point.",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");
}

void Monitor_MI::parameters
::callback_arguments()
{
	Monitor::parameters::callback_arguments();
}

void Monitor_MI::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	Monitor::parameters::get_headers(headers, full);

	auto p = this->get_prefix();

	headers[p].push_back(std::make_pair("Number of trials (n)",        std::to_string(this->n_trials)));
	if (full) headers[p].push_back(std::make_pair("Size (N)",          std::to_string(this->N       )));
	if (full) headers[p].push_back(std::make_pair("Inter frame level", std::to_string(this->n_frames)));
}

template <typename B, typename R>
module::Monitor_MI<B,R>* Monitor_MI::parameters
::build() const
{
	if (this->type == "STD") return new module::Monitor_MI<B,R>(this->N, this->n_trials, this->n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename R>
module::Monitor_MI<B,R>* Monitor_MI
::build(const parameters& params)
{
	return params.template build<B,R>();
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Monitor_MI<B_8 ,R_8 >* aff3ct::factory::Monitor_MI::parameters::build<B_8, R_8 >() const;
template aff3ct::module::Monitor_MI<B_16,R_16>* aff3ct::factory::Monitor_MI::parameters::build<B_16,R_16>() const;
template aff3ct::module::Monitor_MI<B_32,R_32>* aff3ct::factory::Monitor_MI::parameters::build<B_32,R_32>() const;
template aff3ct::module::Monitor_MI<B_64,R_64>* aff3ct::factory::Monitor_MI::parameters::build<B_64,R_64>() const;
template aff3ct::module::Monitor_MI<B_8 ,R_8 >* aff3ct::factory::Monitor_MI::build<B_8, R_8 >(const aff3ct::factory::Monitor_MI::parameters&);
template aff3ct::module::Monitor_MI<B_16,R_16>* aff3ct::factory::Monitor_MI::build<B_16,R_16>(const aff3ct::factory::Monitor_MI::parameters&);
template aff3ct::module::Monitor_MI<B_32,R_32>* aff3ct::factory::Monitor_MI::build<B_32,R_32>(const aff3ct::factory::Monitor_MI::parameters&);
template aff3ct::module::Monitor_MI<B_64,R_64>* aff3ct::factory::Monitor_MI::build<B_64,R_64>(const aff3ct::factory::Monitor_MI::parameters&);
#else
template aff3ct::module::Monitor_MI<B,R>* aff3ct::factory::Monitor_MI::parameters::build<B,R>() const;
template aff3ct::module::Monitor_MI<B,R>* aff3ct::factory::Monitor_MI::build<B,R>(const aff3ct::factory::Monitor_MI::parameters&);
#endif
// ==================================================================================== explicit template instantiation
