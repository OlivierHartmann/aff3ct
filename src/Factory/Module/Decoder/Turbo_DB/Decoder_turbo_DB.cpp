#include "Tools/Exception/exception.hpp"

#include "Module/Decoder/Turbo_DB/Decoder_turbo_DB.hpp"

#include "Decoder_turbo_DB.hpp"

const std::string aff3ct::factory::Decoder_turbo_DB_name   = "Decoder Turbo DB";
const std::string aff3ct::factory::Decoder_turbo_DB_prefix = "dec";

using namespace aff3ct;
using namespace aff3ct::factory;

Decoder_turbo_DB::parameters
::parameters(const std::string &prefix)
: Decoder::parameters(Decoder_turbo_DB_name, prefix),
  sub(new Decoder_RSC_DB   ::parameters("sub")),
  itl(new Interleaver      ::parameters("itl")),
  sf (new Scaling_factor   ::parameters("sf" )),
  fnc(new Flip_and_check_DB::parameters("fnc"))
{
	type   = "TURBO_DB";
	implem = "STD";

	type_set  .insert("TURBO_DB");
	implem_set.insert("STD");

	sub->no_argflag(true);
	itl->no_argflag(true);
	sf ->no_argflag(true);
	fnc->no_argflag(true);
}

Decoder_turbo_DB::parameters* Decoder_turbo_DB::parameters
::clone() const
{
	return new Decoder_turbo_DB::parameters(*this);
}

std::vector<std::string> Decoder_turbo_DB::parameters
::get_names() const
{
	auto n = Decoder::parameters::get_names();
	if (sf  != nullptr) { auto nn = sf ->get_names(); for (auto &x : nn) n.push_back(x); }
	if (fnc != nullptr) { auto nn = fnc->get_names(); for (auto &x : nn) n.push_back(x); }
	if (sub != nullptr) { auto nn = sub->get_names(); for (auto &x : nn) n.push_back(x); }
	if (itl != nullptr) { auto nn = itl->get_names(); for (auto &x : nn) n.push_back(x); }
	return n;
}

std::vector<std::string> Decoder_turbo_DB::parameters
::get_short_names() const
{
	auto sn = Decoder::parameters::get_short_names();
	if (sf  != nullptr) { auto nn = sf ->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	if (fnc != nullptr) { auto nn = fnc->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	if (sub != nullptr) { auto nn = sub->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	if (itl != nullptr) { auto nn = itl->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	return sn;
}

std::vector<std::string> Decoder_turbo_DB::parameters
::get_prefixes() const
{
	auto p = Decoder::parameters::get_prefixes();
	if (sf  != nullptr) { auto nn = sf ->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	if (fnc != nullptr) { auto nn = fnc->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	if (sub != nullptr) { auto nn = sub->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	if (itl != nullptr) { auto nn = itl->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	return p;
}

void Decoder_turbo_DB::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	Decoder::parameters::register_arguments(app);

	CLI::remove_option(app, "--cw-size", p, naf);

	if (itl != nullptr)
	{
		itl->register_arguments(app);

		CLI::remove_option(app, "--size", itl->get_prefix(), itl->no_argflag());
		CLI::remove_option(app, "--fra" , itl->get_prefix(), itl->no_argflag());
	}

	CLI::add_option(app, p, naf,
		"-i,--ite",
		n_ite,
		"Maximal number of iterations in the turbo decoder.",
		true)
		->group("Standard");


	sf->register_arguments(app);

	CLI::remove_option(app, "--ite", sf->get_prefix(), sf->no_argflag());


	fnc->register_arguments(app);

	CLI::remove_option(app, "--size", fnc->get_prefix(), fnc->no_argflag());
	CLI::remove_option(app, "--fra" , fnc->get_prefix(), fnc->no_argflag());
	CLI::remove_option(app, "--ite" , fnc->get_prefix(), fnc->no_argflag());


	sub->register_arguments(app);

	CLI::remove_option(app, "--info-bits", sub->get_prefix(), sub->no_argflag());
	CLI::remove_option(app, "--fra"      , sub->get_prefix(), sub->no_argflag());
}

void Decoder_turbo_DB::parameters
::callback_arguments()
{
	auto p = get_short_name();

	Decoder::parameters::callback_arguments();

	sub->K        = K;
	sub->n_frames = n_frames;

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
			if (sub->implem == "DVB-RCS1")
				itl->core->type = "DVB-RCS1";
			else if (sub->implem == "DVB-RCS2")
				itl->core->type = "DVB-RCS2";
		}
	}

	sf->n_ite = n_ite;

	sf->callback_arguments();

	fnc->size     = K;
	fnc->n_frames = n_frames;
	fnc->n_ite    = n_ite;

	fnc->callback_arguments();
}

void Decoder_turbo_DB::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_short_name();

	Decoder::parameters::get_headers(headers, full);

	if (type != "ML" && type != "CHASE")
	{
		if (itl != nullptr)
			itl->get_headers(headers, full);

		headers[p].push_back(std::make_pair("Num. of iterations (i)", std::to_string(n_ite)));
		if (tail_length && full)
			headers[p].push_back(std::make_pair("Tail length", std::to_string(tail_length)));

		sf ->get_headers(headers, full);
		fnc->get_headers(headers, full);
		sub->get_headers(headers, full);
	}
}

template <typename B, typename Q>
module::Decoder_turbo_DB<B,Q>* Decoder_turbo_DB::parameters
::build(const module::Interleaver<Q>           &itl,
              module::Decoder_RSC_DB_BCJR<B,Q> &siso_n,
              module::Decoder_RSC_DB_BCJR<B,Q> &siso_i,
              const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	if (type == "TURBO_DB")
	{
		if (implem == "STD") return new module::Decoder_turbo_DB<B,Q>(K, N_cw, n_ite, itl, siso_n, siso_i);
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_turbo_DB::parameters
::build(const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	return Decoder::parameters::build<B,Q>(encoder);
}

template <typename B, typename Q>
module::Decoder_turbo_DB<B,Q>* Decoder_turbo_DB
::build(const parameters                       &params,
        const module::Interleaver<Q>           &itl,
              module::Decoder_RSC_DB_BCJR<B,Q> &siso_n,
              module::Decoder_RSC_DB_BCJR<B,Q> &siso_i,
              const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build<B,Q>(itl, siso_n, siso_i, encoder);
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_turbo_DB
::build(const parameters &params, const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build<B,Q>(encoder);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Decoder_turbo_DB<B_8 ,Q_8 >* aff3ct::factory::Decoder_turbo_DB::parameters::build<B_8 ,Q_8 >(const aff3ct::module::Interleaver<Q_8 >&, aff3ct::module::Decoder_RSC_DB_BCJR<B_8 ,Q_8 >&, aff3ct::module::Decoder_RSC_DB_BCJR<B_8 ,Q_8 >&, const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_turbo_DB<B_16,Q_16>* aff3ct::factory::Decoder_turbo_DB::parameters::build<B_16,Q_16>(const aff3ct::module::Interleaver<Q_16>&, aff3ct::module::Decoder_RSC_DB_BCJR<B_16,Q_16>&, aff3ct::module::Decoder_RSC_DB_BCJR<B_16,Q_16>&, const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_turbo_DB<B_32,Q_32>* aff3ct::factory::Decoder_turbo_DB::parameters::build<B_32,Q_32>(const aff3ct::module::Interleaver<Q_32>&, aff3ct::module::Decoder_RSC_DB_BCJR<B_32,Q_32>&, aff3ct::module::Decoder_RSC_DB_BCJR<B_32,Q_32>&, const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_turbo_DB<B_64,Q_64>* aff3ct::factory::Decoder_turbo_DB::parameters::build<B_64,Q_64>(const aff3ct::module::Interleaver<Q_64>&, aff3ct::module::Decoder_RSC_DB_BCJR<B_64,Q_64>&, aff3ct::module::Decoder_RSC_DB_BCJR<B_64,Q_64>&, const std::unique_ptr<module::Encoder<B_64>>&) const;
template aff3ct::module::Decoder_turbo_DB<B_8 ,Q_8 >* aff3ct::factory::Decoder_turbo_DB::build<B_8 ,Q_8 >(const aff3ct::factory::Decoder_turbo_DB::parameters&, const aff3ct::module::Interleaver<Q_8 >&, aff3ct::module::Decoder_RSC_DB_BCJR<B_8 ,Q_8 >&, aff3ct::module::Decoder_RSC_DB_BCJR<B_8 ,Q_8 >&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_turbo_DB<B_16,Q_16>* aff3ct::factory::Decoder_turbo_DB::build<B_16,Q_16>(const aff3ct::factory::Decoder_turbo_DB::parameters&, const aff3ct::module::Interleaver<Q_16>&, aff3ct::module::Decoder_RSC_DB_BCJR<B_16,Q_16>&, aff3ct::module::Decoder_RSC_DB_BCJR<B_16,Q_16>&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_turbo_DB<B_32,Q_32>* aff3ct::factory::Decoder_turbo_DB::build<B_32,Q_32>(const aff3ct::factory::Decoder_turbo_DB::parameters&, const aff3ct::module::Interleaver<Q_32>&, aff3ct::module::Decoder_RSC_DB_BCJR<B_32,Q_32>&, aff3ct::module::Decoder_RSC_DB_BCJR<B_32,Q_32>&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_turbo_DB<B_64,Q_64>* aff3ct::factory::Decoder_turbo_DB::build<B_64,Q_64>(const aff3ct::factory::Decoder_turbo_DB::parameters&, const aff3ct::module::Interleaver<Q_64>&, aff3ct::module::Decoder_RSC_DB_BCJR<B_64,Q_64>&, aff3ct::module::Decoder_RSC_DB_BCJR<B_64,Q_64>&, const std::unique_ptr<module::Encoder<B_64>>&);
#else
template aff3ct::module::Decoder_turbo_DB<B,Q>* aff3ct::factory::Decoder_turbo_DB::parameters::build<B,Q>(const aff3ct::module::Interleaver<Q>&, aff3ct::module::Decoder_RSC_DB_BCJR<B,Q>&, aff3ct::module::Decoder_RSC_DB_BCJR<B,Q>&, const std::unique_ptr<module::Encoder<B>>& ) const;
template aff3ct::module::Decoder_turbo_DB<B,Q>* aff3ct::factory::Decoder_turbo_DB::build<B,Q>(const aff3ct::factory::Decoder_turbo_DB::parameters&, const aff3ct::module::Interleaver<Q>&, aff3ct::module::Decoder_RSC_DB_BCJR<B,Q>&, aff3ct::module::Decoder_RSC_DB_BCJR<B,Q>&, const std::unique_ptr<module::Encoder<B>>& );
#endif

#ifdef MULTI_PREC
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_turbo_DB::parameters::build<B_8 ,Q_8 >(const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_turbo_DB::parameters::build<B_16,Q_16>(const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_turbo_DB::parameters::build<B_32,Q_32>(const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_turbo_DB::parameters::build<B_64,Q_64>(const std::unique_ptr<module::Encoder<B_64>>&) const;
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_turbo_DB::build<B_8 ,Q_8 >(const aff3ct::factory::Decoder_turbo_DB::parameters&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_turbo_DB::build<B_16,Q_16>(const aff3ct::factory::Decoder_turbo_DB::parameters&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_turbo_DB::build<B_32,Q_32>(const aff3ct::factory::Decoder_turbo_DB::parameters&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_turbo_DB::build<B_64,Q_64>(const aff3ct::factory::Decoder_turbo_DB::parameters&, const std::unique_ptr<module::Encoder<B_64>>&);
#else
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_turbo_DB::parameters::build<B,Q>(const std::unique_ptr<module::Encoder<B>>& ) const;
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_turbo_DB::build<B,Q>(const aff3ct::factory::Decoder_turbo_DB::parameters&, const std::unique_ptr<module::Encoder<B>>& );
#endif
// ==================================================================================== explicit template instantiation
