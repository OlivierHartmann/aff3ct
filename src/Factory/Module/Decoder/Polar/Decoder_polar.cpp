#include "Tools/Exception/exception.hpp"

#include "Module/Decoder/Polar/SC/Decoder_polar_SC_naive.hpp"
#include "Module/Decoder/Polar/SC/Decoder_polar_SC_naive_sys.hpp"
#include "Module/Decoder/Polar/SC/Decoder_polar_SC_fast_sys.hpp"
#include "Module/Decoder/Polar/SCAN/Decoder_polar_SCAN_naive.hpp"
#include "Module/Decoder/Polar/SCAN/Decoder_polar_SCAN_naive_sys.hpp"
#include "Module/Decoder/Polar/SCL/Decoder_polar_SCL_naive.hpp"
#include "Module/Decoder/Polar/SCL/Decoder_polar_SCL_naive_sys.hpp"
#include "Module/Decoder/Polar/SCL/Decoder_polar_SCL_fast_sys.hpp"
#include "Module/Decoder/Polar/SCL/Decoder_polar_SCL_MEM_fast_sys.hpp"
#include "Module/Decoder/Polar/SCL/CRC/Decoder_polar_SCL_naive_CA.hpp"
#include "Module/Decoder/Polar/SCL/CRC/Decoder_polar_SCL_naive_CA_sys.hpp"
#include "Module/Decoder/Polar/SCL/CRC/Decoder_polar_SCL_fast_CA_sys.hpp"
#include "Module/Decoder/Polar/SCL/CRC/Decoder_polar_SCL_MEM_fast_CA_sys.hpp"
#include "Module/Decoder/Polar/ASCL/Decoder_polar_ASCL_fast_CA_sys.hpp"
#include "Module/Decoder/Polar/ASCL/Decoder_polar_ASCL_MEM_fast_CA_sys.hpp"

//#define API_POLAR_DYNAMIC 1

#include "Tools/Code/Polar/API/API_polar_dynamic_seq.hpp"
#include "Tools/Code/Polar/API/API_polar_dynamic_intra.hpp"
#ifdef API_POLAR_DYNAMIC
#include "Tools/Code/Polar/API/API_polar_dynamic_inter.hpp"
#include "Tools/Code/Polar/API/API_polar_dynamic_inter_8bit_bitpacking.hpp"
#else
#include "Tools/Code/Polar/API/API_polar_static_seq.hpp"
#include "Tools/Code/Polar/API/API_polar_static_inter.hpp"
#include "Tools/Code/Polar/API/API_polar_static_inter_8bit_bitpacking.hpp"
#include "Tools/Code/Polar/API/API_polar_static_intra_8bit.hpp"
#include "Tools/Code/Polar/API/API_polar_static_intra_16bit.hpp"
#include "Tools/Code/Polar/API/API_polar_static_intra_32bit.hpp"
#endif

#include "Tools/Code/Polar/nodes_parser.h"

#include "Decoder_polar.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Decoder_polar_name   = "Decoder Polar";
const std::string aff3ct::factory::Decoder_polar_prefix = "dec";

Decoder_polar::parameters
::parameters(const std::string &prefix)
: Decoder_polar::parameters(Decoder_polar_name, prefix)
{
}

Decoder_polar::parameters
::parameters(const std::string &name, const std::string &prefix)
: Decoder::parameters(name, prefix)
{
	type   = "SC";
	implem = "FAST";

	type_set  .insert({"SC", "SCL", "SCL_MEM", "ASCL", "ASCL_MEM", "SCAN"});
	implem_set.insert({"FAST", "NAIVE"});
}

Decoder_polar::parameters* Decoder_polar::parameters
::clone() const
{
	return new Decoder_polar::parameters(*this);
}

void Decoder_polar::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	Decoder::parameters::register_arguments(app);

	CLI::add_option(app, p, naf,
		"-i,--ite",
		n_ite,
		"Maximal number of iterations in the SCAN decoder.",
		true)
		->group("Standard");

	CLI::add_option(app, p, naf,
		"-L,--lists",
		L,
		"Maximal number of paths in the SCL decoder.",
		true)
		->group("Standard");

	CLI::add_set(app, p, naf,
		"--simd",
		simd_strategy,
		{"INTRA", "INTER"},
		"The SIMD strategy you want to use.",
		true)
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--polar-nodes",
		polar_nodes,
		"The type of nodes you want to detect in the Polar tree (ex: \"{R0,R1,R0L,REP_2-8,REPL,SPC_4+}\").",
		true)
		->group("Standard");

	CLI::add_flag(app, p, naf,
		"--partial-adaptive",
		partial_adaptive,
		"Enable the partial adaptive mode for the ASCL decoder (by default full adaptive is selected).")
		->group("Standard");

	CLI::add_flag(app, p, naf,
		"--no-sys",
		not_systematic,
		"Does not suppose a systematic encoding.")
		->group("Standard");
}

void Decoder_polar::parameters
::callback_arguments()
{
	Decoder::parameters::callback_arguments();

	// force 1 iteration max if not SCAN (and polar code)
	if (type != "SCAN") n_ite = 1;
}

void Decoder_polar::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_short_name();

	Decoder::parameters::get_headers(headers, full);

	if (type != "ML" && type != "CHASE")
	{
		if (!simd_strategy.empty())
			headers[p].push_back(std::make_pair("SIMD strategy", simd_strategy));

		if (type == "SCAN")
			headers[p].push_back(std::make_pair("Num. of iterations (i)", std::to_string(n_ite)));

		if (type == "SCL" || type == "SCL_MEM")
			headers[p].push_back(std::make_pair("Num. of lists (L)", std::to_string(L)));

		if (type == "ASCL" || type == "ASCL_MEM")
		{
			auto adaptative_mode = partial_adaptive ? "partial" : "full";
			headers[p].push_back(std::make_pair("Max num. of lists (L)", std::to_string(L)));
			headers[p].push_back(std::make_pair("Adaptative mode", adaptative_mode));
		}

		if ((type == "SC"      ||
		     type == "SCL"     ||
		     type == "ASCL"    ||
		     type == "SCL_MEM" ||
		     type == "ASCL_MEM") && implem == "FAST")
			headers[p].push_back(std::make_pair("Polar node types", polar_nodes));
	}
}

template <typename B, typename Q>
module::Decoder_SISO_SIHO<B,Q>* Decoder_polar::parameters
::build_siso(const std::vector<bool> &frozen_bits, const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	if (type == "SCAN" && !not_systematic)
	{
		if (implem == "NAIVE") return new module::Decoder_polar_SCAN_naive_sys<B, Q, tools::f_LLR<Q>, tools::v_LLR<Q>, tools::h_LLR<B,Q>>(K, N_cw, n_ite, frozen_bits, n_frames);
	}
	else if (type == "SCAN" && not_systematic)
	{
		if (implem == "NAIVE") return new module::Decoder_polar_SCAN_naive    <B, Q, tools::f_LLR<Q>, tools::v_LLR<Q>, tools::h_LLR<B,Q>>(K, N_cw, n_ite, frozen_bits, n_frames);
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q, class API_polar>
module::Decoder_SIHO<B,Q>* Decoder_polar::parameters
::_build(const std::vector<bool> &frozen_bits, module::CRC<B> *crc, const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	if (not_systematic) // non-systematic encoding
	{
		if (implem == "NAIVE")
		{
			if (crc == nullptr || crc->get_size() == 0)
			{
				if (type == "SC"  ) return new module::Decoder_polar_SC_naive        <B,Q,tools::f_LLR<Q>,tools::g_LLR<B,Q>,tools::h_LLR<B,Q>>(K, N_cw,        frozen_bits,       n_frames);
				if (type == "SCAN") return new module::Decoder_polar_SCAN_naive      <B,Q,tools::f_LLR<Q>,tools::v_LLR<  Q>,tools::h_LLR<B,Q>>(K, N_cw, n_ite, frozen_bits,       n_frames);
				if (type == "SCL" ) return new module::Decoder_polar_SCL_naive       <B,Q,tools::f_LLR<Q>,tools::g_LLR<B,Q>                  >(K, N_cw, L,     frozen_bits,       n_frames);
			}
			else
				if (type == "SCL" ) return new module::Decoder_polar_SCL_naive_CA    <B,Q,tools::f_LLR<Q>,tools::g_LLR<B,Q>                  >(K, N_cw, L,     frozen_bits, *crc, n_frames);
		}
	}
	else // systematic encoding
	{
		if (implem == "NAIVE")
		{
			if (crc == nullptr || crc->get_size() == 0)
			{
				if (type == "SC"  ) return new module::Decoder_polar_SC_naive_sys    <B,Q,tools::f_LLR<Q>,tools::g_LLR<B,Q>,tools::h_LLR<B,Q>>(K, N_cw,        frozen_bits,       n_frames);
				if (type == "SCAN") return new module::Decoder_polar_SCAN_naive_sys  <B,Q,tools::f_LLR<Q>,tools::v_LLR<  Q>,tools::h_LLR<B,Q>>(K, N_cw, n_ite, frozen_bits,       n_frames);
				if (type == "SCL" ) return new module::Decoder_polar_SCL_naive_sys   <B,Q,tools::f_LLR<Q>,tools::g_LLR<B,Q>                  >(K, N_cw, L,     frozen_bits,       n_frames);
			}
			else
				if (type == "SCL" ) return new module::Decoder_polar_SCL_naive_CA_sys<B,Q,tools::f_LLR<Q>,tools::g_LLR<B,Q>                  >(K, N_cw, L,     frozen_bits, *crc, n_frames);
		}
		else if (implem == "FAST")
		{
			if (crc == nullptr || crc->get_size() == 0)
			{
				int idx_r0, idx_r1;
				auto polar_patterns = tools::nodes_parser(polar_nodes, idx_r0, idx_r1);
				if (type == "SC"  ) return new module::Decoder_polar_SC_fast_sys<B, Q, API_polar>(K, N_cw, frozen_bits, std::move(polar_patterns), idx_r0, idx_r1, n_frames);
			}
		}
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q, class API_polar>
module::Decoder_SIHO<B,Q>* Decoder_polar::parameters
::_build_scl_fast(const std::vector<bool> &frozen_bits, module::CRC<B> *crc, const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	int idx_r0, idx_r1;
	auto polar_patterns = tools::nodes_parser(polar_nodes, idx_r0, idx_r1);

	if (implem == "FAST" && !not_systematic)
	{
		if (crc != nullptr && crc->get_size() > 0)
		{
			if (type == "ASCL"    ) return new module::Decoder_polar_ASCL_fast_CA_sys    <B, Q, API_polar>(K, N_cw, L, frozen_bits, std::move(polar_patterns), idx_r0, idx_r1, *crc, !partial_adaptive, n_frames);
			if (type == "ASCL_MEM") return new module::Decoder_polar_ASCL_MEM_fast_CA_sys<B, Q, API_polar>(K, N_cw, L, frozen_bits, std::move(polar_patterns), idx_r0, idx_r1, *crc, !partial_adaptive, n_frames);
			if (type == "SCL"     ) return new module::Decoder_polar_SCL_fast_CA_sys     <B, Q, API_polar>(K, N_cw, L, frozen_bits, std::move(polar_patterns), idx_r0, idx_r1, *crc,                    n_frames);
			if (type == "SCL_MEM" ) return new module::Decoder_polar_SCL_MEM_fast_CA_sys <B, Q, API_polar>(K, N_cw, L, frozen_bits, std::move(polar_patterns), idx_r0, idx_r1, *crc,                    n_frames);
		}
		else
		{
			if (type == "SCL"     ) return new module::Decoder_polar_SCL_fast_sys        <B, Q, API_polar>(K, N_cw, L, frozen_bits, std::move(polar_patterns), idx_r0, idx_r1,                          n_frames);
			if (type == "SCL_MEM" ) return new module::Decoder_polar_SCL_MEM_fast_sys    <B, Q, API_polar>(K, N_cw, L, frozen_bits, std::move(polar_patterns), idx_r0, idx_r1,                          n_frames);
		}
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_polar::parameters
::build(const std::vector<bool> &frozen_bits, module::CRC<B> *crc, const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	try
	{
		return Decoder::parameters::build<B,Q>(encoder);
	}
	catch (tools::cannot_allocate const&)
	{
		if (type.find("SCL") != std::string::npos && implem == "FAST")
		{
			if (simd_strategy == "INTRA")
			{
				if (typeid(B) == typeid(signed char))
				{
					return _build_scl_fast<B,Q,tools::API_polar_dynamic_intra<B,Q>>(frozen_bits, crc, encoder);
				}
				else if (typeid(B) == typeid(short))
				{
					return _build_scl_fast<B,Q,tools::API_polar_dynamic_intra<B,Q>>(frozen_bits, crc, encoder);
				}
				else if (typeid(B) == typeid(int))
				{
					return _build_scl_fast<B,Q,tools::API_polar_dynamic_intra<B,Q>>(frozen_bits, crc, encoder);
				}
			}
			else if (simd_strategy.empty())
			{
				return _build_scl_fast<B,Q,tools::API_polar_dynamic_seq<B,Q>>(frozen_bits, crc, encoder);
			}
		}

		if (simd_strategy == "INTER" && type == "SC" && implem == "FAST")
		{
			if (typeid(B) == typeid(signed char))
			{
#ifdef ENABLE_BIT_PACKING
#ifdef API_POLAR_DYNAMIC
				using API_polar = tools::API_polar_dynamic_inter_8bit_bitpacking<B,Q>;
#else
				using API_polar = tools::API_polar_static_inter_8bit_bitpacking<B,Q>;
#endif
#else
#ifdef API_POLAR_DYNAMIC
				using API_polar = tools::API_polar_dynamic_inter<B,Q>;
#else
				using API_polar = tools::API_polar_static_inter<B,Q>;
#endif
#endif
				return _build<B,Q,API_polar>(frozen_bits, crc, encoder);
			}
			else
			{
#ifdef API_POLAR_DYNAMIC
				using API_polar = tools::API_polar_dynamic_inter<B,Q>;
#else
				using API_polar = tools::API_polar_static_inter<B,Q>;
#endif
				return _build<B,Q,API_polar>(frozen_bits, crc, encoder);
			}
		}
		else if (simd_strategy == "INTRA" && implem == "FAST")
		{
			if (typeid(B) == typeid(signed char))
			{
#ifdef API_POLAR_DYNAMIC
				using API_polar = tools::API_polar_dynamic_intra<B,Q>;
#else
				using API_polar = tools::API_polar_static_intra_8bit<B,Q>;
#endif
				return _build<B,Q,API_polar>(frozen_bits, crc, encoder);
			}
			else if (typeid(B) == typeid(short))
			{
#ifdef API_POLAR_DYNAMIC
				using API_polar = tools::API_polar_dynamic_intra<B,Q>;
#else
				using API_polar = tools::API_polar_static_intra_16bit<B,Q>;
#endif
				return _build<B,Q,API_polar>(frozen_bits, crc, encoder);
			}
			else if (typeid(B) == typeid(int))
			{
#ifdef API_POLAR_DYNAMIC
				using API_polar = tools::API_polar_dynamic_intra<B,Q>;
#else
				using API_polar = tools::API_polar_static_intra_32bit<B,Q>;
#endif
				return _build<B,Q,API_polar>(frozen_bits, crc, encoder);
			}
		}
		else if (simd_strategy.empty())
		{
#ifdef API_POLAR_DYNAMIC
			using API_polar = tools::API_polar_dynamic_seq<B,Q>;
#else
			using API_polar = tools::API_polar_static_seq<B,Q>;
#endif
			return _build<B,Q,API_polar>(frozen_bits, crc, encoder);
		}
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SISO_SIHO<B,Q>* Decoder_polar
::build_siso(const parameters& params, const std::vector<bool> &frozen_bits,
             const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build_siso<B,Q>(frozen_bits, encoder);
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_polar
::build(const parameters& params, const std::vector<bool> &frozen_bits, module::CRC<B> *crc,
        const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build<B,Q>(frozen_bits, crc, encoder);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Decoder_SISO_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_polar::parameters::build_siso<B_8 ,Q_8 >(const std::vector<bool>&, const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_SISO_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_polar::parameters::build_siso<B_16,Q_16>(const std::vector<bool>&, const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_SISO_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_polar::parameters::build_siso<B_32,Q_32>(const std::vector<bool>&, const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_SISO_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_polar::parameters::build_siso<B_64,Q_64>(const std::vector<bool>&, const std::unique_ptr<module::Encoder<B_64>>&) const;
template aff3ct::module::Decoder_SISO_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_polar::build_siso<B_8 ,Q_8 >(const aff3ct::factory::Decoder_polar::parameters&, const std::vector<bool>&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SISO_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_polar::build_siso<B_16,Q_16>(const aff3ct::factory::Decoder_polar::parameters&, const std::vector<bool>&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SISO_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_polar::build_siso<B_32,Q_32>(const aff3ct::factory::Decoder_polar::parameters&, const std::vector<bool>&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SISO_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_polar::build_siso<B_64,Q_64>(const aff3ct::factory::Decoder_polar::parameters&, const std::vector<bool>&, const std::unique_ptr<module::Encoder<B_64>>&);
#else
template aff3ct::module::Decoder_SISO_SIHO<B,Q>* aff3ct::factory::Decoder_polar::parameters::build_siso<B,Q>(const std::vector<bool>&, const std::unique_ptr<module::Encoder<B>>& ) const;
template aff3ct::module::Decoder_SISO_SIHO<B,Q>* aff3ct::factory::Decoder_polar::build_siso<B,Q>(const aff3ct::factory::Decoder_polar::parameters&, const std::vector<bool>&, const std::unique_ptr<module::Encoder<B>>& );
#endif

#ifdef MULTI_PREC
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_polar::parameters::build<B_8 ,Q_8 >(const std::vector<bool>&, module::CRC<B_8 >*, const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_polar::parameters::build<B_16,Q_16>(const std::vector<bool>&, module::CRC<B_16>*, const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_polar::parameters::build<B_32,Q_32>(const std::vector<bool>&, module::CRC<B_32>*, const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_polar::parameters::build<B_64,Q_64>(const std::vector<bool>&, module::CRC<B_64>*, const std::unique_ptr<module::Encoder<B_64>>&) const;
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_polar::build<B_8 ,Q_8 >(const aff3ct::factory::Decoder_polar::parameters&, const std::vector<bool>&, module::CRC<B_8 >*, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_polar::build<B_16,Q_16>(const aff3ct::factory::Decoder_polar::parameters&, const std::vector<bool>&, module::CRC<B_16>*, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_polar::build<B_32,Q_32>(const aff3ct::factory::Decoder_polar::parameters&, const std::vector<bool>&, module::CRC<B_32>*, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_polar::build<B_64,Q_64>(const aff3ct::factory::Decoder_polar::parameters&, const std::vector<bool>&, module::CRC<B_64>*, const std::unique_ptr<module::Encoder<B_64>>&);
#else
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_polar::parameters::build<B,Q>(const std::vector<bool>&, module::CRC<B>*, const std::unique_ptr<module::Encoder<B>>& ) const;
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_polar::build<B,Q>(const aff3ct::factory::Decoder_polar::parameters&, const std::vector<bool>&, module::CRC<B>*, const std::unique_ptr<module::Encoder<B>>& );
#endif
// ==================================================================================== explicit template instantiation
