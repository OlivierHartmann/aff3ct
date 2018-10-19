#include "Module/Decoder/Generic/ML/Decoder_maximum_likelihood_std.hpp"
#include "Module/Decoder/Generic/ML/Decoder_maximum_likelihood_naive.hpp"
#include "Module/Decoder/Generic/Chase/Decoder_chase_std.hpp"

#include "Decoder.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Decoder_name   = "Decoder";
const std::string aff3ct::factory::Decoder_prefix = "dec";

Decoder::parameters
::parameters(const std::string &name, const std::string &prefix)
: Factory::parameters(name, Decoder_name, prefix)
{
}

Decoder::parameters* Decoder::parameters
::clone() const
{
	return new Decoder::parameters(*this);
}

void Decoder::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	CLI::add_option(app, p, naf,
		"-K,--info-bits",
		K,
		"Useful number of bit transmitted (information bits).")
		->required()
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_option(app, p, naf,
		"-N,--cw-size",
		N_cw,
		"The codeword size.")
		->required()
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_option(app, p, naf,
		"-F,--fra",
		n_frames,
		"Set the number of inter frame level to process.",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	type_option =
	CLI::add_set(app, p, naf,
		"-D,--type",
		type,
		type_set,
		"Select the algorithm you want to decode the codeword.",
		true)
		->group("Standard");

	implem_option =
	CLI::add_set(app, p, naf,
		"--implem",
		implem,
		implem_set,
		"Select the implementation of the algorithm to decode.",
		true)
		->group("Standard");

	CLI::add_flag(app, p, naf,
		"--hamming",
		hamming,
		"Enable the computation of the Hamming distance instead of the Euclidean distance in the ML/CHASE decoders.")
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--flips",
		flips,
		"Set the maximum number of flips in the CHASE decoder.",
		true)
		->group("Standard");
}


bool Decoder::parameters
::implem_option_set_by_user() const
{
	return implem_option->count() != 0;
}

bool Decoder::parameters
::type_option_set_by_user() const
{
	return type_option->count() != 0;
}

void Decoder::parameters
::callback_arguments()
{
	R = (float)K / (float)N_cw;
}

void Decoder::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_short_name();

	headers[p].push_back(std::make_pair("Type (D)",type));
	if (implem.size()) headers[p].push_back(std::make_pair("Implementation", implem));

	if (full) headers[p].push_back(std::make_pair("Info. bits (K)",    std::to_string(K)));
	if (full) headers[p].push_back(std::make_pair("Codeword size (N)", std::to_string(N_cw)));
	if (full) headers[p].push_back(std::make_pair("Code rate (R)",     std::to_string(R)));

	headers[p].push_back(std::make_pair("Systematic", ((not_systematic) ? "no" : "yes")));
	if (full) headers[p].push_back(std::make_pair("Inter frame level", std::to_string(n_frames)));
	if (type == "ML" || type == "CHASE")
		headers[p].push_back(std::make_pair("Distance", hamming ? "Hamming" : "Euclidean"));
	if (type == "CHASE")
		headers[p].push_back(std::make_pair("Max flips", std::to_string(flips)));
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder::parameters
::build(const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	if (encoder)
	{
		if (type == "ML")
		{
			if (implem == "STD"  ) return new module::Decoder_ML_std  <B,Q>(K, N_cw, *encoder, hamming, n_frames);
			if (implem == "NAIVE") return new module::Decoder_ML_naive<B,Q>(K, N_cw, *encoder, hamming, n_frames);
		}
		else if (type == "CHASE")
		{
			if (implem == "STD") return new module::Decoder_chase_std<B,Q>(K, N_cw, *encoder, flips, hamming, n_frames);
		}
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder::parameters::build<B_8 ,Q_8 >(const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder::parameters::build<B_16,Q_16>(const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder::parameters::build<B_32,Q_32>(const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder::parameters::build<B_64,Q_64>(const std::unique_ptr<module::Encoder<B_64>>&) const;
#else
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder::parameters::build<B,Q>(const std::unique_ptr<module::Encoder<B>>&) const;
#endif
// ==================================================================================== explicit template instantiation
