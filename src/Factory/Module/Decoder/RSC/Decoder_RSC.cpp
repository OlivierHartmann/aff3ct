#include <type_traits>
#include <mipp.h>

#include "Tools/Exception/exception.hpp"

#include "Module/Decoder/RSC/BCJR/Seq/Decoder_RSC_BCJR_seq_std.hpp"
#include "Module/Decoder/RSC/BCJR/Seq/Decoder_RSC_BCJR_seq_scan.hpp"
#include "Module/Decoder/RSC/BCJR/Seq/Decoder_RSC_BCJR_seq_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Seq/Decoder_RSC_BCJR_seq_very_fast.hpp"

#include "Module/Decoder/RSC/BCJR/Seq_generic/Decoder_RSC_BCJR_seq_generic_std.hpp"
#include "Module/Decoder/RSC/BCJR/Seq_generic/Decoder_RSC_BCJR_seq_generic_std_json.hpp"

#include "Module/Decoder/RSC/BCJR/Intra/Decoder_RSC_BCJR_intra_std.hpp"
#include "Module/Decoder/RSC/BCJR/Intra/Decoder_RSC_BCJR_intra_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Inter_intra/Decoder_RSC_BCJR_inter_intra_fast_x2_SSE.hpp"
#include "Module/Decoder/RSC/BCJR/Inter_intra/Decoder_RSC_BCJR_inter_intra_fast_x2_AVX.hpp"
#include "Module/Decoder/RSC/BCJR/Inter_intra/Decoder_RSC_BCJR_inter_intra_fast_x4_AVX.hpp"

#include "Module/Decoder/RSC/BCJR/Inter/Decoder_RSC_BCJR_inter_std.hpp"
#include "Module/Decoder/RSC/BCJR/Inter/Decoder_RSC_BCJR_inter_fast.hpp"
#include "Module/Decoder/RSC/BCJR/Inter/Decoder_RSC_BCJR_inter_very_fast.hpp"

#include "Decoder_RSC.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Decoder_RSC_name   = "Decoder RSC";
const std::string aff3ct::factory::Decoder_RSC_prefix = "dec";

Decoder_RSC::parameters
::parameters(const std::string &prefix)
: Decoder_RSC::parameters(Decoder_RSC_name, prefix)
{
}

Decoder_RSC::parameters
::parameters(const std::string &name, const std::string &prefix)
: Decoder::parameters(name, prefix)
{
	type   = "BCJR";
	implem = "STD";

	type_set  .insert({"BCJR"});
	implem_set.insert({"GENERIC", "FAST", "VERY_FAST"});
}

Decoder_RSC::parameters* Decoder_RSC::parameters
::clone() const
{
	return new Decoder_RSC::parameters(*this);
}

void Decoder_RSC::parameters
::delete_poly_option(CLI::App *app)
{
	CLI::remove_option(app, "--poly", get_prefix(), no_argflag());
	poly_option = nullptr;
}



void Decoder_RSC::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	Decoder::parameters::register_arguments(app);

	CLI::remove_option(app, "--cw-size", p, naf);

	CLI::add_set(app, p, naf,
		"--simd",
		simd_strategy,
		{"INTRA", "INTER"},
		"The SIMD strategy you want to use.",
		true)
		->group("Standard");

	CLI::add_set(app, p, naf,
		"--max",
		max,
		{"MAX", "MAXL", "MAXS"},
		"The MAX implementation for the nodes.",
		true)
		->group("Standard");

	CLI::add_flag(app, p, naf,
		"--no-buff",
		not_buffered,
		"Disable the buffered encoding.")
		->group("Standard");

	poly_option =
	CLI::add_option(app, p, naf,
		"--poly",
		poly_str,
		"The polynomials describing RSC code, should be of the form \"{A,B}\" in octal base.",
		true)
		->group("Standard");

	CLI::add_set(app, p, naf,
		"--std",
		standard,
		{"LTE", "CCSDS"},
		"Select a standard and set automatically some parameters (overwritten by \"--poly\").",
		true)
		->group("Standard");
}

void Decoder_RSC::parameters
::callback_arguments()
{
	Decoder::parameters::callback_arguments();

	if (poly_option != nullptr && !poly_option->empty())
	{
#ifdef _MSC_VER
		sscanf_s   (poly_str.c_str(), "{%o,%o}", &poly[0], &poly[1]);
#else
		std::sscanf(poly_str.c_str(), "{%o,%o}", &poly[0], &poly[1]);
#endif

		if (poly[0] == 013 && poly[1] == 015)
			standard = "LTE";

		else if (poly[0] == 023 && poly[1] == 033)
			standard = "CCSDS";

		else
			standard = "";
	}
	else
	{
		if (standard == "LTE")
			poly = {013, 015};

		else if (standard == "CCSDS")
			poly = {023, 033};
	}

	if (standard.empty())
		implem = "GENERIC";

	tail_length = (int)(2 * std::floor(std::log2((float)std::max(poly[0], poly[1]))));
	N_cw        = 2 * K + tail_length;
	R           = (float)K / (float)N_cw;
}

void Decoder_RSC::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_name();

	Decoder::parameters::get_headers(headers, full);

	if (type != "ML" && type != "CHASE")
	{
		if (tail_length && full)
			headers[p].push_back(std::make_pair("Tail length", std::to_string(tail_length)));

		if (full) headers[p].push_back(std::make_pair("Buffered", (!not_buffered ? "on" : "off")));

		if (!standard.empty())
			headers[p].push_back(std::make_pair("Standard", standard));

		std::stringstream ss_poly;
		ss_poly << "{0" << std::oct << poly[0] << ",0" << std::oct << poly[1] << "}";
		headers[p].push_back(std::make_pair(std::string("Polynomials"), ss_poly.str()));

		if (!simd_strategy.empty())
			headers[p].push_back(std::make_pair(std::string("SIMD strategy"), simd_strategy));

		headers[p].push_back(std::make_pair(std::string("Max type"), max));
	}
}

template <typename B, typename Q, typename QD, tools::proto_max<Q> MAX1, tools::proto_max<QD> MAX2>
module::Decoder_SISO_SIHO<B,Q>* Decoder_RSC::parameters
::_build_siso_seq(const std::vector<std::vector<int>> &trellis,
                        std::ostream                  &stream,
                  const int                            n_ite,
                  const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	if (type == "BCJR")
	{
		if (implem == "STD"         ) return new module::Decoder_RSC_BCJR_seq_std             <B,Q,QD,MAX1,MAX2>(K, trellis,        !not_buffered,         n_frames);
		if (implem == "GENERIC"     ) return new module::Decoder_RSC_BCJR_seq_generic_std     <B,Q,QD,MAX1,MAX2>(K, trellis,        !not_buffered,         n_frames);
		if (implem == "GENERIC_JSON") return new module::Decoder_RSC_BCJR_seq_generic_std_json<B,Q,QD,MAX1,MAX2>(K, trellis, n_ite, !not_buffered, stream, n_frames);
		if (implem == "FAST"        ) return new module::Decoder_RSC_BCJR_seq_fast            <B,Q,QD,MAX1,MAX2>(K, trellis,        !not_buffered,         n_frames);
		if (implem == "VERY_FAST"   ) return new module::Decoder_RSC_BCJR_seq_very_fast       <B,Q,QD,MAX1,MAX2>(K, trellis,        !not_buffered,         n_frames);
		if (implem == "SCAN"        ) return new module::Decoder_RSC_BCJR_seq_scan            <B,Q,QD          >(K, trellis,        !not_buffered,         n_frames);
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q, typename QD, tools::proto_max_i<Q> MAX>
module::Decoder_SISO_SIHO<B,Q>* Decoder_RSC::parameters
::_build_siso_simd(const std::vector<std::vector<int>> &trellis, const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	if (type == "BCJR" && simd_strategy == "INTER")
	{
		if (implem == "STD"      ) return new module::Decoder_RSC_BCJR_inter_std      <B,Q,MAX>(K, trellis, !not_buffered, n_frames);
		if (implem == "FAST"     ) return new module::Decoder_RSC_BCJR_inter_fast     <B,Q,MAX>(K, trellis, !not_buffered, n_frames);
		if (implem == "VERY_FAST") return new module::Decoder_RSC_BCJR_inter_very_fast<B,Q,MAX>(K, trellis, !not_buffered, n_frames);
	}

	if (type == "BCJR" && simd_strategy == "INTRA")
	{
		if (implem == "STD")
		{
			switch (mipp::nElReg<Q>())
			{
				case 8: return new module::Decoder_RSC_BCJR_intra_std<B,Q,MAX>(K, trellis, !not_buffered, n_frames);
				default:
					break;
			}
		}
		else if (implem == "FAST")
		{
#ifdef __AVX__
			switch (mipp::nElReg<Q>())
			{
				case 8:  return new module::Decoder_RSC_BCJR_intra_fast             <B,Q,MAX>(K, trellis, !not_buffered, n_frames); break;
				case 16: return new module::Decoder_RSC_BCJR_inter_intra_fast_x2_AVX<B,Q,MAX>(K, trellis, !not_buffered, n_frames); break;
				case 32: return new module::Decoder_RSC_BCJR_inter_intra_fast_x4_AVX<B,Q,MAX>(K, trellis, !not_buffered, n_frames); break;
				default:
					break;
			}
#else /* NEON and SSE */
			switch (mipp::nElReg<Q>())
			{
				case 8:  return new module::Decoder_RSC_BCJR_intra_fast             <B,Q,MAX>(K, trellis, !not_buffered, n_frames); break;
				case 16: return new module::Decoder_RSC_BCJR_inter_intra_fast_x2_SSE<B,Q,MAX>(K, trellis, !not_buffered, n_frames); break;
				default:
					break;
			}
#endif
		}
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SISO_SIHO<B,Q>* Decoder_RSC::parameters
::build_siso(const std::vector<std::vector<int>> &trellis,
                   std::ostream                  &stream,
             const int                            n_ite,
             const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	using QD = typename std::conditional<std::is_same<Q,int8_t>::value,int16_t,Q>::type;

	if (simd_strategy.empty())
	{
		if (max == "MAX" ) return _build_siso_seq<B,Q,QD,tools::max       <Q>,tools::max       <QD>>(trellis, stream, n_ite, encoder);
		if (max == "MAXS") return _build_siso_seq<B,Q,QD,tools::max_star  <Q>,tools::max_star  <QD>>(trellis, stream, n_ite, encoder);
		if (max == "MAXL") return _build_siso_seq<B,Q,QD,tools::max_linear<Q>,tools::max_linear<QD>>(trellis, stream, n_ite, encoder);
	}
	else
	{
		if (max == "MAX" ) return _build_siso_simd<B,Q,QD,tools::max_i       <Q>>(trellis, encoder);
		if (max == "MAXS") return _build_siso_simd<B,Q,QD,tools::max_star_i  <Q>>(trellis, encoder);
		if (max == "MAXL") return _build_siso_simd<B,Q,QD,tools::max_linear_i<Q>>(trellis, encoder);
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_RSC::parameters
::build(const std::vector<std::vector<int>> &trellis,
              std::ostream                  &stream,
        const int                            n_ite,
        const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	try
	{
		return Decoder::parameters::build<B,Q>(encoder);
	}
	catch (tools::cannot_allocate const&)
	{
		return build_siso<B,Q>(trellis, stream, n_ite);
	}
}

template <typename B, typename Q>
module::Decoder_SISO_SIHO<B,Q>* Decoder_RSC
::build_siso(const parameters                    &params,
             const std::vector<std::vector<int>> &trellis,
                   std::ostream                  &stream,
             const int                            n_ite,
             const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build_siso<B,Q>(trellis, stream, n_ite, encoder);
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_RSC
::build(const parameters                    &params,
        const std::vector<std::vector<int>> &trellis,
              std::ostream                  &stream,
        const int                            n_ite,
        const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build<B,Q>(trellis, stream, n_ite, encoder);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Decoder_SISO_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_RSC::parameters::build_siso<B_8 ,Q_8 >(const std::vector<std::vector<int>>&, std::ostream&, const int, const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_SISO_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_RSC::parameters::build_siso<B_16,Q_16>(const std::vector<std::vector<int>>&, std::ostream&, const int, const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_SISO_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_RSC::parameters::build_siso<B_32,Q_32>(const std::vector<std::vector<int>>&, std::ostream&, const int, const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_SISO_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_RSC::parameters::build_siso<B_64,Q_64>(const std::vector<std::vector<int>>&, std::ostream&, const int, const std::unique_ptr<module::Encoder<B_64>>&) const;
template aff3ct::module::Decoder_SISO_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_RSC::build_siso<B_8 ,Q_8 >(const aff3ct::factory::Decoder_RSC::parameters&, const std::vector<std::vector<int>>&, std::ostream&, const int, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SISO_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_RSC::build_siso<B_16,Q_16>(const aff3ct::factory::Decoder_RSC::parameters&, const std::vector<std::vector<int>>&, std::ostream&, const int, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SISO_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_RSC::build_siso<B_32,Q_32>(const aff3ct::factory::Decoder_RSC::parameters&, const std::vector<std::vector<int>>&, std::ostream&, const int, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SISO_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_RSC::build_siso<B_64,Q_64>(const aff3ct::factory::Decoder_RSC::parameters&, const std::vector<std::vector<int>>&, std::ostream&, const int, const std::unique_ptr<module::Encoder<B_64>>&);
#else
template aff3ct::module::Decoder_SISO_SIHO<B,Q>* aff3ct::factory::Decoder_RSC::parameters::build_siso<B,Q>(const std::vector<std::vector<int>>&, std::ostream&, const int, const std::unique_ptr<module::Encoder<B>>&) const;
template aff3ct::module::Decoder_SISO_SIHO<B,Q>* aff3ct::factory::Decoder_RSC::build_siso<B,Q>(const aff3ct::factory::Decoder_RSC::parameters&, const std::vector<std::vector<int>>&, std::ostream&, const int, const std::unique_ptr<module::Encoder<B>>&);
#endif

#ifdef MULTI_PREC
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_RSC::parameters::build<B_8 ,Q_8 >(const std::vector<std::vector<int>>&, std::ostream&, const int, const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_RSC::parameters::build<B_16,Q_16>(const std::vector<std::vector<int>>&, std::ostream&, const int, const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_RSC::parameters::build<B_32,Q_32>(const std::vector<std::vector<int>>&, std::ostream&, const int, const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_RSC::parameters::build<B_64,Q_64>(const std::vector<std::vector<int>>&, std::ostream&, const int, const std::unique_ptr<module::Encoder<B_64>>&) const;
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_RSC::build<B_8 ,Q_8 >(const aff3ct::factory::Decoder_RSC::parameters&, const std::vector<std::vector<int>>&, std::ostream&, const int, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_RSC::build<B_16,Q_16>(const aff3ct::factory::Decoder_RSC::parameters&, const std::vector<std::vector<int>>&, std::ostream&, const int, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_RSC::build<B_32,Q_32>(const aff3ct::factory::Decoder_RSC::parameters&, const std::vector<std::vector<int>>&, std::ostream&, const int, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_RSC::build<B_64,Q_64>(const aff3ct::factory::Decoder_RSC::parameters&, const std::vector<std::vector<int>>&, std::ostream&, const int, const std::unique_ptr<module::Encoder<B_64>>&);
#else
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_RSC::parameters::build<B,Q>(const std::vector<std::vector<int>>&, std::ostream&, const int, const std::unique_ptr<module::Encoder<B>>&) const;
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_RSC::build<B,Q>(const aff3ct::factory::Decoder_RSC::parameters&, const std::vector<std::vector<int>>&, std::ostream&, const int, const std::unique_ptr<module::Encoder<B>>&);
#endif
// ==================================================================================== explicit template instantiation
