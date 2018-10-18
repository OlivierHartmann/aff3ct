#include "Tools/Exception/exception.hpp"

#include "Module/Encoder/Polar/Encoder_polar.hpp"
#include "Module/Encoder/Polar/Encoder_polar_sys.hpp"

#include "Encoder_polar.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Encoder_polar_name   = "Encoder Polar";
const std::string aff3ct::factory::Encoder_polar_prefix = "enc";

Encoder_polar::parameters
::parameters(const std::string &prefix)
: Encoder::parameters(Encoder_polar_name, prefix)
{
	type = "POLAR";

	type_set.insert({"POLAR"});
}

Encoder_polar::parameters* Encoder_polar::parameters
::clone() const
{
	return new Encoder_polar::parameters(*this);
}

void Encoder_polar::parameters
::register_arguments(CLI::App &app)
{
	auto p = get_prefix();

	Encoder::parameters::register_arguments(app);

	CLI::add_flag(app, p,
		"--no-sys",
		not_systematic,
		"Disable the systematic encoding.")
		->group("Standard");
}

void Encoder_polar::parameters
::callback_arguments()
{
	Encoder::parameters::callback_arguments();
}

void Encoder_polar::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	Encoder::parameters::get_headers(headers, full);
}

template <typename B>
module::Encoder_polar<B>* Encoder_polar::parameters
::build(const std::vector<bool> &frozen_bits) const
{
	if (type == "POLAR" &&  not_systematic) return new module::Encoder_polar    <B>(K, N_cw, frozen_bits, n_frames);
	if (type == "POLAR" && !not_systematic) return new module::Encoder_polar_sys<B>(K, N_cw, frozen_bits, n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B>
module::Encoder_polar<B>* Encoder_polar
::build(const parameters& params, const std::vector<bool> &frozen_bits)
{
	return params.template build<B>(frozen_bits);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Encoder_polar<B_8 >* aff3ct::factory::Encoder_polar::parameters::build<B_8 >(const std::vector<bool>&) const;
template aff3ct::module::Encoder_polar<B_16>* aff3ct::factory::Encoder_polar::parameters::build<B_16>(const std::vector<bool>&) const;
template aff3ct::module::Encoder_polar<B_32>* aff3ct::factory::Encoder_polar::parameters::build<B_32>(const std::vector<bool>&) const;
template aff3ct::module::Encoder_polar<B_64>* aff3ct::factory::Encoder_polar::parameters::build<B_64>(const std::vector<bool>&) const;
template aff3ct::module::Encoder_polar<B_8 >* aff3ct::factory::Encoder_polar::build<B_8 >(const aff3ct::factory::Encoder_polar::parameters&, const std::vector<bool>&);
template aff3ct::module::Encoder_polar<B_16>* aff3ct::factory::Encoder_polar::build<B_16>(const aff3ct::factory::Encoder_polar::parameters&, const std::vector<bool>&);
template aff3ct::module::Encoder_polar<B_32>* aff3ct::factory::Encoder_polar::build<B_32>(const aff3ct::factory::Encoder_polar::parameters&, const std::vector<bool>&);
template aff3ct::module::Encoder_polar<B_64>* aff3ct::factory::Encoder_polar::build<B_64>(const aff3ct::factory::Encoder_polar::parameters&, const std::vector<bool>&);
#else
template aff3ct::module::Encoder_polar<B>* aff3ct::factory::Encoder_polar::parameters::build<B>(const std::vector<bool>&) const;
template aff3ct::module::Encoder_polar<B>* aff3ct::factory::Encoder_polar::build<B>(const aff3ct::factory::Encoder_polar::parameters&, const std::vector<bool>&);
#endif
// ==================================================================================== explicit template instantiation
