#include "Tools/Exception/exception.hpp"

#include "Module/Encoder/Turbo_DB/Encoder_turbo_DB.hpp"

#include "Encoder_turbo_DB.hpp"

const std::string aff3ct::factory::Encoder_turbo_DB_name   = "Encoder Turbo DB";
const std::string aff3ct::factory::Encoder_turbo_DB_prefix = "enc";

using namespace aff3ct;
using namespace aff3ct::factory;

Encoder_turbo_DB::parameters
::parameters(const std::string &prefix)
: Encoder::parameters(Encoder_turbo_DB_name, prefix),
  itl(new Interleaver::parameters("itl")),
  sub(new Encoder_RSC_DB::parameters("sub"))
{
	type = "TURBO_DB";

	type_set.insert("TURBO_DB");

	sub->no_argflag(true);
	itl->no_argflag(true);
}

Encoder_turbo_DB::parameters* Encoder_turbo_DB::parameters
::clone() const
{
	return new Encoder_turbo_DB::parameters(*this);
}

std::vector<std::string> Encoder_turbo_DB::parameters
::get_names() const
{
	auto n = Encoder::parameters::get_names();
	if (sub != nullptr) { auto nn = sub->get_names(); for (auto &x : nn) n.push_back(x); }
	if (itl != nullptr) { auto nn = itl->get_names(); for (auto &x : nn) n.push_back(x); }
	return n;
}
std::vector<std::string> Encoder_turbo_DB::parameters
::get_short_names() const
{
	auto sn = Encoder::parameters::get_short_names();
	if (sub != nullptr) { auto nn = sub->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	if (itl != nullptr) { auto nn = itl->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	return sn;
}

std::vector<std::string> Encoder_turbo_DB::parameters
::get_prefixes() const
{
	auto p = Encoder::parameters::get_prefixes();
	if (sub != nullptr) { auto nn = sub->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	if (itl != nullptr) { auto nn = itl->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	return p;
}

void Encoder_turbo_DB::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	Encoder::parameters::register_arguments(app);

	CLI::remove_option(app, "--cw-size", p, naf);

	if (itl != nullptr)
	{
		itl->register_arguments(app);

		CLI::remove_option(app, "--size", itl->get_prefix(), itl->no_argflag());
		CLI::remove_option(app, "--fra" , itl->get_prefix(), itl->no_argflag());
	}

	CLI::add_option(app, p, naf,
		"--json-path",
		json_path,
		"Path of store the encoder and decoder traces formated in JSON.")
		// ->check(CLI::ExistingFile) <- need write mode
		->group("Standard");

	sub->register_arguments(app);

	CLI::remove_option(app, "--info-bits", sub->get_prefix(), sub->no_argflag());
	CLI::remove_option(app, "--fra"      , sub->get_prefix(), sub->no_argflag());
	CLI::remove_option(app, "--seed"     , sub->get_prefix(), sub->no_argflag());
	CLI::remove_option(app, "--path"     , sub->get_prefix(), sub->no_argflag());
	CLI::remove_option(app, "--no-buff"  , sub->get_prefix(), sub->no_argflag());
}

void Encoder_turbo_DB::parameters
::callback_arguments()
{
	Encoder::parameters::callback_arguments();

	sub->K        = K;
	sub->n_frames = n_frames;
	sub->seed     = seed;

	sub->callback_arguments();

	N_cw = 2 * sub->N_cw - K;
	R    = (float)K / (float)N_cw;

	if (itl != nullptr)
	{
		itl->core->size     = K >> 1;
		itl->core->n_frames = n_frames;

		itl->callback_arguments();

		if (!itl->core->type_option_set_by_user())
		{
			if (sub->standard == "DVB-RCS1")
				itl->core->type = "DVB-RCS1";
			else if (sub->standard == "DVB-RCS2")
				itl->core->type = "DVB-RCS2";
		}
	}
}

void Encoder_turbo_DB::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_short_name();

	Encoder::parameters::get_headers(headers, full);

	if (itl != nullptr)
		itl->get_headers(headers, full);

	if (tail_length)
		headers[p].push_back(std::make_pair("Tail length", std::to_string(tail_length)));

	if (!json_path.empty())
		headers[p].push_back(std::make_pair("Path to the JSON file", json_path));

	sub->get_headers(headers, full);
}

template <typename B>
module::Encoder_turbo_DB<B>* Encoder_turbo_DB::parameters
::build(const module::Interleaver<B> &itl, module::Encoder_RSC_DB<B> &sub_enc) const
{
	if (type == "TURBO_DB") return new module::Encoder_turbo_DB<B>(K, N_cw, itl, sub_enc, sub_enc);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B>
module::Encoder_turbo_DB<B>* Encoder_turbo_DB
::build(const parameters                &params,
        const module::Interleaver<B>    &itl,
              module::Encoder_RSC_DB<B> &sub_enc)
{
	return params.template build<B>(itl, sub_enc);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Encoder_turbo_DB<B_8 >* aff3ct::factory::Encoder_turbo_DB::parameters::build<B_8 >(const aff3ct::module::Interleaver<B_8 >&, aff3ct::module::Encoder_RSC_DB<B_8 >&) const;
template aff3ct::module::Encoder_turbo_DB<B_16>* aff3ct::factory::Encoder_turbo_DB::parameters::build<B_16>(const aff3ct::module::Interleaver<B_16>&, aff3ct::module::Encoder_RSC_DB<B_16>&) const;
template aff3ct::module::Encoder_turbo_DB<B_32>* aff3ct::factory::Encoder_turbo_DB::parameters::build<B_32>(const aff3ct::module::Interleaver<B_32>&, aff3ct::module::Encoder_RSC_DB<B_32>&) const;
template aff3ct::module::Encoder_turbo_DB<B_64>* aff3ct::factory::Encoder_turbo_DB::parameters::build<B_64>(const aff3ct::module::Interleaver<B_64>&, aff3ct::module::Encoder_RSC_DB<B_64>&) const;
template aff3ct::module::Encoder_turbo_DB<B_8 >* aff3ct::factory::Encoder_turbo_DB::build<B_8 >(const aff3ct::factory::Encoder_turbo_DB::parameters&, const aff3ct::module::Interleaver<B_8 >&, aff3ct::module::Encoder_RSC_DB<B_8 >&);
template aff3ct::module::Encoder_turbo_DB<B_16>* aff3ct::factory::Encoder_turbo_DB::build<B_16>(const aff3ct::factory::Encoder_turbo_DB::parameters&, const aff3ct::module::Interleaver<B_16>&, aff3ct::module::Encoder_RSC_DB<B_16>&);
template aff3ct::module::Encoder_turbo_DB<B_32>* aff3ct::factory::Encoder_turbo_DB::build<B_32>(const aff3ct::factory::Encoder_turbo_DB::parameters&, const aff3ct::module::Interleaver<B_32>&, aff3ct::module::Encoder_RSC_DB<B_32>&);
template aff3ct::module::Encoder_turbo_DB<B_64>* aff3ct::factory::Encoder_turbo_DB::build<B_64>(const aff3ct::factory::Encoder_turbo_DB::parameters&, const aff3ct::module::Interleaver<B_64>&, aff3ct::module::Encoder_RSC_DB<B_64>&);
#else
template aff3ct::module::Encoder_turbo_DB<B>* aff3ct::factory::Encoder_turbo_DB::parameters::build<B>(const aff3ct::module::Interleaver<B>&, aff3ct::module::Encoder_RSC_DB<B>&) const;
template aff3ct::module::Encoder_turbo_DB<B>* aff3ct::factory::Encoder_turbo_DB::build<B>(const aff3ct::factory::Encoder_turbo_DB::parameters&, const aff3ct::module::Interleaver<B>&, aff3ct::module::Encoder_RSC_DB<B>&);
#endif
// ==================================================================================== explicit template instantiation
