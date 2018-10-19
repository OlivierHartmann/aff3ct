#include "Tools/Exception/exception.hpp"

#include "Flip_and_check.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Flip_and_check_name   = "Flip and check";
const std::string aff3ct::factory::Flip_and_check_prefix = "fnc";

Flip_and_check::parameters
::parameters(const std::string &prefix)
: Factory::parameters(Flip_and_check_name, Flip_and_check_name, prefix)
{
}

Flip_and_check::parameters
::parameters(const std::string &name, const std::string &prefix)
: Factory::parameters(name, Flip_and_check_name, prefix)
{
}

Flip_and_check::parameters* Flip_and_check::parameters
::clone() const
{
	return new Flip_and_check::parameters(*this);
}

void Flip_and_check::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	CLI::add_option(app, p, naf,
		"--size",
		size,
		"Size (in bit) of the extrinsic for the fnc processing.")
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

	CLI::add_flag(app, p, naf,
		"--enable",
		enable,
		"Enables the flip and check decoder (requires \"crc --type\").")
		->group("Standard");

	CLI::add_option(app, p, naf,
		"-q",
		q,
		"Set the search's space for the fnc algorithm.",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--ite-m",
		ite_min,
		"Set first iteration at which the fnc is used.",
		true)
		->group("Standard");

	ite_M_option =
	CLI::add_option(app, p, naf,
		"--ite-M",
		ite_min,
		"Set last iteration at which the fnc is used.",
		true)
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--ite-s",
		ite_step,
		"Set iteration step for the fnc algorithm.",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_option(app, p, naf,
		"-i,--ite",
		ite_step,
		"Maximal number of iterations in the turbo.",
		true)
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--crc-ite",
		start_crc_check_ite,
		"Set the iteration to start the CRC checking.",
		true)
		->group("Standard");
}

void Flip_and_check::parameters
::callback_arguments()
{
	if (ite_M_option->empty())
		ite_max = n_ite;
}

void Flip_and_check::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_short_name();

	headers[p].push_back(std::make_pair("Enabled", ((this->enable) ? "yes" : "no")));
	if (this->enable)
	{
		headers[p].push_back(std::make_pair("FNC q",        std::to_string(this->q       )));
		headers[p].push_back(std::make_pair("FNC ite min",  std::to_string(this->ite_min )));
		headers[p].push_back(std::make_pair("FNC ite max",  std::to_string(this->ite_max )));
		headers[p].push_back(std::make_pair("FNC ite step", std::to_string(this->ite_step)));
	}
}

template<typename B, typename Q>
tools::Flip_and_check<B,Q>* Flip_and_check::parameters
::build(module::CRC<B> &crc) const
{
	if (!this->enable)
		throw tools::cannot_allocate(__FILE__, __LINE__, __func__);

	return new tools::Flip_and_check<B,Q>(this->size, this->n_ite, crc, this->start_crc_check_ite, this->q, this->ite_min, this->ite_max, this->ite_step, this->n_frames);
}

template<typename B, typename Q>
tools::Flip_and_check<B,Q>* Flip_and_check
::build(const parameters& params, module::CRC<B> &crc)
{
	return params.template build<B,Q>(crc);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::tools::Flip_and_check<B_8 ,Q_8 >* aff3ct::factory::Flip_and_check::parameters::build<B_8 ,Q_8 >(module::CRC<B_8 >&) const;
template aff3ct::tools::Flip_and_check<B_16,Q_16>* aff3ct::factory::Flip_and_check::parameters::build<B_16,Q_16>(module::CRC<B_16>&) const;
template aff3ct::tools::Flip_and_check<B_32,Q_32>* aff3ct::factory::Flip_and_check::parameters::build<B_32,Q_32>(module::CRC<B_32>&) const;
template aff3ct::tools::Flip_and_check<B_64,Q_64>* aff3ct::factory::Flip_and_check::parameters::build<B_64,Q_64>(module::CRC<B_64>&) const;
template aff3ct::tools::Flip_and_check<B_8 ,Q_8 >* aff3ct::factory::Flip_and_check::build<B_8 ,Q_8 >(const aff3ct::factory::Flip_and_check::parameters&, module::CRC<B_8 >&);
template aff3ct::tools::Flip_and_check<B_16,Q_16>* aff3ct::factory::Flip_and_check::build<B_16,Q_16>(const aff3ct::factory::Flip_and_check::parameters&, module::CRC<B_16>&);
template aff3ct::tools::Flip_and_check<B_32,Q_32>* aff3ct::factory::Flip_and_check::build<B_32,Q_32>(const aff3ct::factory::Flip_and_check::parameters&, module::CRC<B_32>&);
template aff3ct::tools::Flip_and_check<B_64,Q_64>* aff3ct::factory::Flip_and_check::build<B_64,Q_64>(const aff3ct::factory::Flip_and_check::parameters&, module::CRC<B_64>&);
#else
template aff3ct::tools::Flip_and_check<B,Q>* aff3ct::factory::Flip_and_check::parameters::build<B,Q>(module::CRC<B>&) const;
template aff3ct::tools::Flip_and_check<B,Q>* aff3ct::factory::Flip_and_check::build<B,Q>(const aff3ct::factory::Flip_and_check::parameters&, module::CRC<B>&);
#endif
// ==================================================================================== explicit template instantiation
