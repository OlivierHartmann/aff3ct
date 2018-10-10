#include <sstream>

#include "Module/Decoder/BCH/Standard/Decoder_BCH_std.hpp"
#include "Module/Decoder/BCH/Genius/Decoder_BCH_genius.hpp"
#include "Module/Decoder/BCH/Fast/Decoder_BCH_fast.hpp"

#include "Tools/Exception/exception.hpp"

#include "Decoder_BCH.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Decoder_BCH_name   = "Decoder BCH";
const std::string aff3ct::factory::Decoder_BCH_prefix = "dec";

Decoder_BCH::parameters
::parameters(const std::string &prefix)
: Decoder::parameters(Decoder_BCH_name, prefix)
{
	type   = "ALGEBRAIC";
	implem = "STD";
}

Decoder_BCH::parameters* Decoder_BCH::parameters
::clone() const
{
	return new Decoder_BCH::parameters(*this);
}

void Decoder_BCH::parameters
::register_arguments(CLI::App &app)
{
	Decoder::parameters::register_arguments(app);

	auto sub = CLI::make_subcommand(app, get_prefix(), get_name() + " parameters");

	sub->add_option(
		"-T,--corr-pow",
		t,
		"Correction power of the BCH code.")
		->check(CLI::StrictlyPositiveRange(0u))
		->excludes(sub->get_option("--info-bits"))
		->group("Standard");

	type_set  .insert("ALGEBRAIC");
	implem_set.insert({"GENIUS", "FAST"});
}

void Decoder_BCH::parameters
::callback_arguments()
{
	Decoder::parameters::callback_arguments();

	if (N_cw == 0)
	{
		std::stringstream message;
		message << "The size of the codeword 'N_cw' is null.";
		throw tools::runtime_error(__FILE__, __LINE__, __func__, message.str());
	}

	m = (int)std::ceil(std::log2((float)N_cw));
	if (m == 0)
	{
		std::stringstream message;
		message << "The Galois Field order 'm' is null (because 'N_cw' = " << N_cw << ").";
		throw tools::runtime_error(__FILE__, __LINE__, __func__, message.str());
	}

	if (t != 0)
	{
		if (K == 0)
		{
			K = N_cw - tools::BCH_polynomial_generator<>(N_cw, t).get_n_rdncy();
			R = (float) K / (float) N_cw;
		}
	}
	else
		t = (N_cw - K) / m;
}

void Decoder_BCH::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	Decoder::parameters::get_headers(headers, full);

	if (type != "ML" && type != "CHASE")
	{
		auto p = get_prefix();

		headers[p].push_back(std::make_pair("Galois field order (m)", std::to_string(m)));
		headers[p].push_back(std::make_pair("Correction power (T)",   std::to_string(t)));
	}
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_BCH::parameters
::build(const tools::BCH_polynomial_generator<B> &GF, const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	try
	{
		return Decoder::parameters::build<B,Q>(encoder);
	}
	catch (tools::cannot_allocate const&)
	{
		return build_hiho<B,Q>(GF, encoder);
	}
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_BCH
::build(const parameters &params, const tools::BCH_polynomial_generator<B> &GF, const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build<B,Q>(GF, encoder);
}

template <typename B, typename Q>
module::Decoder_SIHO_HIHO<B,Q>* Decoder_BCH::parameters
::build_hiho(const tools::BCH_polynomial_generator<B> &GF, const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	if (type == "ALGEBRAIC")
	{
		if (implem == "STD" ) return new module::Decoder_BCH_std <B,Q>(K, N_cw, GF, n_frames);
		if (implem == "FAST") return new module::Decoder_BCH_fast<B,Q>(K, N_cw, GF, n_frames);

		if (encoder)
		{
			if (implem == "GENIUS") return new module::Decoder_BCH_genius<B,Q>(K, N_cw, t, *encoder, n_frames);
		}
	}


	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SIHO_HIHO<B,Q>* Decoder_BCH
::build_hiho(const parameters &params, const tools::BCH_polynomial_generator<B> &GF, const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build_hiho<B,Q>(GF, encoder);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_BCH::parameters::build<B_8 ,Q_8 >(const aff3ct::tools::BCH_polynomial_generator<B_8 >&, const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_BCH::parameters::build<B_16,Q_16>(const aff3ct::tools::BCH_polynomial_generator<B_16>&, const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_BCH::parameters::build<B_32,Q_32>(const aff3ct::tools::BCH_polynomial_generator<B_32>&, const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_BCH::parameters::build<B_64,Q_64>(const aff3ct::tools::BCH_polynomial_generator<B_64>&, const std::unique_ptr<module::Encoder<B_64>>&) const;
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_BCH::build<B_8 ,Q_8 >(const aff3ct::factory::Decoder_BCH::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_8 >&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_BCH::build<B_16,Q_16>(const aff3ct::factory::Decoder_BCH::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_16>&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_BCH::build<B_32,Q_32>(const aff3ct::factory::Decoder_BCH::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_32>&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_BCH::build<B_64,Q_64>(const aff3ct::factory::Decoder_BCH::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_64>&, const std::unique_ptr<module::Encoder<B_64>>&);
#else
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_BCH::parameters::build<B,Q>(const aff3ct::tools::BCH_polynomial_generator<B>&, const std::unique_ptr<module::Encoder<B>>& ) const;
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_BCH::build<B,Q>(const aff3ct::factory::Decoder_BCH::parameters&, const aff3ct::tools::BCH_polynomial_generator<B>&, const std::unique_ptr<module::Encoder<B>>& );
#endif
// ==================================================================================== explicit template instantiation


// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Decoder_SIHO_HIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_BCH::parameters::build_hiho<B_8 ,Q_8 >(const aff3ct::tools::BCH_polynomial_generator<B_8 >&, const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_SIHO_HIHO<B_16,Q_16>* aff3ct::factory::Decoder_BCH::parameters::build_hiho<B_16,Q_16>(const aff3ct::tools::BCH_polynomial_generator<B_16>&, const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_SIHO_HIHO<B_32,Q_32>* aff3ct::factory::Decoder_BCH::parameters::build_hiho<B_32,Q_32>(const aff3ct::tools::BCH_polynomial_generator<B_32>&, const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_SIHO_HIHO<B_64,Q_64>* aff3ct::factory::Decoder_BCH::parameters::build_hiho<B_64,Q_64>(const aff3ct::tools::BCH_polynomial_generator<B_64>&, const std::unique_ptr<module::Encoder<B_64>>&) const;
template aff3ct::module::Decoder_SIHO_HIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_BCH::build_hiho<B_8 ,Q_8 >(const aff3ct::factory::Decoder_BCH::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_8 >&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SIHO_HIHO<B_16,Q_16>* aff3ct::factory::Decoder_BCH::build_hiho<B_16,Q_16>(const aff3ct::factory::Decoder_BCH::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_16>&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SIHO_HIHO<B_32,Q_32>* aff3ct::factory::Decoder_BCH::build_hiho<B_32,Q_32>(const aff3ct::factory::Decoder_BCH::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_32>&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SIHO_HIHO<B_64,Q_64>* aff3ct::factory::Decoder_BCH::build_hiho<B_64,Q_64>(const aff3ct::factory::Decoder_BCH::parameters&, const aff3ct::tools::BCH_polynomial_generator<B_64>&, const std::unique_ptr<module::Encoder<B_64>>&);
#else
template aff3ct::module::Decoder_SIHO_HIHO<B,Q>* aff3ct::factory::Decoder_BCH::parameters::build_hiho<B>(const aff3ct::tools::BCH_polynomial_generator<B>&, const std::unique_ptr<module::Encoder<B>>& ) const;
template aff3ct::module::Decoder_SIHO_HIHO<B,Q>* aff3ct::factory::Decoder_BCH::build_hiho<B>(const aff3ct::factory::Decoder_BCH::parameters&, const aff3ct::tools::BCH_polynomial_generator<B>&, const std::unique_ptr<module::Encoder<B>>& );
#endif
// ==================================================================================== explicit template instantiation

