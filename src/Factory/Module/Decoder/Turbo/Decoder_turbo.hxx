#include "Tools/Exception/exception.hpp"

#include "Module/Decoder/Turbo/Decoder_turbo_std.hpp"
#include "Module/Decoder/Turbo/Decoder_turbo_fast.hpp"

#include "Decoder_turbo.hpp"

namespace aff3ct
{
namespace factory
{
template <class D1, class D2>
Decoder_turbo::parameters<D1,D2>
::parameters(const std::string &prefix)
: Decoder_turbo::parameters<D1,D2>(Decoder_turbo_name, prefix)
{
}

template <class D1, class D2>
Decoder_turbo::parameters<D1,D2>
::parameters(const std::string &name, const std::string &prefix)
: Decoder::parameters(name, prefix),
  sub1(new typename D1::parameters(std::is_same<D1,D2>() ? "sub Decoder" : "sub Decoder 1", std::is_same<D1,D2>() ? "sub" : "sub1")),
  sub2(new typename D2::parameters(std::is_same<D1,D2>() ? "sub Decoder" : "sub Decoder 2", std::is_same<D1,D2>() ? "sub" : "sub2")),
  itl (new Interleaver   ::parameters("itl")),
  sf  (new Scaling_factor::parameters("sf" )),
  fnc (new Flip_and_check::parameters("fnc"))
{
	type   = "TURBO";
	implem = "FAST";

	type_set  .insert("TURBO");
	implem_set.insert({"STD", "FAST"});

	sub1->no_argflag(true);
	sub2->no_argflag(true);
	itl ->no_argflag(true);
	sf  ->no_argflag(true);
	fnc ->no_argflag(true);
}

template <class D1, class D2>
Decoder_turbo::parameters<D1,D2>* Decoder_turbo::parameters<D1,D2>
::clone() const
{
	return new Decoder_turbo::parameters<D1,D2>(*this);
}

template <class D1, class D2>
std::vector<std::string> Decoder_turbo::parameters<D1,D2>
::get_names() const
{
	auto n = Decoder::parameters::get_names();
	if (sf   != nullptr) { auto nn = sf  ->get_names(); for (auto &x : nn) n.push_back(x); }
	if (fnc  != nullptr) { auto nn = fnc ->get_names(); for (auto &x : nn) n.push_back(x); }
	if (sub1 != nullptr) { auto nn = sub1->get_names(); for (auto &x : nn) n.push_back(x); }
	if (!std::is_same<D1,D2>())
	if (sub2 != nullptr) { auto nn = sub2->get_names(); for (auto &x : nn) n.push_back(x); }
	if (itl  != nullptr) { auto nn = itl ->get_names(); for (auto &x : nn) n.push_back(x); }
	return n;
}

template <class D1, class D2>
std::vector<std::string> Decoder_turbo::parameters<D1,D2>
::get_short_names() const
{
	auto sn = Decoder::parameters::get_short_names();
	if (sf   != nullptr) { auto nn = sf  ->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	if (fnc  != nullptr) { auto nn = fnc ->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	if (sub1 != nullptr) { auto nn = sub1->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	if (!std::is_same<D1,D2>())
	if (sub2 != nullptr) { auto nn = sub2->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	if (itl  != nullptr) { auto nn = itl ->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	return sn;
}

template <class D1, class D2>
std::vector<std::string> Decoder_turbo::parameters<D1,D2>
::get_prefixes() const
{
	auto p = Decoder::parameters::get_prefixes();
	if (sf   != nullptr) { auto nn = sf  ->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	if (fnc  != nullptr) { auto nn = fnc ->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	if (sub1 != nullptr) { auto nn = sub1->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	if (!std::is_same<D1,D2>())
	if (sub2 != nullptr) { auto nn = sub2->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	if (itl  != nullptr) { auto nn = itl ->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	return p;
}

template <class D1, class D2>
void Decoder_turbo::parameters<D1,D2>
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

	CLI::add_flag(app, p, naf,
		"--sc",
		self_corrected,
		"Enable the self corrected decoder (requires \"crc --type\").")
		->group("Standard");

	CLI::add_flag(app, p, naf,
		"--json",
		enable_json,
		"Enable the json output trace.")
		->group("Standard");


	sf->register_arguments(app);

	CLI::remove_option(app, "--ite", sf->get_prefix(), sf->no_argflag());


	fnc->register_arguments(app);

	CLI::remove_option(app, "--size", fnc->get_prefix(), fnc->no_argflag());
	CLI::remove_option(app, "--fra" , fnc->get_prefix(), fnc->no_argflag());
	CLI::remove_option(app, "--ite" , fnc->get_prefix(), fnc->no_argflag());


	sub1->register_arguments(app);

	CLI::remove_option(app, "--info-bits", sub1->get_prefix(), sub1->no_argflag());
	CLI::remove_option(app, "--fra"      , sub1->get_prefix(), sub1->no_argflag());


	if (!std::is_same<D1,D2>())
	{
		sub2->register_arguments(app);

		CLI::remove_option(app, "--info-bits", sub2->get_prefix(), sub2->no_argflag());
		CLI::remove_option(app, "--fra"      , sub2->get_prefix(), sub2->no_argflag());
	}
}

template <class D1, class D2>
void Decoder_turbo::parameters<D1,D2>
::callback_arguments()
{
	Decoder::parameters::callback_arguments();

	sub1->K        = sub2->K        = K;
	sub1->n_frames = sub2->n_frames = n_frames;

	sub1->callback_arguments();

	if (!sub1->implem_option_set_by_user())
	{
		sub1->implem = "GENERIC";
	}

	if (std::is_same<D1,D2>())
	{
		sub2->not_buffered  = sub1->not_buffered;
		sub2->n_frames      = sub1->n_frames;
		sub2->tail_length   = sub1->tail_length;
		sub2->poly          = sub1->poly;
		sub2->standard      = sub1->standard;
		sub2->max           = sub1->max;
		sub2->simd_strategy = sub1->simd_strategy;
		sub2->implem        = sub1->implem;

		tail_length = 2 * sub1->tail_length;
		N_cw        = 2 * sub1->N_cw - K;
	}
	else
	{
		sub2->callback_arguments();
		tail_length = sub1->tail_length + sub2->tail_length;
		N_cw        = sub1->N_cw + sub2->N_cw - K;
	}

	R = (float)K / (float)N_cw;

	if (enable_json)
	{
		sub1->implem        = "GENERIC_JSON";
		sub2->implem        = "GENERIC_JSON";
		sub1->simd_strategy = "";
		sub2->simd_strategy = "";
	}

	if (itl != nullptr)
	{
		itl->core->size     = K;
		itl->core->n_frames = n_frames;

		itl->callback_arguments();

		if (!itl->core->type_option_set_by_user())
		{
			if (sub1->standard == "LTE")
				itl->core->type = "LTE";
			else if (sub1->standard == "CCSDS")
				itl->core->type = "CCSDS";
		}
	}

	sf->n_ite = n_ite;

	sf->callback_arguments();

	fnc->size     = K;
	fnc->n_frames = n_frames;
	fnc->n_ite    = n_ite;

	fnc->callback_arguments();
}

template <class D1, class D2>
void Decoder_turbo::parameters<D1,D2>
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_name();

	Decoder::parameters::get_headers(headers, full);

	if (itl != nullptr)
		itl->get_headers(headers, full);

	if (type != "ML" && type != "CHASE")
	{
		headers[p].push_back(std::make_pair("Num. of iterations (i)", std::to_string(n_ite)));
		if (tail_length && full)
			headers[p].push_back(std::make_pair("Tail length", std::to_string(tail_length)));
		headers[p].push_back(std::make_pair("Enable json", ((enable_json) ? "on" : "off")));
		headers[p].push_back(std::make_pair("Self-corrected", ((self_corrected) ? "on" : "off")));

		sf ->get_headers(headers, full);
		fnc->get_headers(headers, full);

		sub1->get_headers(headers, full);
		if (!std::is_same<D1,D2>())
			sub2->get_headers(headers, full);
	}
}

template <class D1, class D2>
template <typename B, typename Q>
module::Decoder_turbo<B,Q>* Decoder_turbo::parameters<D1,D2>
::build(const module::Interleaver<Q>  &itl,
              module::Decoder_SISO<Q> &siso_n,
              module::Decoder_SISO<Q> &siso_i,
              const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	if (type == "TURBO")
	{
		if (implem == "STD" ) return new module::Decoder_turbo_std <B,Q>(K, N_cw, n_ite, itl, siso_n, siso_i, !sub1->not_buffered);
		if (implem == "FAST") return new module::Decoder_turbo_fast<B,Q>(K, N_cw, n_ite, itl, siso_n, siso_i, !sub1->not_buffered);
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <class D1, class D2>
template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_turbo::parameters<D1,D2>
::build(const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	return Decoder::parameters::build<B,Q>(encoder);
}

template <typename B, typename Q, class D1, class D2>
module::Decoder_turbo<B,Q>* Decoder_turbo
::build(const parameters<D1,D2>       &params,
        const module::Interleaver<Q>  &itl,
              module::Decoder_SISO<Q> &siso_n,
              module::Decoder_SISO<Q> &siso_i,
              const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build<B,Q>(itl, siso_n, siso_i, encoder);
}

template <typename B, typename Q, class D1, class D2>
module::Decoder_SIHO<B,Q>* Decoder_turbo
::build(const parameters<D1,D2> &params, const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build<B,Q>(encoder);
}
}
}
