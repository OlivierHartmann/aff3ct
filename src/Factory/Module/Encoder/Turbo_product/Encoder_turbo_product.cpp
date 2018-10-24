#include "Tools/Exception/exception.hpp"

#include "Module/Encoder/Turbo_product/Encoder_turbo_product.hpp"

#include "Encoder_turbo_product.hpp"

const std::string aff3ct::factory::Encoder_turbo_product_name   = "Encoder Turbo Product Code";
const std::string aff3ct::factory::Encoder_turbo_product_prefix = "enc";

using namespace aff3ct;
using namespace aff3ct::factory;

Encoder_turbo_product::parameters
::parameters(const std::string &prefix)
: Encoder_turbo_product::parameters(Encoder_turbo_product_name, prefix)
{
}

Encoder_turbo_product::parameters
::parameters(const std::string &name, const std::string &prefix)
: Encoder::parameters(name, prefix),
  sub(new Encoder_BCH::parameters("sub")),
  itl(new Interleaver::parameters("itl"))
{
	type = "TURBO_PROD";

	type_set.insert("TURBO_PROD");

	sub->no_argflag(true);
	itl->no_argflag(true);
}

Encoder_turbo_product::parameters* Encoder_turbo_product::parameters
::clone() const
{
	return new Encoder_turbo_product::parameters(*this);
}

std::vector<std::string> Encoder_turbo_product::parameters
::get_names() const
{
	auto n = Encoder::parameters::get_names();
	if (sub != nullptr) { auto nn = sub->get_names(); for (auto &x : nn) n.push_back(x); }
	if (itl != nullptr) { auto nn = itl->get_names(); for (auto &x : nn) n.push_back(x); }
	return n;
}

std::vector<std::string> Encoder_turbo_product::parameters
::get_short_names() const
{
	auto sn = Encoder::parameters::get_short_names();
	if (sub != nullptr) { auto nn = sub->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	if (itl != nullptr) { auto nn = itl->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	return sn;
}

std::vector<std::string> Encoder_turbo_product::parameters
::get_prefixes() const
{
	auto p = Encoder::parameters::get_prefixes();
	if (sub != nullptr) { auto nn = sub->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	if (itl != nullptr) { auto nn = itl->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	return p;
}

void Encoder_turbo_product::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	Encoder::parameters::register_arguments(app);

	if (itl != nullptr)
	{
		itl->register_arguments(app);

		CLI::remove_option(app, "--size", itl->get_prefix(), itl->no_argflag());
		CLI::remove_option(app, "--fra" , itl->get_prefix(), itl->no_argflag());
	}

	CLI::add_flag(app, p, naf,
		"--ext",
		parity_extended,
		"Extends code with a parity bits.")
		->group("Standard");


	sub->type_set = {"BCH"};
	sub->register_arguments(app);

	CLI::remove_option(app, "--info-bits", sub->get_prefix(), sub->no_argflag());
	CLI::remove_option(app, "--cw-size",   sub->get_prefix(), sub->no_argflag());
	CLI::remove_option(app, "--fra",       sub->get_prefix(), sub->no_argflag());
	CLI::remove_option(app, "--path",      sub->get_prefix(), sub->no_argflag());
	CLI::remove_option(app, "--start-idx", sub->get_prefix(), sub->no_argflag());
	CLI::remove_option(app, "--seed",      sub->get_prefix(), sub->no_argflag());
}

void Encoder_turbo_product::parameters
::callback_arguments()
{
	Encoder::parameters::callback_arguments();

	sub->K        = K;
	sub->N_cw     = N_cw;
	sub->n_frames = n_frames;
	sub->callback_arguments();

	K    = sub->K    * sub->K;
	N_cw = sub->N_cw * sub->N_cw;
	R    = (float)K / (float)N_cw;


	if (itl != nullptr)
	{
		itl->core->type     = "ROW_COL";
		itl->core->n_frames = n_frames;

		if (parity_extended)
			itl->core->n_cols = sub->N_cw +1;
		else
			itl->core->n_cols = sub->N_cw;

		itl->core->size = itl->core->n_cols * itl->core->n_cols;
		N_cw = itl->core->size;

		itl->callback_arguments();
	}
}

void Encoder_turbo_product::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_name();

	Encoder::parameters::get_headers(headers, full);

	if (itl != nullptr)
		itl->get_headers(headers, full);

	headers[p].push_back(std::make_pair("Parity extended", (parity_extended ? "yes" : "no")));

	sub->get_headers(headers, full);
}

template <typename B>
module::Encoder_turbo_product<B>* Encoder_turbo_product::parameters
::build(const module::Interleaver<B> &itl,
              module::Encoder_BCH<B> &enc_r,
              module::Encoder_BCH<B> &enc_c) const
{
	if (type == "TURBO_PROD") return new module::Encoder_turbo_product<B>(itl, enc_r, enc_c, n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B>
module::Encoder_turbo_product<B>* Encoder_turbo_product
::build(const parameters              &params,
        const module::Interleaver<B> &itl,
              module::Encoder_BCH<B> &enc_r,
              module::Encoder_BCH<B> &enc_c)
{
	return params.template build<B>(itl, enc_r, enc_c);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Encoder_turbo_product<B_8 >* aff3ct::factory::Encoder_turbo_product::parameters::build<B_8 >(const aff3ct::module::Interleaver<B_8 >&, aff3ct::module::Encoder_BCH<B_8 >&, aff3ct::module::Encoder_BCH<B_8 >&) const;
template aff3ct::module::Encoder_turbo_product<B_16>* aff3ct::factory::Encoder_turbo_product::parameters::build<B_16>(const aff3ct::module::Interleaver<B_16>&, aff3ct::module::Encoder_BCH<B_16>&, aff3ct::module::Encoder_BCH<B_16>&) const;
template aff3ct::module::Encoder_turbo_product<B_32>* aff3ct::factory::Encoder_turbo_product::parameters::build<B_32>(const aff3ct::module::Interleaver<B_32>&, aff3ct::module::Encoder_BCH<B_32>&, aff3ct::module::Encoder_BCH<B_32>&) const;
template aff3ct::module::Encoder_turbo_product<B_64>* aff3ct::factory::Encoder_turbo_product::parameters::build<B_64>(const aff3ct::module::Interleaver<B_64>&, aff3ct::module::Encoder_BCH<B_64>&, aff3ct::module::Encoder_BCH<B_64>&) const;
template aff3ct::module::Encoder_turbo_product<B_8 >* aff3ct::factory::Encoder_turbo_product::build<B_8 >(const aff3ct::factory::Encoder_turbo_product::parameters&, const aff3ct::module::Interleaver<B_8 >&, aff3ct::module::Encoder_BCH<B_8 >&, aff3ct::module::Encoder_BCH<B_8 >&);
template aff3ct::module::Encoder_turbo_product<B_16>* aff3ct::factory::Encoder_turbo_product::build<B_16>(const aff3ct::factory::Encoder_turbo_product::parameters&, const aff3ct::module::Interleaver<B_16>&, aff3ct::module::Encoder_BCH<B_16>&, aff3ct::module::Encoder_BCH<B_16>&);
template aff3ct::module::Encoder_turbo_product<B_32>* aff3ct::factory::Encoder_turbo_product::build<B_32>(const aff3ct::factory::Encoder_turbo_product::parameters&, const aff3ct::module::Interleaver<B_32>&, aff3ct::module::Encoder_BCH<B_32>&, aff3ct::module::Encoder_BCH<B_32>&);
template aff3ct::module::Encoder_turbo_product<B_64>* aff3ct::factory::Encoder_turbo_product::build<B_64>(const aff3ct::factory::Encoder_turbo_product::parameters&, const aff3ct::module::Interleaver<B_64>&, aff3ct::module::Encoder_BCH<B_64>&, aff3ct::module::Encoder_BCH<B_64>&);
#else
template aff3ct::module::Encoder_turbo_product<B>* aff3ct::factory::Encoder_turbo_product::parameters::build<B>(const aff3ct::module::Interleaver<B>&, aff3ct::module::Encoder_BCH<B>&, aff3ct::module::Encoder_BCH<B>&) const;
template aff3ct::module::Encoder_turbo_product<B>* aff3ct::factory::Encoder_turbo_product::build<B>(const aff3ct::factory::Encoder_turbo_product::parameters&, const aff3ct::module::Interleaver<B>&, aff3ct::module::Encoder_BCH<B>&, aff3ct::module::Encoder_BCH<B>&);
#endif
// ==================================================================================== explicit template instantiation
