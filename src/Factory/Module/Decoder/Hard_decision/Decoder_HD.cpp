#include <sstream>

#include "Tools/Exception/exception.hpp"

#include "Module/Decoder/Hard_decision/Decoder_HD.hpp"

#include "Decoder_HD.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Decoder_HD_name   = "Decoder hard decision";
const std::string aff3ct::factory::Decoder_HD_prefix = "dec";

Decoder_HD::parameters
::parameters(const std::string &prefix)
: Decoder_HD::parameters(Decoder_HD_name, prefix)
{
}

Decoder_HD::parameters
::parameters(const std::string &name, const std::string &prefix)
: Decoder::parameters(name, prefix)
{
	type   = "HD";
	implem = "STD";

	type_set.insert("HD");
}

Decoder_HD::parameters* Decoder_HD::parameters
::clone() const
{
	return new Decoder_HD::parameters(*this);
}

void Decoder_HD::parameters
::register_arguments(CLI::App &app)
{
	Decoder::parameters::register_arguments(app);
}

void Decoder_HD::parameters
::callback_arguments()
{
	Decoder::parameters::callback_arguments();
}

void Decoder_HD::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	Decoder::parameters::get_headers(headers, full);
}

template <typename B, typename Q>
module::Decoder_SISO_SIHO<B,Q>* Decoder_HD::parameters
::build_siso(const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	if (type == "HD" && implem == "STD") return new module::Decoder_HD<B,Q>(K, n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_HD::parameters
::build(const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	try
	{
		return Decoder::parameters::build<B,Q>(encoder);
	}
	catch (tools::cannot_allocate const&)
	{
		if (type == "HD" && implem == "STD") return new module::Decoder_HD<B,Q>(K, n_frames);
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SISO_SIHO<B,Q>* Decoder_HD
::build_siso(const parameters &params, const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build_siso<B,Q>(encoder);
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_HD
::build(const parameters &params, const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build<B,Q>(encoder);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_HD::parameters::build<B_8 ,Q_8 >(const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_HD::parameters::build<B_16,Q_16>(const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_HD::parameters::build<B_32,Q_32>(const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_HD::parameters::build<B_64,Q_64>(const std::unique_ptr<module::Encoder<B_64>>&) const;
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_HD::build<B_8 ,Q_8 >(const aff3ct::factory::Decoder_HD::parameters&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_HD::build<B_16,Q_16>(const aff3ct::factory::Decoder_HD::parameters&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_HD::build<B_32,Q_32>(const aff3ct::factory::Decoder_HD::parameters&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_HD::build<B_64,Q_64>(const aff3ct::factory::Decoder_HD::parameters&, const std::unique_ptr<module::Encoder<B_64>>&);
#else
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_HD::parameters::build<B,Q>(const std::unique_ptr<module::Encoder<B>>& ) const;
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_HD::build<B,Q>(const aff3ct::factory::Decoder_HD::parameters&, const std::unique_ptr<module::Encoder<B>>& );
#endif

#ifdef MULTI_PREC
template aff3ct::module::Decoder_SISO_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_HD::parameters::build_siso<B_8 ,Q_8 >(const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_SISO_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_HD::parameters::build_siso<B_16,Q_16>(const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_SISO_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_HD::parameters::build_siso<B_32,Q_32>(const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_SISO_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_HD::parameters::build_siso<B_64,Q_64>(const std::unique_ptr<module::Encoder<B_64>>&) const;
template aff3ct::module::Decoder_SISO_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_HD::build_siso<B_8 ,Q_8 >(const aff3ct::factory::Decoder_HD::parameters&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SISO_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_HD::build_siso<B_16,Q_16>(const aff3ct::factory::Decoder_HD::parameters&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SISO_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_HD::build_siso<B_32,Q_32>(const aff3ct::factory::Decoder_HD::parameters&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SISO_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_HD::build_siso<B_64,Q_64>(const aff3ct::factory::Decoder_HD::parameters&, const std::unique_ptr<module::Encoder<B_64>>&);
#else
template aff3ct::module::Decoder_SISO_SIHO<B,Q>* aff3ct::factory::Decoder_HD::parameters::build_siso<B,Q>(const std::unique_ptr<module::Encoder<B>>& ) const;
template aff3ct::module::Decoder_SISO_SIHO<B,Q>* aff3ct::factory::Decoder_HD::build_siso<B,Q>(const aff3ct::factory::Decoder_HD::parameters&, const std::unique_ptr<module::Encoder<B>>& );
#endif
// ==================================================================================== explicit template instantiation
