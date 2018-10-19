#include "Tools/Exception/exception.hpp"

#include "Module/Decoder/RSC_DB/BCJR/Decoder_RSC_DB_BCJR_generic.hpp"
#include "Module/Decoder/RSC_DB/BCJR/Decoder_RSC_DB_BCJR_DVB_RCS1.hpp"
#include "Module/Decoder/RSC_DB/BCJR/Decoder_RSC_DB_BCJR_DVB_RCS2.hpp"

#include "Decoder_RSC_DB.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Decoder_RSC_DB_name   = "Decoder RSC DB";
const std::string aff3ct::factory::Decoder_RSC_DB_prefix = "dec";

Decoder_RSC_DB::parameters
::parameters(const std::string &prefix)
: Decoder::parameters(Decoder_RSC_DB_name, prefix)
{
	type   = "BCJR";
	implem = "GENERIC";

	type_set  .insert("BCJR");
	implem_set.insert({"GENERIC", "DVB-RCS1", "DVB-RCS2"});
}

Decoder_RSC_DB::parameters* Decoder_RSC_DB::parameters
::clone() const
{
	return new Decoder_RSC_DB::parameters(*this);
}

void Decoder_RSC_DB::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	Decoder::parameters::register_arguments(app);

	CLI::remove_option(app, "--cw-size", p, naf);

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
		"Disable the !not_buffered encoding.")
		->group("Standard");
}

void Decoder_RSC_DB::parameters
::callback_arguments()
{
	Decoder::parameters::callback_arguments();

	N_cw = 2 * K;
	R    = (float)K / (float)N_cw;
}

void Decoder_RSC_DB::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_short_name();

	Decoder::parameters::get_headers(headers, full);

	if (type != "ML" && type != "CHASE")
	{
		if (tail_length && full)
			headers[p].push_back(std::make_pair("Tail length", std::to_string(tail_length)));

		if (full) headers[p].push_back(std::make_pair("Buffered", (!not_buffered ? "on" : "off")));

		headers[p].push_back(std::make_pair(std::string("Max type"), max));
	}
}

template <typename B, typename Q, tools::proto_max<Q> MAX>
module::Decoder_RSC_DB_BCJR<B,Q>* Decoder_RSC_DB::parameters
::_build_siso(const std::vector<std::vector<int>> &trellis, const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	if (type == "BCJR")
	{
		if (implem == "GENERIC" ) return new module::Decoder_RSC_DB_BCJR_generic <B,Q,MAX>(K, trellis, !not_buffered, n_frames);
		if (implem == "DVB-RCS1") return new module::Decoder_RSC_DB_BCJR_DVB_RCS1<B,Q,MAX>(K, trellis, !not_buffered, n_frames);
		if (implem == "DVB-RCS2") return new module::Decoder_RSC_DB_BCJR_DVB_RCS2<B,Q,MAX>(K, trellis, !not_buffered, n_frames);
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_RSC_DB_BCJR<B,Q>* Decoder_RSC_DB::parameters
::build_siso(const std::vector<std::vector<int>> &trellis, const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	if (max == "MAX" ) return _build_siso<B,Q,tools::max       <Q>>(trellis, encoder);
	if (max == "MAXS") return _build_siso<B,Q,tools::max_star  <Q>>(trellis, encoder);
	if (max == "MAXL") return _build_siso<B,Q,tools::max_linear<Q>>(trellis, encoder);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_RSC_DB::parameters
::build(const std::vector<std::vector<int>> &trellis, const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	try
	{
		return Decoder::parameters::build<B,Q>(encoder);
	}
	catch (tools::cannot_allocate const&)
	{
		return build_siso<B,Q>(trellis);
	}
}

template <typename B, typename Q>
module::Decoder_RSC_DB_BCJR<B,Q>* Decoder_RSC_DB
::build_siso(const parameters &params, const std::vector<std::vector<int>> &trellis,
             const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build_siso<B,Q>(trellis, encoder);
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_RSC_DB
::build(const parameters &params, const std::vector<std::vector<int>> &trellis,
        const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build<B,Q>(trellis, encoder);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Decoder_RSC_DB_BCJR<B_8 ,Q_8 >* aff3ct::factory::Decoder_RSC_DB::parameters::build_siso<B_8 ,Q_8 >(const std::vector<std::vector<int>>&, const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_RSC_DB_BCJR<B_16,Q_16>* aff3ct::factory::Decoder_RSC_DB::parameters::build_siso<B_16,Q_16>(const std::vector<std::vector<int>>&, const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_RSC_DB_BCJR<B_32,Q_32>* aff3ct::factory::Decoder_RSC_DB::parameters::build_siso<B_32,Q_32>(const std::vector<std::vector<int>>&, const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_RSC_DB_BCJR<B_64,Q_64>* aff3ct::factory::Decoder_RSC_DB::parameters::build_siso<B_64,Q_64>(const std::vector<std::vector<int>>&, const std::unique_ptr<module::Encoder<B_64>>&) const;
template aff3ct::module::Decoder_RSC_DB_BCJR<B_8 ,Q_8 >* aff3ct::factory::Decoder_RSC_DB::build_siso<B_8 ,Q_8 >(const aff3ct::factory::Decoder_RSC_DB::parameters&, const std::vector<std::vector<int>>&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_RSC_DB_BCJR<B_16,Q_16>* aff3ct::factory::Decoder_RSC_DB::build_siso<B_16,Q_16>(const aff3ct::factory::Decoder_RSC_DB::parameters&, const std::vector<std::vector<int>>&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_RSC_DB_BCJR<B_32,Q_32>* aff3ct::factory::Decoder_RSC_DB::build_siso<B_32,Q_32>(const aff3ct::factory::Decoder_RSC_DB::parameters&, const std::vector<std::vector<int>>&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_RSC_DB_BCJR<B_64,Q_64>* aff3ct::factory::Decoder_RSC_DB::build_siso<B_64,Q_64>(const aff3ct::factory::Decoder_RSC_DB::parameters&, const std::vector<std::vector<int>>&, const std::unique_ptr<module::Encoder<B_64>>&);
#else
template aff3ct::module::Decoder_RSC_DB_BCJR<B,Q>* aff3ct::factory::Decoder_RSC_DB::parameters::build_siso<B,Q>(const std::vector<std::vector<int>>&, const std::unique_ptr<module::Encoder<B>>&) const;
template aff3ct::module::Decoder_RSC_DB_BCJR<B,Q>* aff3ct::factory::Decoder_RSC_DB::build_siso<B,Q>(const aff3ct::factory::Decoder_RSC_DB::parameters&, const std::vector<std::vector<int>>&, const std::unique_ptr<module::Encoder<B>>&);
#endif

#ifdef MULTI_PREC
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_RSC_DB::parameters::build<B_8 ,Q_8 >(const std::vector<std::vector<int>>&, const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_RSC_DB::parameters::build<B_16,Q_16>(const std::vector<std::vector<int>>&, const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_RSC_DB::parameters::build<B_32,Q_32>(const std::vector<std::vector<int>>&, const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_RSC_DB::parameters::build<B_64,Q_64>(const std::vector<std::vector<int>>&, const std::unique_ptr<module::Encoder<B_64>>&) const;
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_RSC_DB::build<B_8 ,Q_8 >(const aff3ct::factory::Decoder_RSC_DB::parameters&, const std::vector<std::vector<int>>&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_RSC_DB::build<B_16,Q_16>(const aff3ct::factory::Decoder_RSC_DB::parameters&, const std::vector<std::vector<int>>&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_RSC_DB::build<B_32,Q_32>(const aff3ct::factory::Decoder_RSC_DB::parameters&, const std::vector<std::vector<int>>&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_RSC_DB::build<B_64,Q_64>(const aff3ct::factory::Decoder_RSC_DB::parameters&, const std::vector<std::vector<int>>&, const std::unique_ptr<module::Encoder<B_64>>&);
#else
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_RSC_DB::parameters::build<B,Q>(const std::vector<std::vector<int>>&, const std::unique_ptr<module::Encoder<B>>&) const;
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_RSC_DB::build<B,Q>(const aff3ct::factory::Decoder_RSC_DB::parameters&, const std::vector<std::vector<int>>&, const std::unique_ptr<module::Encoder<B>>&);
#endif
// ==================================================================================== explicit template instantiation
