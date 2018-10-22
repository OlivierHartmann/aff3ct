#include "Tools/Exception/exception.hpp"

#include "Module/Monitor/BFER/Monitor_BFER.hpp"

#include "Monitor_BFER.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Monitor_BFER_name   = "Monitor BER/FER";
const std::string aff3ct::factory::Monitor_BFER_prefix = "mnt";

Monitor_BFER::parameters
::parameters(const std::string &prefix)
: Monitor::parameters(Monitor_name, prefix)
{
}

Monitor_BFER::parameters* Monitor_BFER::parameters
::clone() const
{
	return new Monitor_BFER::parameters(*this);
}

void Monitor_BFER::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	Monitor::parameters::register_arguments(app);

	CLI::add_option(app, p, naf,
		"-K,--info-bits",
		K,
		"Number of bits to check.")
		->required()
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");


	if (!CLI::has_option(app, "--fra", p, naf))
	CLI::add_option(app, p, naf,
		"-F,--fra",
		n_frames,
		"Set the number of inter frame level to process.",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_option(app, p, naf,
		"-e,--max-fe",
		n_frame_errors,
		"Max number of frame errors for each noise point simulation.",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--max-frame",
		max_frame,
		"Maximum number of frames for each noise point simulation (0 means no limit).",
		true)
		->check(CLI::PositiveRange(0))
		->group("Advanced");

	CLI::add_option(app, p, naf,
		"--err-hist",
		err_hist,
		"Activate the histogram of the number of errors per frame. Set the max number of bit error per frame"
		" included in the histogram (0 is no limit).")
		->check(CLI::PositiveRange(0))
		->group("Advanced");

	CLI::add_option(app, p, naf,
		"--err-hist-path",
		err_hist_path,
		"Path to the output histogram (add automatically the current noise value and the extension '.txt').",
		true)
		->group("Standard");
}

void Monitor_BFER::parameters
::callback_arguments()
{
	Monitor::parameters::callback_arguments();
}

void Monitor_BFER::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_name();

	Monitor::parameters::get_headers(headers, full);

	headers[p].push_back(std::make_pair("Frame error count (e)", std::to_string(n_frame_errors)));
	if (full) headers[p].push_back(std::make_pair("Size (K)",          std::to_string(K       )));
	if (full) headers[p].push_back(std::make_pair("Inter frame level", std::to_string(n_frames)));

	if (err_hist != (unsigned)-1)
		headers[p].push_back(std::make_pair("Error histogram path", err_hist_path));
}

template <typename B>
module::Monitor_BFER<B>* Monitor_BFER::parameters
::build(bool count_unknown_values) const
{
	if (type == "STD") return new module::Monitor_BFER<B>(K, n_frame_errors, max_frame, count_unknown_values, n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B>
module::Monitor_BFER<B>* Monitor_BFER
::build(const parameters& params, bool count_unknown_values)
{
	return params.template build<B>(count_unknown_values);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Monitor_BFER<B_8 >* aff3ct::factory::Monitor_BFER::parameters::build<B_8 >(bool) const;
template aff3ct::module::Monitor_BFER<B_16>* aff3ct::factory::Monitor_BFER::parameters::build<B_16>(bool) const;
template aff3ct::module::Monitor_BFER<B_32>* aff3ct::factory::Monitor_BFER::parameters::build<B_32>(bool) const;
template aff3ct::module::Monitor_BFER<B_64>* aff3ct::factory::Monitor_BFER::parameters::build<B_64>(bool) const;
template aff3ct::module::Monitor_BFER<B_8 >* aff3ct::factory::Monitor_BFER::build<B_8 >(const aff3ct::factory::Monitor_BFER::parameters&, bool);
template aff3ct::module::Monitor_BFER<B_16>* aff3ct::factory::Monitor_BFER::build<B_16>(const aff3ct::factory::Monitor_BFER::parameters&, bool);
template aff3ct::module::Monitor_BFER<B_32>* aff3ct::factory::Monitor_BFER::build<B_32>(const aff3ct::factory::Monitor_BFER::parameters&, bool);
template aff3ct::module::Monitor_BFER<B_64>* aff3ct::factory::Monitor_BFER::build<B_64>(const aff3ct::factory::Monitor_BFER::parameters&, bool);
#else
template aff3ct::module::Monitor_BFER<B>* aff3ct::factory::Monitor_BFER::parameters::build<B>(bool) const;
template aff3ct::module::Monitor_BFER<B>* aff3ct::factory::Monitor_BFER::build<B>(const aff3ct::factory::Monitor_BFER::parameters&, bool);
#endif