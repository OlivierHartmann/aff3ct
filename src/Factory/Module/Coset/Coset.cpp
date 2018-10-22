#include "Module/Coset/Bit/Coset_bit.hpp"
#include "Module/Coset/Real/Coset_real.hpp"

#include "Coset.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Coset_name   = "Coset";
const std::string aff3ct::factory::Coset_prefix = "cst";

Coset::parameters
::parameters(const std::string &prefix)
: Factory::parameters(Coset_name, Coset_name, prefix)
{
}

Coset::parameters* Coset::parameters
::clone() const
{
	return new Coset::parameters(*this);
}

void Coset::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	CLI::add_option(app, p, naf,
		"-N,--size",
		size,
		"Coset size.")
		->required()
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_set(app, p, naf,
		"--type",
		type,
		{"STD"},
		"Coset type.",
		true)
		->group("Standard");

	CLI::add_option(app, p, naf,
		"-F,--fra",
		n_frames,
		"Set the number of inter frame level to process.",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");
}

void Coset::parameters
::callback_arguments()
{
}

void Coset::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_name();

	headers[p].push_back(std::make_pair("Type", this->type));
	if (full) headers[p].push_back(std::make_pair("Size (N)", std::to_string(this->size)));
	if (full) headers[p].push_back(std::make_pair("Inter frame level", std::to_string(this->n_frames)));
}

template <typename B1, typename B2>
module::Coset<B1,B2>* Coset::parameters
::build_bit() const
{
	if (type == "STD") return new module::Coset_bit<B1,B2>(size, n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename R>
module::Coset<B,R>* Coset::parameters
::build_real() const
{
	if (type == "STD") return new module::Coset_real<B,R>(size, n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B1, typename B2>
module::Coset<B1,B2>* Coset
::build_bit(const parameters &params)
{
	return params.template build_bit<B1,B2>();
}

template <typename B, typename R>
module::Coset<B,R>* Coset
::build_real(const parameters &params)
{
	return params.template build_real<B,R>();
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Coset<B_8 ,B_8 >* aff3ct::factory::Coset::parameters::build_bit<B_8 ,B_8 >() const;
template aff3ct::module::Coset<B_16,B_16>* aff3ct::factory::Coset::parameters::build_bit<B_16,B_16>() const;
template aff3ct::module::Coset<B_32,B_32>* aff3ct::factory::Coset::parameters::build_bit<B_32,B_32>() const;
template aff3ct::module::Coset<B_64,B_64>* aff3ct::factory::Coset::parameters::build_bit<B_64,B_64>() const;
template aff3ct::module::Coset<B_8 ,B_8 >* aff3ct::factory::Coset::build_bit<B_8 ,B_8 >(const aff3ct::factory::Coset::parameters&);
template aff3ct::module::Coset<B_16,B_16>* aff3ct::factory::Coset::build_bit<B_16,B_16>(const aff3ct::factory::Coset::parameters&);
template aff3ct::module::Coset<B_32,B_32>* aff3ct::factory::Coset::build_bit<B_32,B_32>(const aff3ct::factory::Coset::parameters&);
template aff3ct::module::Coset<B_64,B_64>* aff3ct::factory::Coset::build_bit<B_64,B_64>(const aff3ct::factory::Coset::parameters&);
#else
template aff3ct::module::Coset<B,B>* aff3ct::factory::Coset::parameters::build_bit<B,B>() const;
template aff3ct::module::Coset<B,B>* aff3ct::factory::Coset::build_bit<B,B>(const aff3ct::factory::Coset::parameters&);
#endif

#ifdef MULTI_PREC
template aff3ct::module::Coset<B_8 ,Q_8 >* aff3ct::factory::Coset::parameters::build_real<B_8 ,Q_8 >() const;
template aff3ct::module::Coset<B_16,Q_16>* aff3ct::factory::Coset::parameters::build_real<B_16,Q_16>() const;
template aff3ct::module::Coset<B_32,Q_32>* aff3ct::factory::Coset::parameters::build_real<B_32,Q_32>() const;
template aff3ct::module::Coset<B_64,Q_64>* aff3ct::factory::Coset::parameters::build_real<B_64,Q_64>() const;
template aff3ct::module::Coset<B_8 ,Q_8 >* aff3ct::factory::Coset::build_real<B_8 ,Q_8 >(const aff3ct::factory::Coset::parameters&);
template aff3ct::module::Coset<B_16,Q_16>* aff3ct::factory::Coset::build_real<B_16,Q_16>(const aff3ct::factory::Coset::parameters&);
template aff3ct::module::Coset<B_32,Q_32>* aff3ct::factory::Coset::build_real<B_32,Q_32>(const aff3ct::factory::Coset::parameters&);
template aff3ct::module::Coset<B_64,Q_64>* aff3ct::factory::Coset::build_real<B_64,Q_64>(const aff3ct::factory::Coset::parameters&);
#else
template aff3ct::module::Coset<B,Q>* aff3ct::factory::Coset::parameters::build_real<B,Q>() const;
template aff3ct::module::Coset<B,Q>* aff3ct::factory::Coset::build_real<B,Q>(const aff3ct::factory::Coset::parameters&);
#endif
// ==================================================================================== explicit template instantiation
