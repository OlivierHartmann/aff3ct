#include <sstream>

#include "Module/Decoder/RS/Standard/Decoder_RS_std.hpp"
#include "Module/Decoder/RS/Genius/Decoder_RS_genius.hpp"

#include "Tools/Exception/exception.hpp"

#include "Decoder_RS.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Decoder_RS_name   = "Decoder RS";
const std::string aff3ct::factory::Decoder_RS_prefix = "dec";

Decoder_RS::parameters
::parameters(const std::string &prefix)
: Decoder::parameters(Decoder_RS_name, prefix)
{
	type   = "ALGEBRAIC";
	implem = "STD";

	type_set  .insert("ALGEBRAIC");
	implem_set.insert("STD");
}

Decoder_RS::parameters* Decoder_RS::parameters
::clone() const
{
	return new Decoder_RS::parameters(*this);
}

void Decoder_RS::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	Decoder::parameters::register_arguments(app);

	CLI::add_option(app, p, naf,
		"-T,--corr-pow",
		t,
		"Correction power of the RS code.")
		->check(CLI::StrictlyPositiveRange(0u))
		// ->excludes(app.get_option("--info-bits"))
		->group("Standard");
}

void Decoder_RS::parameters
::callback_arguments()
{
	Decoder::parameters::callback_arguments();

	auto p = get_prefix();

	m = (int)std::ceil(std::log2(N_cw));
	if (m == 0)
	{
		std::stringstream message;
		message << "The Galois Field order is null (because N_cw = " << N_cw << ").";
		throw tools::runtime_error(__FILE__, __LINE__, __func__, message.str());
	}

	if (t != 0)
	{
		if (K == 0)
		{
			K = N_cw - t * 2;
			R = (float) K / (float) N_cw;
		}
	}
	else
		t = (N_cw - K) / 2;
}

void Decoder_RS::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_short_name();

	Decoder::parameters::get_headers(headers, full);

	if (type != "ML" && type != "CHASE")
	{
		headers[p].push_back(std::make_pair("Galois field order (m)", std::to_string(m)));
		headers[p].push_back(std::make_pair("Correction power (T)",   std::to_string(t)));
	}
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_RS::parameters
::build(const tools::RS_polynomial_generator &GF, const std::unique_ptr<module::Encoder<B>>& encoder) const
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
module::Decoder_SIHO<B,Q>* Decoder_RS
::build(const parameters &params, const tools::RS_polynomial_generator &GF, const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build<B,Q>(GF, encoder);
}

template <typename B, typename Q>
module::Decoder_SIHO_HIHO<B,Q>* Decoder_RS::parameters
::build_hiho(const tools::RS_polynomial_generator &GF, const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	if (type == "ALGEBRAIC")
	{
		if (implem == "STD") return new module::Decoder_RS_std<B,Q>(K, N_cw, GF, n_frames);

		if (encoder)
		{
			if (implem == "GENIUS") return new module::Decoder_RS_genius<B,Q>(K, N_cw, GF, *encoder, n_frames);
		}
	}


	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SIHO_HIHO<B,Q>* Decoder_RS
::build_hiho(const parameters &params, const tools::RS_polynomial_generator &GF, const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build_hiho<B,Q>(GF, encoder);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_RS::parameters::build<B_8 ,Q_8 >(const aff3ct::tools::RS_polynomial_generator&, const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_RS::parameters::build<B_16,Q_16>(const aff3ct::tools::RS_polynomial_generator&, const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_RS::parameters::build<B_32,Q_32>(const aff3ct::tools::RS_polynomial_generator&, const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_RS::parameters::build<B_64,Q_64>(const aff3ct::tools::RS_polynomial_generator&, const std::unique_ptr<module::Encoder<B_64>>&) const;
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_RS::build<B_8 ,Q_8 >(const aff3ct::factory::Decoder_RS::parameters&, const aff3ct::tools::RS_polynomial_generator&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_RS::build<B_16,Q_16>(const aff3ct::factory::Decoder_RS::parameters&, const aff3ct::tools::RS_polynomial_generator&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_RS::build<B_32,Q_32>(const aff3ct::factory::Decoder_RS::parameters&, const aff3ct::tools::RS_polynomial_generator&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_RS::build<B_64,Q_64>(const aff3ct::factory::Decoder_RS::parameters&, const aff3ct::tools::RS_polynomial_generator&, const std::unique_ptr<module::Encoder<B_64>>&);
#else
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_RS::parameters::build<B,Q>(const aff3ct::tools::RS_polynomial_generator&, const std::unique_ptr<module::Encoder<B>>& ) const;
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_RS::build<B,Q>(const aff3ct::factory::Decoder_RS::parameters&, const aff3ct::tools::RS_polynomial_generator&, const std::unique_ptr<module::Encoder<B>>& );
#endif
// ==================================================================================== explicit template instantiation


// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Decoder_SIHO_HIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_RS::parameters::build_hiho<B_8 ,Q_8 >(const aff3ct::tools::RS_polynomial_generator&, const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_SIHO_HIHO<B_16,Q_16>* aff3ct::factory::Decoder_RS::parameters::build_hiho<B_16,Q_16>(const aff3ct::tools::RS_polynomial_generator&, const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_SIHO_HIHO<B_32,Q_32>* aff3ct::factory::Decoder_RS::parameters::build_hiho<B_32,Q_32>(const aff3ct::tools::RS_polynomial_generator&, const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_SIHO_HIHO<B_64,Q_64>* aff3ct::factory::Decoder_RS::parameters::build_hiho<B_64,Q_64>(const aff3ct::tools::RS_polynomial_generator&, const std::unique_ptr<module::Encoder<B_64>>&) const;
template aff3ct::module::Decoder_SIHO_HIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_RS::build_hiho<B_8 ,Q_8 >(const aff3ct::factory::Decoder_RS::parameters&, const aff3ct::tools::RS_polynomial_generator&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SIHO_HIHO<B_16,Q_16>* aff3ct::factory::Decoder_RS::build_hiho<B_16,Q_16>(const aff3ct::factory::Decoder_RS::parameters&, const aff3ct::tools::RS_polynomial_generator&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SIHO_HIHO<B_32,Q_32>* aff3ct::factory::Decoder_RS::build_hiho<B_32,Q_32>(const aff3ct::factory::Decoder_RS::parameters&, const aff3ct::tools::RS_polynomial_generator&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SIHO_HIHO<B_64,Q_64>* aff3ct::factory::Decoder_RS::build_hiho<B_64,Q_64>(const aff3ct::factory::Decoder_RS::parameters&, const aff3ct::tools::RS_polynomial_generator&, const std::unique_ptr<module::Encoder<B_64>>&);
#else
template aff3ct::module::Decoder_SIHO_HIHO<B,Q>* aff3ct::factory::Decoder_RS::parameters::build_hiho<B>(const aff3ct::tools::RS_polynomial_generator&, const std::unique_ptr<module::Encoder<B>>& ) const;
template aff3ct::module::Decoder_SIHO_HIHO<B,Q>* aff3ct::factory::Decoder_RS::build_hiho<B>(const aff3ct::factory::Decoder_RS::parameters&, const aff3ct::tools::RS_polynomial_generator&, const std::unique_ptr<module::Encoder<B>>& );
#endif
// ==================================================================================== explicit template instantiation

