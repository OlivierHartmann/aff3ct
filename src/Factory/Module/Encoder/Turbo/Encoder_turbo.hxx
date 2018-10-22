#include "Tools/Exception/exception.hpp"

#include "Module/Encoder/Turbo/Encoder_turbo.hpp"
#include "Module/Encoder/Turbo/Encoder_turbo_legacy.hpp"

#include "Encoder_turbo.hpp"

namespace aff3ct
{
namespace factory
{

template <class E1, class E2>
Encoder_turbo::parameters<E1,E2>
::parameters(const std::string &prefix)
: Encoder_turbo::parameters<E1,E2>(Encoder_turbo_name, prefix)
{
}

template <class E1, class E2>
Encoder_turbo::parameters<E1,E2>
::parameters(const std::string &name, const std::string &prefix)
: Encoder::parameters(name, prefix),
  itl (new Interleaver::parameters("itl")),
  sub1(new typename E1::parameters(std::is_same<E1,E2>() ? "sub Encoder" : "sub Encoder 1", std::is_same<E1,E2>() ? "sub" : "sub1")),
  sub2(new typename E2::parameters(std::is_same<E1,E2>() ? "sub Encoder" : "sub Encoder 2", std::is_same<E1,E2>() ? "sub" : "sub2"))
{
	type = "TURBO";

	type_set.insert("TURBO");

	sub1->no_argflag(true);
	sub2->no_argflag(true);
	itl ->no_argflag(true);
}

template <class E1, class E2>
Encoder_turbo::parameters<E1,E2>* Encoder_turbo::parameters<E1,E2>
::clone() const
{
	return new Encoder_turbo::parameters<E1,E2>(*this);
}

template <class E1, class E2>
std::vector<std::string> Encoder_turbo::parameters<E1,E2>
::get_names() const
{
	auto n = Encoder::parameters::get_names();
	if (sub1 != nullptr) { auto nn = sub1->get_names(); for (auto &x : nn) n.push_back(x); }
	if (!std::is_same<E1,E2>())
	if (sub2 != nullptr) { auto nn = sub2->get_names(); for (auto &x : nn) n.push_back(x); }
	if (itl  != nullptr) { auto nn = itl ->get_names(); for (auto &x : nn) n.push_back(x); }
	return n;
}

template <class E1, class E2>
std::vector<std::string> Encoder_turbo::parameters<E1,E2>
::get_short_names() const
{
	auto sn = Encoder::parameters::get_short_names();
	if (sub1 != nullptr) { auto nn = sub1->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	if (!std::is_same<E1,E2>())
	if (sub2 != nullptr) { auto nn = sub2->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	if (itl  != nullptr) { auto nn = itl ->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	return sn;
}

template <class E1, class E2>
std::vector<std::string> Encoder_turbo::parameters<E1,E2>
::get_prefixes() const
{
	auto p = Encoder::parameters::get_prefixes();
	if (sub1 != nullptr) { auto nn = sub1->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	if (!std::is_same<E1,E2>())
	if (sub2 != nullptr) { auto nn = sub2->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	if (itl  != nullptr) { auto nn = itl ->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	return p;
}

template <class E1, class E2>
void Encoder_turbo::parameters<E1,E2>
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

	sub1->register_arguments(app);

	CLI::remove_option(app, "--info-bits", sub1->get_prefix(), sub1->no_argflag());
	CLI::remove_option(app, "--fra"      , sub1->get_prefix(), sub1->no_argflag());
	CLI::remove_option(app, "--seed"     , sub1->get_prefix(), sub1->no_argflag());
	CLI::remove_option(app, "--path"     , sub1->get_prefix(), sub1->no_argflag());

	if (!std::is_same<E1,E2>())
	{
		sub2->register_arguments(app);

		CLI::remove_option(app, "--info-bits", sub2->get_prefix(), sub2->no_argflag());
		CLI::remove_option(app, "--fra"      , sub2->get_prefix(), sub2->no_argflag());
		CLI::remove_option(app, "--seed"     , sub2->get_prefix(), sub2->no_argflag());
		CLI::remove_option(app, "--path"     , sub2->get_prefix(), sub2->no_argflag());
	}
}

template <class E1, class E2>
void Encoder_turbo::parameters<E1,E2>
::callback_arguments()
{
	Encoder::parameters::callback_arguments();

	auto p = get_prefix();

	sub1->K        = sub2->K        = K;
	sub1->n_frames = sub2->n_frames = n_frames;
	sub1->seed     = sub2->seed     = seed;

	sub1->callback_arguments();
	sub2->callback_arguments();

	if (!json_path.empty())
	{
		sub1->type += "_JSON";
		sub2->type += "_JSON";
	}

	tail_length = sub1->tail_length + sub2->tail_length;
	N_cw        = sub1->N_cw + sub2->N_cw - K;
	R           = (float)K / (float)N_cw;

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
}

template <class E1, class E2>
void Encoder_turbo::parameters<E1,E2>
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_name();

	Encoder::parameters::get_headers(headers, full);

	if (itl != nullptr)
		itl->get_headers(headers);

	if (tail_length)
		headers[p].push_back(std::make_pair("Tail length", std::to_string(tail_length)));

	if (!json_path.empty())
		headers[p].push_back(std::make_pair("Path to the JSON file", json_path));

	sub1->get_headers(headers, full);
	if (!std::is_same<E1,E2>())
		sub2->get_headers(headers, full);
}

template <class E1, class E2>
template <typename B>
module::Encoder_turbo<B>* Encoder_turbo::parameters<E1,E2>
::build(const module::Interleaver<B> &itl,
              std::shared_ptr<module::Encoder<B>> enc_n,
              std::shared_ptr<module::Encoder<B>> enc_i) const
{
	enc_i = (enc_i == nullptr) ? enc_n : enc_i;

	if (!sub1->not_buffered)
	{
		if (type == "TURBO") return new module::Encoder_turbo       <B>(K, N_cw, itl, *enc_n, *enc_i);
	}
	else if (enc_n == enc_i)
	{
		if (type == "TURBO") return new module::Encoder_turbo_legacy<B>(K, N_cw, itl, *enc_n);
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, class E1, class E2>
module::Encoder_turbo<B>* Encoder_turbo
::build(const parameters<E1,E2>      &params,
        const module::Interleaver<B> &itl,
              std::shared_ptr<module::Encoder<B>> enc_n,
              std::shared_ptr<module::Encoder<B>> enc_i)
{
	return params.template build<B>(itl, enc_n, enc_i);
}
}
}
