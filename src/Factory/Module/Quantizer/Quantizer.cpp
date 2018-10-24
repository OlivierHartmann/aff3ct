#include <type_traits>

#include "Tools/Exception/exception.hpp"

#include "Module/Quantizer/Pow2/Quantizer_pow2.hpp"
#include "Module/Quantizer/Pow2/Quantizer_pow2_fast.hpp"
#include "Module/Quantizer/Custom/Quantizer_custom.hpp"
#include "Module/Quantizer/NO/Quantizer_NO.hpp"

#include "Quantizer.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Quantizer_name   = "Quantizer";
const std::string aff3ct::factory::Quantizer_prefix = "qnt";

Quantizer::parameters
::parameters(const std::string &prefix)
: Factory::parameters(Quantizer_name, Quantizer_name, prefix)
{
}

Quantizer::parameters* Quantizer::parameters
::clone() const
{
	return new Quantizer::parameters(*this);
}

void Quantizer::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	CLI::add_option(app, p, naf,
		"-N,--size",
		size,
		"Number of real to quantize.")
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

	CLI::add_set(app, p, naf,
		"--type",
		type,
		{"POW2", "CUSTOM"},
		"Type of the quantizer to use in the simulation.",
		true)
		->group("Standard");

	CLI::add_set(app, p, naf,
		"--implem",
		implem,
		{"STD", "FAST"},
		"Select the implementation of quantizer.",
		true)
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--dec",
		n_decimals,
		"The position of the fixed point in the quantified representation.",
		true)
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--bits",
		n_bits,
		"The number of bits used for the quantizer.",
		true)
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--range",
		range,
		"The min/max bound for the tricky quantizer.",
		true)
		->check(CLI::NotNullSymetricRange(0))
		->group("Standard");
}

void Quantizer::parameters
::callback_arguments()
{
}

void Quantizer::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_name();

	std::string quantif = "unused";
	if (type == "CUSTOM")
		quantif = "{" + std::to_string(n_bits) + ", " + std::to_string(range)+"f}";
	else if (type == "POW2")
		quantif = "{" + std::to_string(n_bits) + ", " + std::to_string(n_decimals)+"}";

	headers[p].push_back(std::make_pair("Type", type));
	headers[p].push_back(std::make_pair("Implementation", implem));
	if (full) headers[p].push_back(std::make_pair("Frame size (N)", std::to_string(size)));
	if (full) headers[p].push_back(std::make_pair("Inter frame level", std::to_string(n_frames)));
	headers[p].push_back(std::make_pair("Fixed-point config.", quantif));
}

template <typename R, typename Q>
module::Quantizer<R,Q>* Quantizer::parameters
::build() const
{
	if (type == "POW2"   && implem == "STD" ) return new module::Quantizer_pow2     <R,Q>(size, n_decimals, n_bits, n_frames);
	if (type == "POW2"   && implem == "FAST") return new module::Quantizer_pow2_fast<R,Q>(size, n_decimals, n_bits, n_frames);
	if (type == "CUSTOM" && implem == "STD" ) return new module::Quantizer_custom   <R,Q>(size, range,      n_bits, n_frames);
	if (type == "NO"                        ) return new module::Quantizer_NO       <R,Q>(size,                     n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename R, typename Q>
module::Quantizer<R,Q>* Quantizer
::build(const parameters& params)
{
	return params.template build<R,Q>();
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Quantizer<R_8 ,Q_8 >* aff3ct::factory::Quantizer::parameters::build<R_8 ,Q_8 >() const;
template aff3ct::module::Quantizer<R_16,Q_16>* aff3ct::factory::Quantizer::parameters::build<R_16,Q_16>() const;
template aff3ct::module::Quantizer<R_32,Q_32>* aff3ct::factory::Quantizer::parameters::build<R_32,Q_32>() const;
template aff3ct::module::Quantizer<R_64,Q_64>* aff3ct::factory::Quantizer::parameters::build<R_64,Q_64>() const;
template aff3ct::module::Quantizer<R_8 ,Q_8 >* aff3ct::factory::Quantizer::build<R_8 ,Q_8 >(const aff3ct::factory::Quantizer::parameters&);
template aff3ct::module::Quantizer<R_16,Q_16>* aff3ct::factory::Quantizer::build<R_16,Q_16>(const aff3ct::factory::Quantizer::parameters&);
template aff3ct::module::Quantizer<R_32,Q_32>* aff3ct::factory::Quantizer::build<R_32,Q_32>(const aff3ct::factory::Quantizer::parameters&);
template aff3ct::module::Quantizer<R_64,Q_64>* aff3ct::factory::Quantizer::build<R_64,Q_64>(const aff3ct::factory::Quantizer::parameters&);
#else
template aff3ct::module::Quantizer<R,Q>* aff3ct::factory::Quantizer::parameters::build<R,Q>() const;
template aff3ct::module::Quantizer<R,Q>* aff3ct::factory::Quantizer::build<R,Q>(const aff3ct::factory::Quantizer::parameters&);
#endif
// ==================================================================================== explicit template instantiation
