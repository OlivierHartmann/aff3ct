#include "Tools/Exception/exception.hpp"
#include "Tools/general_utils.h"
#include "Tools/Arguments/Splitter/Splitter.hpp"

#include "Module/Decoder/Turbo_product/Chase_pyndiah/Decoder_chase_pyndiah.hpp"
#include "Module/Decoder/Turbo_product/Decoder_turbo_product.hpp"

#include "Decoder_turbo_product.hpp"

const std::string aff3ct::factory::Decoder_turbo_product_name   = "Decoder Turbo Product Code";
const std::string aff3ct::factory::Decoder_turbo_product_prefix = "dec";

using namespace aff3ct;
using namespace aff3ct::factory;

Decoder_turbo_product::parameters
::parameters(const std::string &prefix)
: Decoder_turbo_product::parameters(Decoder_turbo_product_name, prefix)
{
}

Decoder_turbo_product::parameters
::parameters(const std::string &name, const std::string &prefix)
: Decoder::parameters(name, prefix),
  sub(new Decoder_BCH::parameters("sub")),
  itl(new Interleaver::parameters("itl"))
{
	type   = "CP";
	implem = "STD";

	type_set  .insert("CP");
	implem_set.insert("FAST");

	sub->no_argflag(true);
	itl->no_argflag(true);
}

Decoder_turbo_product::parameters* Decoder_turbo_product::parameters
::clone() const
{
	return new Decoder_turbo_product::parameters(*this);
}

std::vector<std::string> Decoder_turbo_product::parameters
::get_names() const
{
	auto n = Decoder::parameters::get_names();
	if (sub != nullptr) { auto nn = sub->get_names(); for (auto &x : nn) n.push_back(x); }
	if (itl != nullptr) { auto nn = itl->get_names(); for (auto &x : nn) n.push_back(x); }
	return n;
}

std::vector<std::string> Decoder_turbo_product::parameters
::get_short_names() const
{
	auto sn = Decoder::parameters::get_short_names();
	if (sub != nullptr) { auto nn = sub->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	if (itl != nullptr) { auto nn = itl->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	return sn;
}

std::vector<std::string> Decoder_turbo_product::parameters
::get_prefixes() const
{
	auto p = Decoder::parameters::get_prefixes();
	if (sub != nullptr) { auto nn = sub->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	if (itl != nullptr) { auto nn = itl->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	return p;
}

struct Real_splitter
{
	static std::vector<std::string> split(const std::string& val)
	{
		const std::string head      = "{([";
		const std::string queue     = "})]";
		const std::string separator = ",";

		return tools::Splitter::split(val, head, queue, separator);
	}
};

void Decoder_turbo_product::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	Decoder::parameters::register_arguments(app);

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
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--alpha",
		alpha,
		"Weighting factor, one by half iteration (so twice more than number of iterations). "
		"If not enough given values, then automatically extends the last to all iterations.")
		->check(CLI::VectorLength(1,0))
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--beta",
		beta,
		"Reliability factor, one by half iteration (so twice more than number of iterations). "
		"If not enough given values, then automatically extends the last to all iterations. "
		"If not given, then computes beta dynamically from the least reliable position metrics.")
		->check(CLI::VectorLength(1,0))
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--pos",
		n_least_reliable_positions,
		"Number of least reliable positions.",
		true)
		->check(CLI::StrictlyPositiveRange(16u))
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--tv",
		n_test_vectors,
		"Number of test vectors (0 means equal to 2^pos).",
		true)
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--cp",
		n_competitors,
		"Number of competitors (0 means equal to number of test vectors, 1 means only the decided word).",
		true)
		->group("Standard");

	CLI::add_flag(app, p, naf,
		"--ext",
		parity_extended,
		"Considers an extended code with a parity bits.")
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--cp-coef",
		cp_coef,
		"The 5 Chase Pyndiah constant coefficients \"a,b,c,d,e\".",
		true)
		->check(CLI::VectorLength(5,5))
		->group("Standard");


	sub->type_set   = {"ALGEBRAIC"};
	sub->implem_set = {"STD", "FAST"};
	sub->register_arguments(app);

	CLI::remove_option(app, "--info-bits", sub->get_prefix(), sub->no_argflag());
	CLI::remove_option(app, "--cw-size",   sub->get_prefix(), sub->no_argflag());
	CLI::remove_option(app, "--fra",       sub->get_prefix(), sub->no_argflag());
	CLI::remove_option(app, "--hamming",   sub->get_prefix(), sub->no_argflag());
	CLI::remove_option(app, "--flips",     sub->get_prefix(), sub->no_argflag());
}

void Decoder_turbo_product::parameters
::callback_arguments()
{
	auto p = get_name();

	Decoder::parameters::callback_arguments();

	if (n_test_vectors == 0)
		n_test_vectors = 1 << n_least_reliable_positions;

	if (vals.exist({p+"-c"}))
		n_competitors = vals.to_int({p+"-c"});
	else
		n_competitors = n_test_vectors;


	alpha.resize(n_ite*2, alpha.back());

	if (!beta.empty())
		beta.resize(n_ite*2, beta.back());


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

void Decoder_turbo_product::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_name();

	Decoder::parameters::get_headers(headers, full);

	if (type != "ML")
	{
		if (itl != nullptr)
			itl->get_headers(headers, full);

		headers[p].push_back(std::make_pair("Num. of iterations (i)", std::to_string(n_ite)));

		std::stringstream alpha_str;
		alpha_str << "{";
		for (unsigned i = 0; i < alpha.size(); i++)
			alpha_str << alpha[i] << (i == alpha.size()-1 ?"":", ");
		alpha_str << "}";

		headers[p].push_back(std::make_pair("alpha", alpha_str.str()));

		if (beta.size())
		{
			std::stringstream beta_str;
			beta_str << "{";
			for (unsigned i = 0; i < beta.size(); i++)
				beta_str << beta[i] << (i == beta.size()-1 ?"":", ");
			beta_str << "}";

			headers[p].push_back(std::make_pair("beta", beta_str.str()));
		}

		std::stringstream cp_coeff_str;
		cp_coeff_str << "{";
		for (unsigned i = 0; i < cp_coef.size(); i++)
			cp_coeff_str << cp_coef[i] << (i == cp_coef.size()-1 ?"":", ");
		cp_coeff_str << "}";

		headers[p].push_back(std::make_pair("Chase Pyndiah coeff.", cp_coeff_str.str()));

		headers[p].push_back(std::make_pair("Num. least reliable pos (p)", std::to_string(n_least_reliable_positions)));

		headers[p].push_back(std::make_pair("Num. test vectors (t)", std::to_string(n_test_vectors)));

		headers[p].push_back(std::make_pair("Num. competitors (c)", std::to_string(n_competitors)));

		headers[p].push_back(std::make_pair("Parity extended", (parity_extended ? "yes" : "no")));

		sub->get_headers(headers, full);
	}
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_turbo_product::parameters
::build(const module::Interleaver <Q> &itl,
              module::Decoder_chase_pyndiah<B,Q> &cp_r,
              module::Decoder_chase_pyndiah<B,Q> &cp_c,
              const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	try
	{
		return Decoder::parameters::build<B,Q>(encoder);
	}
	catch (tools::cannot_allocate const&)
	{
		if (type == "CP")
		{
			if (implem == "STD")
				return new module::Decoder_turbo_product<B,Q>(n_ite, alpha, itl, cp_r, cp_c, beta, n_frames);
		}
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SISO_SIHO<B,Q>* Decoder_turbo_product::parameters
::build_siso(const module::Interleaver <Q> &itl,
                   module::Decoder_chase_pyndiah<B,Q> &cp_r,
                   module::Decoder_chase_pyndiah<B,Q> &cp_c) const
{
	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_turbo_product
::build(const parameters              &params,
        const module::Interleaver <Q> &itl,
              module::Decoder_chase_pyndiah<B,Q> &cp_r,
              module::Decoder_chase_pyndiah<B,Q> &cp_c,
              const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build<B,Q>(itl, cp_r, cp_c, encoder);
}

template <typename B, typename Q>
module::Decoder_SISO_SIHO<B,Q>* Decoder_turbo_product
::build_siso(const parameters              &params,
             const module::Interleaver <Q> &itl,
                   module::Decoder_chase_pyndiah<B,Q> &cp_r,
                   module::Decoder_chase_pyndiah<B,Q> &cp_c)
{
	return params.template build_siso<B,Q>(itl, cp_r, cp_c);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_turbo_product::parameters::build<B_8 ,Q_8 >(const aff3ct::module::Interleaver<Q_8 >&, aff3ct::module::Decoder_chase_pyndiah<B_8 ,Q_8 > &, aff3ct::module::Decoder_chase_pyndiah<B_8 ,Q_8 > &, const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_turbo_product::parameters::build<B_16,Q_16>(const aff3ct::module::Interleaver<Q_16>&, aff3ct::module::Decoder_chase_pyndiah<B_16,Q_16> &, aff3ct::module::Decoder_chase_pyndiah<B_16,Q_16> &, const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_turbo_product::parameters::build<B_32,Q_32>(const aff3ct::module::Interleaver<Q_32>&, aff3ct::module::Decoder_chase_pyndiah<B_32,Q_32> &, aff3ct::module::Decoder_chase_pyndiah<B_32,Q_32> &, const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_turbo_product::parameters::build<B_64,Q_64>(const aff3ct::module::Interleaver<Q_64>&, aff3ct::module::Decoder_chase_pyndiah<B_64,Q_64> &, aff3ct::module::Decoder_chase_pyndiah<B_64,Q_64> &, const std::unique_ptr<module::Encoder<B_64>>&) const;
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_turbo_product::build<B_8 ,Q_8 >(const aff3ct::factory::Decoder_turbo_product::parameters&, const aff3ct::module::Interleaver<Q_8 >&, aff3ct::module::Decoder_chase_pyndiah<B_8 ,Q_8 > &, aff3ct::module::Decoder_chase_pyndiah<B_8 ,Q_8 > &, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_turbo_product::build<B_16,Q_16>(const aff3ct::factory::Decoder_turbo_product::parameters&, const aff3ct::module::Interleaver<Q_16>&, aff3ct::module::Decoder_chase_pyndiah<B_16,Q_16> &, aff3ct::module::Decoder_chase_pyndiah<B_16,Q_16> &, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_turbo_product::build<B_32,Q_32>(const aff3ct::factory::Decoder_turbo_product::parameters&, const aff3ct::module::Interleaver<Q_32>&, aff3ct::module::Decoder_chase_pyndiah<B_32,Q_32> &, aff3ct::module::Decoder_chase_pyndiah<B_32,Q_32> &, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_turbo_product::build<B_64,Q_64>(const aff3ct::factory::Decoder_turbo_product::parameters&, const aff3ct::module::Interleaver<Q_64>&, aff3ct::module::Decoder_chase_pyndiah<B_64,Q_64> &, aff3ct::module::Decoder_chase_pyndiah<B_64,Q_64> &, const std::unique_ptr<module::Encoder<B_64>>&);
#else
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_turbo_product::parameters::build<B,Q>(const aff3ct::module::Interleaver<Q>&, aff3ct::module::Decoder_chase_pyndiah<B,Q> &, aff3ct::module::Decoder_chase_pyndiah<B,Q> &, const std::unique_ptr<module::Encoder<B>>& ) const;
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_turbo_product::build<B,Q>(const aff3ct::factory::Decoder_turbo_product::parameters&, const aff3ct::module::Interleaver<Q>&, aff3ct::module::Decoder_chase_pyndiah<B,Q> &, aff3ct::module::Decoder_chase_pyndiah<B,Q> &, const std::unique_ptr<module::Encoder<B>>& );
#endif

#ifdef MULTI_PREC
template aff3ct::module::Decoder_SISO_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_turbo_product::parameters::build_siso<B_8 ,Q_8 >(const aff3ct::module::Interleaver<Q_8 >&, aff3ct::module::Decoder_chase_pyndiah<B_8 ,Q_8 > &, aff3ct::module::Decoder_chase_pyndiah<B_8 ,Q_8 > &) const;
template aff3ct::module::Decoder_SISO_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_turbo_product::parameters::build_siso<B_16,Q_16>(const aff3ct::module::Interleaver<Q_16>&, aff3ct::module::Decoder_chase_pyndiah<B_16,Q_16> &, aff3ct::module::Decoder_chase_pyndiah<B_16,Q_16> &) const;
template aff3ct::module::Decoder_SISO_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_turbo_product::parameters::build_siso<B_32,Q_32>(const aff3ct::module::Interleaver<Q_32>&, aff3ct::module::Decoder_chase_pyndiah<B_32,Q_32> &, aff3ct::module::Decoder_chase_pyndiah<B_32,Q_32> &) const;
template aff3ct::module::Decoder_SISO_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_turbo_product::parameters::build_siso<B_64,Q_64>(const aff3ct::module::Interleaver<Q_64>&, aff3ct::module::Decoder_chase_pyndiah<B_64,Q_64> &, aff3ct::module::Decoder_chase_pyndiah<B_64,Q_64> &) const;
template aff3ct::module::Decoder_SISO_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_turbo_product::build_siso<B_8 ,Q_8 >(const aff3ct::factory::Decoder_turbo_product::parameters&, const aff3ct::module::Interleaver<Q_8 >&, aff3ct::module::Decoder_chase_pyndiah<B_8 ,Q_8 > &, aff3ct::module::Decoder_chase_pyndiah<B_8 ,Q_8 > &);
template aff3ct::module::Decoder_SISO_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_turbo_product::build_siso<B_16,Q_16>(const aff3ct::factory::Decoder_turbo_product::parameters&, const aff3ct::module::Interleaver<Q_16>&, aff3ct::module::Decoder_chase_pyndiah<B_16,Q_16> &, aff3ct::module::Decoder_chase_pyndiah<B_16,Q_16> &);
template aff3ct::module::Decoder_SISO_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_turbo_product::build_siso<B_32,Q_32>(const aff3ct::factory::Decoder_turbo_product::parameters&, const aff3ct::module::Interleaver<Q_32>&, aff3ct::module::Decoder_chase_pyndiah<B_32,Q_32> &, aff3ct::module::Decoder_chase_pyndiah<B_32,Q_32> &);
template aff3ct::module::Decoder_SISO_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_turbo_product::build_siso<B_64,Q_64>(const aff3ct::factory::Decoder_turbo_product::parameters&, const aff3ct::module::Interleaver<Q_64>&, aff3ct::module::Decoder_chase_pyndiah<B_64,Q_64> &, aff3ct::module::Decoder_chase_pyndiah<B_64,Q_64> &);
#else
template aff3ct::module::Decoder_SISO_SIHO<B,Q>* aff3ct::factory::Decoder_turbo_product::parameters::build_siso<B,Q>(const aff3ct::module::Interleaver<Q>&, aff3ct::module::Decoder_chase_pyndiah<B,Q> &, aff3ct::module::Decoder_chase_pyndiah<B,Q> &) const;
template aff3ct::module::Decoder_SISO_SIHO<B,Q>* aff3ct::factory::Decoder_turbo_product::build_siso<B,Q>(const aff3ct::factory::Decoder_turbo_product::parameters&, const aff3ct::module::Interleaver<Q>&, aff3ct::module::Decoder_chase_pyndiah<B,Q> &, aff3ct::module::Decoder_chase_pyndiah<B,Q> &);
#endif
// ==================================================================================== explicit template instantiation
