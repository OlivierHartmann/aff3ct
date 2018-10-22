#include <cmath>

#include "Tools/Exception/exception.hpp"
#include "Tools/Math/utils.h"

#include "Module/Encoder/RS/Encoder_RS.hpp"

#include "Encoder_RS.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Encoder_RS_name   = "Encoder RS";
const std::string aff3ct::factory::Encoder_RS_prefix = "enc";

Encoder_RS::parameters
::parameters(const std::string &prefix)
: Encoder_RS::parameters(Encoder_RS_name, prefix)
{
}

Encoder_RS::parameters
::parameters(const std::string &name, const std::string &prefix)
: Encoder::parameters(name, prefix)
{
	type = "RS";

	type_set.insert("RS");
}

Encoder_RS::parameters* Encoder_RS::parameters
::clone() const
{
	return new Encoder_RS::parameters(*this);
}

void Encoder_RS::parameters
::register_arguments(CLI::App &app)
{
	Encoder::parameters::register_arguments(app);
}

void Encoder_RS::parameters
::callback_arguments()
{
	Encoder::parameters::callback_arguments();
}

void Encoder_RS::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	Encoder::parameters::get_headers(headers, full);
}

template <typename B>
module::Encoder_RS<B>* Encoder_RS::parameters
::build(const tools::RS_polynomial_generator &GF) const
{
	if (type == "RS") return new module::Encoder_RS<B>(K, N_cw, GF, n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B>
module::Encoder_RS<B>* Encoder_RS
::build(const parameters &params, const tools::RS_polynomial_generator &GF)
{
	return params.template build<B>(GF);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Encoder_RS<B_8 >* aff3ct::factory::Encoder_RS::parameters::build<B_8 >(const aff3ct::tools::RS_polynomial_generator&) const;
template aff3ct::module::Encoder_RS<B_16>* aff3ct::factory::Encoder_RS::parameters::build<B_16>(const aff3ct::tools::RS_polynomial_generator&) const;
template aff3ct::module::Encoder_RS<B_32>* aff3ct::factory::Encoder_RS::parameters::build<B_32>(const aff3ct::tools::RS_polynomial_generator&) const;
template aff3ct::module::Encoder_RS<B_64>* aff3ct::factory::Encoder_RS::parameters::build<B_64>(const aff3ct::tools::RS_polynomial_generator&) const;
template aff3ct::module::Encoder_RS<B_8 >* aff3ct::factory::Encoder_RS::build<B_8 >(const aff3ct::factory::Encoder_RS::parameters&, const aff3ct::tools::RS_polynomial_generator&);
template aff3ct::module::Encoder_RS<B_16>* aff3ct::factory::Encoder_RS::build<B_16>(const aff3ct::factory::Encoder_RS::parameters&, const aff3ct::tools::RS_polynomial_generator&);
template aff3ct::module::Encoder_RS<B_32>* aff3ct::factory::Encoder_RS::build<B_32>(const aff3ct::factory::Encoder_RS::parameters&, const aff3ct::tools::RS_polynomial_generator&);
template aff3ct::module::Encoder_RS<B_64>* aff3ct::factory::Encoder_RS::build<B_64>(const aff3ct::factory::Encoder_RS::parameters&, const aff3ct::tools::RS_polynomial_generator&);
#else
template aff3ct::module::Encoder_RS<B>* aff3ct::factory::Encoder_RS::parameters::build<B>(const aff3ct::tools::RS_polynomial_generator&) const;
template aff3ct::module::Encoder_RS<B>* aff3ct::factory::Encoder_RS::build<B>(const aff3ct::factory::Encoder_RS::parameters&, const aff3ct::tools::RS_polynomial_generator&);
#endif
// ==================================================================================== explicit template instantiation
