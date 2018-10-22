#include "Tools/Exception/exception.hpp"

#include "Module/Decoder/Repetition/Decoder_repetition_std.hpp"
#include "Module/Decoder/Repetition/Decoder_repetition_fast.hpp"

#include "Decoder_repetition.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Decoder_repetition_name   = "Decoder Repetition";
const std::string aff3ct::factory::Decoder_repetition_prefix = "dec";

Decoder_repetition::parameters
::parameters(const std::string &prefix)
: Decoder_repetition::parameters(Decoder_repetition_name, prefix)
{
}

Decoder_repetition::parameters
::parameters(const std::string &name, const std::string &prefix)
: Decoder::parameters(name, prefix)
{
	type   = "REPETITION";
	implem = "STD";

	type_set  .insert({"REPETITION"});
	implem_set.insert({"STD", "FAST"});
}

Decoder_repetition::parameters* Decoder_repetition::parameters
::clone() const
{
	return new Decoder_repetition::parameters(*this);
}

void Decoder_repetition::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	Decoder::parameters::register_arguments(app);

	CLI::add_flag(app, p, naf,
		"--no-buff",
		not_buffered,
		"Does not suppose a buffered encoding.")
		->group("Standard");
}

void Decoder_repetition::parameters
::callback_arguments()
{
	Decoder::parameters::callback_arguments();
}

void Decoder_repetition::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_name();

	Decoder::parameters::get_headers(headers, full);

	if (type != "ML" && type != "CHASE")
		if (full) headers[p].push_back(std::make_pair("Buffered", (not_buffered ? "off" : "on")));
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_repetition::parameters
::build(const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	try
	{
		return Decoder::parameters::build<B,Q>(encoder);
	}
	catch (tools::cannot_allocate const&)
	{
		if (type == "REPETITION")
		{
			if (implem == "STD" ) return new module::Decoder_repetition_std <B,Q>(K, N_cw, !not_buffered, n_frames);
			if (implem == "FAST") return new module::Decoder_repetition_fast<B,Q>(K, N_cw, !not_buffered, n_frames);
		}
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_repetition
::build(const parameters &params, const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build<B,Q>(encoder);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_repetition::parameters::build<B_8 ,Q_8 >(const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_repetition::parameters::build<B_16,Q_16>(const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_repetition::parameters::build<B_32,Q_32>(const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_repetition::parameters::build<B_64,Q_64>(const std::unique_ptr<module::Encoder<B_64>>&) const;
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_repetition::build<B_8 ,Q_8 >(const aff3ct::factory::Decoder_repetition::parameters&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_repetition::build<B_16,Q_16>(const aff3ct::factory::Decoder_repetition::parameters&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_repetition::build<B_32,Q_32>(const aff3ct::factory::Decoder_repetition::parameters&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_repetition::build<B_64,Q_64>(const aff3ct::factory::Decoder_repetition::parameters&, const std::unique_ptr<module::Encoder<B_64>>&);
#else
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_repetition::parameters::build<B,Q>(const std::unique_ptr<module::Encoder<B>>& ) const;
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_repetition::build<B,Q>(const aff3ct::factory::Decoder_repetition::parameters&, const std::unique_ptr<module::Encoder<B>>& );
#endif
// ==================================================================================== explicit template instantiation
