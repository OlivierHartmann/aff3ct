#include <algorithm>

#include "Tools/Exception/exception.hpp"

#include "Encoder_RSC_DB.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Encoder_RSC_DB_name   = "Encoder RSC DB";
const std::string aff3ct::factory::Encoder_RSC_DB_prefix = "enc";

Encoder_RSC_DB::parameters
::parameters(const std::string &prefix)
: Encoder_RSC_DB::parameters(Encoder_RSC_DB_name, prefix)
{
}

Encoder_RSC_DB::parameters
::parameters(const std::string &name, const std::string &prefix)
: Encoder::parameters(name, prefix)
{
	type = "RSC_DB";

	type_set.insert("RSC_DB");
}

Encoder_RSC_DB::parameters* Encoder_RSC_DB::parameters
::clone() const
{
	return new Encoder_RSC_DB::parameters(*this);
}

void Encoder_RSC_DB::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	Encoder::parameters::register_arguments(app);

	CLI::remove_option(app, "--cw-size", p, naf);

	CLI::add_set(app, p, naf,
		"--std",
		standard,
		{"DVB-RCS1", "DVB-RCS2"},
		"Select a standard and set automatically some parameters (overwritten by user given arguments).",
		true)
		->group("Standard");

	CLI::add_flag(app, p, naf,
		"--no-buff",
		not_buffered,
		"Disable the buffered encoding.")
		->group("Standard");
}

void Encoder_RSC_DB::parameters
::callback_arguments()
{
	Encoder::parameters::callback_arguments();

	N_cw = 2 * K;
	R    = (float)K / (float)N_cw;
}

void Encoder_RSC_DB::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_name();

	Encoder::parameters::get_headers(headers, full);

	headers[p].push_back(std::make_pair("Buffered", (!not_buffered ? "on" : "off")));

	if (!standard.empty())
		headers[p].push_back(std::make_pair("Standard", standard));
}

template <typename B>
module::Encoder_RSC_DB<B>* Encoder_RSC_DB::parameters
::build() const
{
	if (type == "RSC_DB") return new module::Encoder_RSC_DB<B>(K, N_cw, standard, !not_buffered, n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B>
module::Encoder_RSC_DB<B>* Encoder_RSC_DB
::build(const parameters &params)
{
	return params.template build<B>();
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Encoder_RSC_DB<B_8 >* aff3ct::factory::Encoder_RSC_DB::parameters::build<B_8 >() const;
template aff3ct::module::Encoder_RSC_DB<B_16>* aff3ct::factory::Encoder_RSC_DB::parameters::build<B_16>() const;
template aff3ct::module::Encoder_RSC_DB<B_32>* aff3ct::factory::Encoder_RSC_DB::parameters::build<B_32>() const;
template aff3ct::module::Encoder_RSC_DB<B_64>* aff3ct::factory::Encoder_RSC_DB::parameters::build<B_64>() const;
template aff3ct::module::Encoder_RSC_DB<B_8 >* aff3ct::factory::Encoder_RSC_DB::build<B_8 >(const aff3ct::factory::Encoder_RSC_DB::parameters&);
template aff3ct::module::Encoder_RSC_DB<B_16>* aff3ct::factory::Encoder_RSC_DB::build<B_16>(const aff3ct::factory::Encoder_RSC_DB::parameters&);
template aff3ct::module::Encoder_RSC_DB<B_32>* aff3ct::factory::Encoder_RSC_DB::build<B_32>(const aff3ct::factory::Encoder_RSC_DB::parameters&);
template aff3ct::module::Encoder_RSC_DB<B_64>* aff3ct::factory::Encoder_RSC_DB::build<B_64>(const aff3ct::factory::Encoder_RSC_DB::parameters&);
#else
template aff3ct::module::Encoder_RSC_DB<B>* aff3ct::factory::Encoder_RSC_DB::parameters::build<B>() const;
template aff3ct::module::Encoder_RSC_DB<B>* aff3ct::factory::Encoder_RSC_DB::build<B>(const aff3ct::factory::Encoder_RSC_DB::parameters&);
#endif
// ==================================================================================== explicit template instantiation
