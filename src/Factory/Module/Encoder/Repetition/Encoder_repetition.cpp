#include "Tools/Exception/exception.hpp"

#include "Module/Encoder/Repetition/Encoder_repetition_sys.hpp"

#include "Encoder_repetition.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Encoder_repetition_name   = "Encoder Repetiton";
const std::string aff3ct::factory::Encoder_repetition_prefix = "enc";

Encoder_repetition::parameters
::parameters(const std::string &prefix)
: Encoder::parameters(Encoder_repetition_name, prefix)
{
	type = "REPETITION";

	type_set.insert({"REPETITION"});
}

Encoder_repetition::parameters* Encoder_repetition::parameters
::clone() const
{
	return new Encoder_repetition::parameters(*this);
}

void Encoder_repetition::parameters
::register_arguments(CLI::App &app)
{
	auto p = get_prefix();

	Encoder::parameters::register_arguments(app);

	CLI::add_flag(app, p,
		"--no-buff",
		not_buffered,
		"Disable the buffered encoding.")
		->group("Standard");
}

void Encoder_repetition::parameters
::callback_arguments()
{
	Encoder::parameters::callback_arguments();
}

void Encoder_repetition::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_short_name();

	Encoder::parameters::get_headers(headers, full);

	headers[p].push_back(std::make_pair("Buffered", (not_buffered ? "off" : "on")));
}

template <typename B>
module::Encoder_repetition_sys<B>* Encoder_repetition::parameters
::build() const
{
	if (type == "REPETITION") return new module::Encoder_repetition_sys<B>(K, N_cw, !not_buffered, n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B>
module::Encoder_repetition_sys<B>* Encoder_repetition
::build(const parameters &params)
{
	return params.template build<B>();
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Encoder_repetition_sys<B_8 >* aff3ct::factory::Encoder_repetition::parameters::build<B_8 >() const;
template aff3ct::module::Encoder_repetition_sys<B_16>* aff3ct::factory::Encoder_repetition::parameters::build<B_16>() const;
template aff3ct::module::Encoder_repetition_sys<B_32>* aff3ct::factory::Encoder_repetition::parameters::build<B_32>() const;
template aff3ct::module::Encoder_repetition_sys<B_64>* aff3ct::factory::Encoder_repetition::parameters::build<B_64>() const;
template aff3ct::module::Encoder_repetition_sys<B_8 >* aff3ct::factory::Encoder_repetition::build<B_8 >(const aff3ct::factory::Encoder_repetition::parameters&);
template aff3ct::module::Encoder_repetition_sys<B_16>* aff3ct::factory::Encoder_repetition::build<B_16>(const aff3ct::factory::Encoder_repetition::parameters&);
template aff3ct::module::Encoder_repetition_sys<B_32>* aff3ct::factory::Encoder_repetition::build<B_32>(const aff3ct::factory::Encoder_repetition::parameters&);
template aff3ct::module::Encoder_repetition_sys<B_64>* aff3ct::factory::Encoder_repetition::build<B_64>(const aff3ct::factory::Encoder_repetition::parameters&);
#else
template aff3ct::module::Encoder_repetition_sys<B>* aff3ct::factory::Encoder_repetition::parameters::build<B>() const;
template aff3ct::module::Encoder_repetition_sys<B>* aff3ct::factory::Encoder_repetition::build<B>(const aff3ct::factory::Encoder_repetition::parameters&);
#endif
// ==================================================================================== explicit template instantiation

