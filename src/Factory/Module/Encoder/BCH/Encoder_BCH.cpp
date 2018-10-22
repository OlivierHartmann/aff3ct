#include <cmath>

#include "Tools/Exception/exception.hpp"
#include "Module/Encoder/BCH/Encoder_BCH.hpp"

#include "Encoder_BCH.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Encoder_BCH_name   = "Encoder BCH";
const std::string aff3ct::factory::Encoder_BCH_prefix = "enc";

Encoder_BCH::parameters
::parameters(const std::string &prefix)
: Encoder_BCH::parameters(Encoder_BCH_name, prefix)
{
}

Encoder_BCH::parameters
::parameters(const std::string &name, const std::string &prefix)
: Encoder::parameters(name, prefix)
{
	type = "BCH";

	type_set.insert("BCH");
}

Encoder_BCH::parameters* Encoder_BCH::parameters
::clone() const
{
	return new Encoder_BCH::parameters(*this);
}

void Encoder_BCH::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	Encoder::parameters::register_arguments(app);

	CLI::get_option(app, "--cw-size", p, naf)->check(CLI::Power_of_two_minus_one<decltype(N_cw)>());
}

void Encoder_BCH::parameters
::callback_arguments()
{
	Encoder::parameters::callback_arguments();
}

void Encoder_BCH::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	Encoder::parameters::get_headers(headers, full);
}

template <typename B>
module::Encoder_BCH<B>* Encoder_BCH::parameters
::build(const tools::BCH_polynomial_generator<B> &GF) const
{
	if (type == "BCH") return new module::Encoder_BCH<B>(K, N_cw, GF, n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B>
module::Encoder_BCH<B>* Encoder_BCH
::build(const parameters &params, const tools::BCH_polynomial_generator<B> &GF)
{
	return params.template build<B>(GF);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Encoder_BCH<B_8 >* aff3ct::factory::Encoder_BCH::parameters::build<B_8 >(const aff3ct::tools::BCH_polynomial_generator<B_8 >&) const;
template aff3ct::module::Encoder_BCH<B_16>* aff3ct::factory::Encoder_BCH::parameters::build<B_16>(const aff3ct::tools::BCH_polynomial_generator<B_16>&) const;
template aff3ct::module::Encoder_BCH<B_32>* aff3ct::factory::Encoder_BCH::parameters::build<B_32>(const aff3ct::tools::BCH_polynomial_generator<B_32>&) const;
template aff3ct::module::Encoder_BCH<B_64>* aff3ct::factory::Encoder_BCH::parameters::build<B_64>(const aff3ct::tools::BCH_polynomial_generator<B_64>&) const;
template aff3ct::module::Encoder_BCH<B_8 >* aff3ct::factory::Encoder_BCH::build<B_8 >(const aff3ct::factory::Encoder_BCH::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_8 >&);
template aff3ct::module::Encoder_BCH<B_16>* aff3ct::factory::Encoder_BCH::build<B_16>(const aff3ct::factory::Encoder_BCH::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_16>&);
template aff3ct::module::Encoder_BCH<B_32>* aff3ct::factory::Encoder_BCH::build<B_32>(const aff3ct::factory::Encoder_BCH::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_32>&);
template aff3ct::module::Encoder_BCH<B_64>* aff3ct::factory::Encoder_BCH::build<B_64>(const aff3ct::factory::Encoder_BCH::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_64>&);
#else
template aff3ct::module::Encoder_BCH<B>* aff3ct::factory::Encoder_BCH::parameters::build<B>(const aff3ct::tools::BCH_polynomial_generator<B>&) const;
template aff3ct::module::Encoder_BCH<B>* aff3ct::factory::Encoder_BCH::build<B>(const aff3ct::factory::Encoder_BCH::parameters&, const aff3ct::tools::BCH_polynomial_generator<B>&);
#endif
// ==================================================================================== explicit template instantiation
