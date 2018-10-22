#include "Tools/Exception/exception.hpp"
#include "Tools/Math/max.h"
#include "Tools/Code/LDPC/Matrix_handler/LDPC_matrix_handler.hpp"

#include "Module/Decoder/LDPC/BP/Flooding/Decoder_LDPC_BP_flooding.hpp"
#include "Module/Decoder/LDPC/BP/Horizontal_layered/Decoder_LDPC_BP_horizontal_layered.hpp"
#include "Module/Decoder/LDPC/BP/Vertical_layered/Decoder_LDPC_BP_vertical_layered.hpp"

#include "Tools/Code/LDPC/Update_rule/SPA/Update_rule_SPA.hpp"
#include "Tools/Code/LDPC/Update_rule/LSPA/Update_rule_LSPA.hpp"
#include "Tools/Code/LDPC/Update_rule/MS/Update_rule_MS.hpp"
#include "Tools/Code/LDPC/Update_rule/OMS/Update_rule_OMS.hpp"
#include "Tools/Code/LDPC/Update_rule/NMS/Update_rule_NMS.hpp"
#include "Tools/Code/LDPC/Update_rule/AMS/Update_rule_AMS.hpp"

#ifdef __cpp_aligned_new
#include "Module/Decoder/LDPC/BP/Horizontal_layered/Decoder_LDPC_BP_horizontal_layered_inter.hpp"
#include "Module/Decoder/LDPC/BP/Vertical_layered/Decoder_LDPC_BP_vertical_layered_inter.hpp"
#include "Module/Decoder/LDPC/BP/Flooding/Decoder_LDPC_BP_flooding_inter.hpp"
#include "Tools/Code/LDPC/Update_rule/SPA/Update_rule_SPA_simd.hpp"
#include "Tools/Code/LDPC/Update_rule/LSPA/Update_rule_LSPA_simd.hpp"
#include "Tools/Code/LDPC/Update_rule/MS/Update_rule_MS_simd.hpp"
#include "Tools/Code/LDPC/Update_rule/OMS/Update_rule_OMS_simd.hpp"
#include "Tools/Code/LDPC/Update_rule/NMS/Update_rule_NMS_simd.hpp"
#include "Tools/Code/LDPC/Update_rule/AMS/Update_rule_AMS_simd.hpp"
#endif

#include "Module/Decoder/LDPC/BP/Horizontal_layered/ONMS/Decoder_LDPC_BP_horizontal_layered_ONMS_inter.hpp"
#include "Module/Decoder/LDPC/BP/Flooding/Gallager/Decoder_LDPC_BP_flooding_Gallager_A.hpp"
#include "Module/Decoder/LDPC/BP/Peeling/Decoder_LDPC_BP_peeling.hpp"
#include "Module/Decoder/LDPC/BF/OMWBF/Decoder_LDPC_bit_flipping_OMWBF.hpp"



#include "Decoder_LDPC.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Decoder_LDPC_name   = "Decoder LDPC";
const std::string aff3ct::factory::Decoder_LDPC_prefix = "dec";

Decoder_LDPC::parameters
::parameters(const std::string &prefix)
: Decoder_LDPC::parameters(Decoder_LDPC_name, prefix)
{
}

Decoder_LDPC::parameters
::parameters(const std::string &name, const std::string &prefix)
: Decoder::parameters(name, prefix)
{
	type   = "BP_FLOODING";
	implem = "SPA";

	type_set.insert({"BP_FLOODING", "BP_HORIZONTAL_LAYERED", "BP_VERTICAL_LAYERED", "BP_PEELING", "BIT_FLIPPING"});
#ifdef __cpp_aligned_new
	type_set.insert({"BP_HORIZONTAL_LAYERED_LEGACY"});
#endif
	implem_set.insert({"SPA", "LSPA", "MS", "OMS", "NMS", "AMS", "GALA", "WBF"});
}

Decoder_LDPC::parameters* Decoder_LDPC::parameters
::clone() const
{
	return new Decoder_LDPC::parameters(*this);
}

void Decoder_LDPC::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	Decoder::parameters::register_arguments(app);

	// auto h_path_option =
	CLI::add_option(app, p, naf,
		"--h-path",
		H_path,
		"Path to the H matrix (AList or QC formated file).")
		->check(CLI::ExistingFile)
		// ->required()
		->group("Standard");

	// h_path_option->excludes(app.get_option("--cw-size"  )); // N_cw is H width
	// h_path_option->excludes(app.get_option("--info-bits")); // if there is no K, then H is considered regular,
	                                                     // so K is the N - H's height



	CLI::add_option(app, p, naf,
		"-i,--ite",
		n_ite,
		"Maximal number of iterations in the LDPC decoder.",
		true)
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--off",
		offset,
		"Offset used in the offset min-sum BP algorithm (works only with \"--implem NMS\").",
		true)
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--norm",
		norm_factor,
		"Normalization factor used in the normalized min-sum BP algorithm (works only with \"--implem NMS\").",
		true)
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--mwbf",
		mwbf_factor,
		"Factor used in the modified WBF algorithm (works only with \"--implem WBF\"). Set 0 for basic WBF.",
		true)
		->group("Standard");


	CLI::add_flag(app, p, naf,
		"--no-synd",
		disable_syndrome,
		"Disable the syndrome detection (and so the stop criterion).")
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--synd-depth",
		syndrome_depth,
		"Successive number of iterations to validate the syndrome detection.",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");


	CLI::add_set(app, p, naf,
		"--simd",
		simd_strategy,
		{"INTER"},
		"The SIMD strategy you want to use.",
		true)
		->group("Standard");

	CLI::add_set(app, p, naf,
		"--min",
		min,
		{"MIN", "MINL", "MINS"},
		"The MIN implementation for the nodes (AMS decoder).",
		true)
		->group("Standard");

	CLI::add_set(app, p, naf,
		"--h-reorder",
		H_reorder,
		{"NONE", "ASC", "DSC"},
		"Specify if the check nodes (CNs) from H have to be reordered, 'NONE': do nothing (default), 'ASC': from the"
		" smallest to the biggest CNs, 'DSC': from the biggest to the smallest CNs.",
		true)
		->group("Standard");
}

void Decoder_LDPC::parameters
::callback_arguments()
{
	if (!H_path.empty())
	{
		unsigned M;
		tools::LDPC_matrix_handler::read_matrix_size(H_path, M, N_cw);

		if (M > N_cw)
			std::swap(M, N_cw);

		K = N_cw - M; // considered as regular so M = N - K
	}

	Decoder::parameters::callback_arguments();
}

void Decoder_LDPC::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_name();

	Decoder::parameters::get_headers(headers, full);

	if (type != "ML" && type != "CHASE")
	{
		if (!H_path.empty())
		{
			headers[p].push_back(std::make_pair("H matrix path", H_path));
			headers[p].push_back(std::make_pair("H matrix reordering", H_reorder));
		}

		if (!simd_strategy.empty())
			headers[p].push_back(std::make_pair("SIMD strategy", simd_strategy));

		headers[p].push_back(std::make_pair("Num. of iterations (i)", std::to_string(n_ite)));

		if (implem == "NMS")
			headers[p].push_back(std::make_pair("Normalize factor", std::to_string(norm_factor)));

		if (implem == "OMS")
			headers[p].push_back(std::make_pair("Offset", std::to_string(offset)));

		std::string syndrome = disable_syndrome ? "off" : "on";
		headers[p].push_back(std::make_pair("Stop criterion (syndrome)", syndrome));

		if (!disable_syndrome)
			headers[p].push_back(std::make_pair("Stop criterion depth", std::to_string(syndrome_depth)));

		if (implem == "AMS")
			headers[p].push_back(std::make_pair("Min type", min));
	}
}

template <typename B, typename Q>
module::Decoder_SISO_SIHO<B,Q>* Decoder_LDPC::parameters
::build_siso(const tools::Sparse_matrix &H, const std::vector<unsigned> &info_bits_pos,
             const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	if (type == "BP_FLOODING" && simd_strategy.empty())
	{
		const auto max_CN_degree = H.get_cols_max_degree();

		if (implem == "MS"  )  return new module::Decoder_LDPC_BP_flooding<B,Q,tools::Update_rule_MS  <Q                           >>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_MS  <Q                           >(             ), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "OMS" )  return new module::Decoder_LDPC_BP_flooding<B,Q,tools::Update_rule_OMS <Q                           >>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_OMS <Q                           >((Q)offset    ), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "NMS" )  return new module::Decoder_LDPC_BP_flooding<B,Q,tools::Update_rule_NMS <Q                           >>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS <Q                           >(norm_factor  ), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "SPA" )  return new module::Decoder_LDPC_BP_flooding<B,Q,tools::Update_rule_SPA <Q                           >>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_SPA <Q                           >(max_CN_degree), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "LSPA")  return new module::Decoder_LDPC_BP_flooding<B,Q,tools::Update_rule_LSPA<Q                           >>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_LSPA<Q                           >(max_CN_degree), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "AMS" )
		{
			if (min == "MIN" ) return new module::Decoder_LDPC_BP_flooding<B,Q,tools::Update_rule_AMS <Q,tools::min             <Q>>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_AMS <Q,tools::min             <Q>>(             ), !disable_syndrome, syndrome_depth, n_frames);
			if (min == "MINL") return new module::Decoder_LDPC_BP_flooding<B,Q,tools::Update_rule_AMS <Q,tools::min_star_linear2<Q>>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_AMS <Q,tools::min_star_linear2<Q>>(             ), !disable_syndrome, syndrome_depth, n_frames);
			if (min == "MINS") return new module::Decoder_LDPC_BP_flooding<B,Q,tools::Update_rule_AMS <Q,tools::min_star        <Q>>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_AMS <Q,tools::min_star        <Q>>(             ), !disable_syndrome, syndrome_depth, n_frames);
		}
	}
	else if (type == "BP_HORIZONTAL_LAYERED" && simd_strategy.empty())
	{
		const auto max_CN_degree = H.get_cols_max_degree();

		if (implem == "MS"  )  return new module::Decoder_LDPC_BP_horizontal_layered<B,Q,tools::Update_rule_MS  <Q                           >>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_MS  <Q                           >(             ), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "OMS" )  return new module::Decoder_LDPC_BP_horizontal_layered<B,Q,tools::Update_rule_OMS <Q                           >>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_OMS <Q                           >((Q)offset    ), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "NMS" )  return new module::Decoder_LDPC_BP_horizontal_layered<B,Q,tools::Update_rule_NMS <Q                           >>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS <Q                           >(norm_factor  ), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "SPA" )  return new module::Decoder_LDPC_BP_horizontal_layered<B,Q,tools::Update_rule_SPA <Q                           >>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_SPA <Q                           >(max_CN_degree), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "LSPA")  return new module::Decoder_LDPC_BP_horizontal_layered<B,Q,tools::Update_rule_LSPA<Q                           >>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_LSPA<Q                           >(max_CN_degree), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "AMS" )
		{
			if (min == "MIN" ) return new module::Decoder_LDPC_BP_horizontal_layered<B,Q,tools::Update_rule_AMS <Q,tools::min             <Q>>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_AMS <Q,tools::min             <Q>>(             ), !disable_syndrome, syndrome_depth, n_frames);
			if (min == "MINL") return new module::Decoder_LDPC_BP_horizontal_layered<B,Q,tools::Update_rule_AMS <Q,tools::min_star_linear2<Q>>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_AMS <Q,tools::min_star_linear2<Q>>(             ), !disable_syndrome, syndrome_depth, n_frames);
			if (min == "MINS") return new module::Decoder_LDPC_BP_horizontal_layered<B,Q,tools::Update_rule_AMS <Q,tools::min_star        <Q>>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_AMS <Q,tools::min_star        <Q>>(             ), !disable_syndrome, syndrome_depth, n_frames);
		}
	}
	else if (type == "BP_VERTICAL_LAYERED" && simd_strategy.empty())
	{
		const auto max_CN_degree = H.get_cols_max_degree();

		if (implem == "MS"  )  return new module::Decoder_LDPC_BP_vertical_layered<B,Q,tools::Update_rule_MS  <Q                           >>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_MS  <Q                           >(             ), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "OMS" )  return new module::Decoder_LDPC_BP_vertical_layered<B,Q,tools::Update_rule_OMS <Q                           >>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_OMS <Q                           >((Q)offset    ), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "NMS" )  return new module::Decoder_LDPC_BP_vertical_layered<B,Q,tools::Update_rule_NMS <Q                           >>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS <Q                           >(norm_factor  ), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "SPA" )  return new module::Decoder_LDPC_BP_vertical_layered<B,Q,tools::Update_rule_SPA <Q                           >>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_SPA <Q                           >(max_CN_degree), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "LSPA")  return new module::Decoder_LDPC_BP_vertical_layered<B,Q,tools::Update_rule_LSPA<Q                           >>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_LSPA<Q                           >(max_CN_degree), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "AMS" )
		{
			if (min == "MIN" ) return new module::Decoder_LDPC_BP_vertical_layered<B,Q,tools::Update_rule_AMS <Q,tools::min             <Q>>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_AMS <Q,tools::min             <Q>>(             ), !disable_syndrome, syndrome_depth, n_frames);
			if (min == "MINL") return new module::Decoder_LDPC_BP_vertical_layered<B,Q,tools::Update_rule_AMS <Q,tools::min_star_linear2<Q>>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_AMS <Q,tools::min_star_linear2<Q>>(             ), !disable_syndrome, syndrome_depth, n_frames);
			if (min == "MINS") return new module::Decoder_LDPC_BP_vertical_layered<B,Q,tools::Update_rule_AMS <Q,tools::min_star        <Q>>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_AMS <Q,tools::min_star        <Q>>(             ), !disable_syndrome, syndrome_depth, n_frames);
		}
	}
	else if (type == "BIT_FLIPPING")
	{
		     if (implem == "WBF")      return new module::Decoder_LDPC_bit_flipping_OMWBF<B,Q>(     K, N_cw, n_ite, H, info_bits_pos, mwbf_factor, !disable_syndrome, syndrome_depth, n_frames);
	}
#ifdef __cpp_aligned_new
	else if (type == "BP_HORIZONTAL_LAYERED" && simd_strategy == "INTER")
	{
		const auto max_CN_degree = H.get_cols_max_degree();

		if (implem == "SPA" ) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_SPA_simd <Q>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_SPA_simd <Q>(max_CN_degree), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "LSPA") return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_LSPA_simd<Q>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_LSPA_simd<Q>(max_CN_degree), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "MS"  ) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_MS_simd  <Q>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_MS_simd  <Q>(             ), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "OMS" ) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_OMS_simd <Q>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_OMS_simd <Q>(offset       ), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "NMS" )
		{
			if (typeid(Q) == typeid(int16_t) || typeid(Q) == typeid(int8_t))
			{
				if (norm_factor == 0.125f) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,1>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,1>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
				if (norm_factor == 0.250f) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,2>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,2>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
				if (norm_factor == 0.375f) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,3>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,3>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
				if (norm_factor == 0.500f) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,4>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,4>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
				if (norm_factor == 0.625f) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,5>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,5>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
				if (norm_factor == 0.750f) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,6>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,6>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
				if (norm_factor == 0.875f) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,7>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,7>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
				if (norm_factor == 1.000f) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,8>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,8>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);

				return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
			}
			else
				return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
		}
		else if (implem == "AMS" )
		{
			if (min == "MIN" ) return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_AMS_simd<Q,tools::min_i             <Q>>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_AMS_simd<Q,tools::min_i             <Q>>(), !disable_syndrome, syndrome_depth, n_frames);
			if (min == "MINL") return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_AMS_simd<Q,tools::min_star_linear2_i<Q>>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_AMS_simd<Q,tools::min_star_linear2_i<Q>>(), !disable_syndrome, syndrome_depth, n_frames);
			if (min == "MINS") return new module::Decoder_LDPC_BP_horizontal_layered_inter<B,Q,tools::Update_rule_AMS_simd<Q,tools::min_star_i        <Q>>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_AMS_simd<Q,tools::min_star_i        <Q>>(), !disable_syndrome, syndrome_depth, n_frames);
		}
	}
#endif
#ifdef __cpp_aligned_new
	else if (type == "BP_HORIZONTAL_LAYERED_LEGACY" && simd_strategy == "INTER")
#else
	else if (type == "BP_HORIZONTAL_LAYERED" && simd_strategy == "INTER")
#endif
	{
		if (implem == "MS" ) return new module::Decoder_LDPC_BP_horizontal_layered_ONMS_inter<B,Q>(K, N_cw, n_ite, H, info_bits_pos, 1.f              , (Q)0     , !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "NMS") return new module::Decoder_LDPC_BP_horizontal_layered_ONMS_inter<B,Q>(K, N_cw, n_ite, H, info_bits_pos, norm_factor, (Q)0           , !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "OMS") return new module::Decoder_LDPC_BP_horizontal_layered_ONMS_inter<B,Q>(K, N_cw, n_ite, H, info_bits_pos, 1.f              , (Q)offset, !disable_syndrome, syndrome_depth, n_frames);
	}
#ifdef __cpp_aligned_new
	else if (type == "BP_FLOODING" && simd_strategy == "INTER")
	{
		const auto max_CN_degree = H.get_cols_max_degree();

		if (implem == "SPA" ) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_SPA_simd <Q>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_SPA_simd <Q>(max_CN_degree), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "LSPA") return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_LSPA_simd<Q>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_LSPA_simd<Q>(max_CN_degree), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "MS"  ) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_MS_simd  <Q>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_MS_simd  <Q>(             ), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "OMS" ) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_OMS_simd <Q>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_OMS_simd <Q>(offset       ), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "NMS" )
		{
			if (typeid(Q) == typeid(int16_t) || typeid(Q) == typeid(int8_t))
			{
				if (norm_factor == 0.125f) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_NMS_simd<Q,1>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,1>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
				if (norm_factor == 0.250f) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_NMS_simd<Q,2>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,2>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
				if (norm_factor == 0.375f) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_NMS_simd<Q,3>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,3>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
				if (norm_factor == 0.500f) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_NMS_simd<Q,4>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,4>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
				if (norm_factor == 0.625f) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_NMS_simd<Q,5>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,5>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
				if (norm_factor == 0.750f) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_NMS_simd<Q,6>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,6>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
				if (norm_factor == 0.875f) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_NMS_simd<Q,7>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,7>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
				if (norm_factor == 1.000f) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_NMS_simd<Q,8>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,8>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);

				return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_NMS_simd<Q>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
			}
			else
				return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_NMS_simd<Q>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
		}
		else if (implem == "AMS" )
		{
			if (min == "MIN" ) return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_AMS_simd<Q,tools::min_i             <Q>>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_AMS_simd<Q,tools::min_i             <Q>>(), !disable_syndrome, syndrome_depth, n_frames);
			if (min == "MINL") return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_AMS_simd<Q,tools::min_star_linear2_i<Q>>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_AMS_simd<Q,tools::min_star_linear2_i<Q>>(), !disable_syndrome, syndrome_depth, n_frames);
			if (min == "MINS") return new module::Decoder_LDPC_BP_flooding_inter<B,Q,tools::Update_rule_AMS_simd<Q,tools::min_star_i        <Q>>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_AMS_simd<Q,tools::min_star_i        <Q>>(), !disable_syndrome, syndrome_depth, n_frames);
		}
	}
#endif
#ifdef __cpp_aligned_new
	else if (type == "BP_VERTICAL_LAYERED" && simd_strategy == "INTER")
	{
		const auto max_CN_degree = H.get_cols_max_degree();

		if (implem == "SPA" ) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_SPA_simd <Q>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_SPA_simd <Q>(max_CN_degree), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "LSPA") return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_LSPA_simd<Q>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_LSPA_simd<Q>(max_CN_degree), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "MS"  ) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_MS_simd  <Q>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_MS_simd  <Q>(             ), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "OMS" ) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_OMS_simd <Q>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_OMS_simd <Q>(offset       ), !disable_syndrome, syndrome_depth, n_frames);
		if (implem == "NMS" )
		{
			if (typeid(Q) == typeid(int16_t) || typeid(Q) == typeid(int8_t))
			{
				if (norm_factor == 0.125f) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,1>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,1>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
				if (norm_factor == 0.250f) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,2>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,2>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
				if (norm_factor == 0.375f) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,3>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,3>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
				if (norm_factor == 0.500f) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,4>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,4>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
				if (norm_factor == 0.625f) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,5>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,5>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
				if (norm_factor == 0.750f) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,6>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,6>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
				if (norm_factor == 0.875f) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,7>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,7>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
				if (norm_factor == 1.000f) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q,8>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q,8>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);

				return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
			}
			else
				return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_NMS_simd<Q>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_NMS_simd<Q>(norm_factor), !disable_syndrome, syndrome_depth, n_frames);
		}
		else if (implem == "AMS" )
		{
			if (min == "MIN" ) return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_AMS_simd<Q,tools::min_i             <Q>>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_AMS_simd<Q,tools::min_i             <Q>>(), !disable_syndrome, syndrome_depth, n_frames);
			if (min == "MINL") return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_AMS_simd<Q,tools::min_star_linear2_i<Q>>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_AMS_simd<Q,tools::min_star_linear2_i<Q>>(), !disable_syndrome, syndrome_depth, n_frames);
			if (min == "MINS") return new module::Decoder_LDPC_BP_vertical_layered_inter<B,Q,tools::Update_rule_AMS_simd<Q,tools::min_star_i        <Q>>>(K, N_cw, n_ite, H, info_bits_pos, tools::Update_rule_AMS_simd<Q,tools::min_star_i        <Q>>(), !disable_syndrome, syndrome_depth, n_frames);
		}
	}

#endif

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_LDPC::parameters
::build(const tools::Sparse_matrix &H, const std::vector<unsigned> &info_bits_pos,
        const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	try
	{
		return Decoder::parameters::build<B,Q>(encoder);
	}
	catch (tools::cannot_allocate const&)
	{
		if ((type == "BP" || type == "BP_FLOODING") && simd_strategy.empty())
		{
			if (implem == "GALA") return new module::Decoder_LDPC_BP_flooding_GALA<B,Q>(K, N_cw, n_ite, H, info_bits_pos, !disable_syndrome, syndrome_depth, n_frames);
		}
		else if (type == "BP_PEELING")
		{
			if (implem == "STD") return new module::Decoder_LDPC_BP_peeling<B,Q>(K, N_cw, n_ite, H, info_bits_pos, !disable_syndrome, syndrome_depth, n_frames);
		}

		return build_siso<B,Q>(H, info_bits_pos);
	}
}

template <typename B, typename Q>
module::Decoder_SISO_SIHO<B,Q>* Decoder_LDPC
::build_siso(const parameters& params, const tools::Sparse_matrix &H, const std::vector<unsigned> &info_bits_pos,
             const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build_siso<B,Q>(H, info_bits_pos, encoder);
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_LDPC
::build(const parameters& params, const tools::Sparse_matrix &H, const std::vector<unsigned> &info_bits_pos,
        const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build<B,Q>(H, info_bits_pos, encoder);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Decoder_SISO_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_LDPC::parameters::build_siso<B_8 ,Q_8 >(const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_SISO_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_LDPC::parameters::build_siso<B_16,Q_16>(const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_SISO_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_LDPC::parameters::build_siso<B_32,Q_32>(const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_SISO_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_LDPC::parameters::build_siso<B_64,Q_64>(const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_64>>&) const;
template aff3ct::module::Decoder_SISO_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_LDPC::build_siso<B_8 ,Q_8 >(const aff3ct::factory::Decoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SISO_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_LDPC::build_siso<B_16,Q_16>(const aff3ct::factory::Decoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SISO_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_LDPC::build_siso<B_32,Q_32>(const aff3ct::factory::Decoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SISO_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_LDPC::build_siso<B_64,Q_64>(const aff3ct::factory::Decoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_64>>&);
#else
template aff3ct::module::Decoder_SISO_SIHO<B,Q>* aff3ct::factory::Decoder_LDPC::parameters::build_siso<B,Q>(const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B>>& ) const;
template aff3ct::module::Decoder_SISO_SIHO<B,Q>* aff3ct::factory::Decoder_LDPC::build_siso<B,Q>(const aff3ct::factory::Decoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B>>& );
#endif

#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_LDPC::parameters::build<B_8 ,Q_8 >(const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_LDPC::parameters::build<B_16,Q_16>(const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_LDPC::parameters::build<B_32,Q_32>(const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_LDPC::parameters::build<B_64,Q_64>(const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_64>>&) const;
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_LDPC::build<B_8 ,Q_8 >(const aff3ct::factory::Decoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_LDPC::build<B_16,Q_16>(const aff3ct::factory::Decoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_LDPC::build<B_32,Q_32>(const aff3ct::factory::Decoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_LDPC::build<B_64,Q_64>(const aff3ct::factory::Decoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B_64>>&);
#else
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_LDPC::parameters::build<B,Q>(const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B>>& ) const;
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_LDPC::build<B,Q>(const aff3ct::factory::Decoder_LDPC::parameters&, const aff3ct::tools::Sparse_matrix&, const std::vector<unsigned>&, const std::unique_ptr<module::Encoder<B>>& );
#endif
// ==================================================================================== explicit template instantiation
