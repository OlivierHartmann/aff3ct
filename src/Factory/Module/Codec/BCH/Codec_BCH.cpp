#include "Codec_BCH.hpp"

#include "Factory/Module/Encoder/BCH/Encoder_BCH.hpp"
#include "Factory/Module/Decoder/BCH/Decoder_BCH.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Codec_BCH_name   = "Codec BCH";
const std::string aff3ct::factory::Codec_BCH_prefix = "cdc";

Codec_BCH::parameters
::parameters(const std::string &prefix)
: Codec          ::parameters(Codec_BCH_name, prefix),
  Codec_SIHO_HIHO::parameters(Codec_BCH_name, prefix)
{
	Codec::parameters::set_enc(new Encoder_BCH::parameters(""));
	Codec::parameters::set_dec(new Decoder_BCH::parameters(""));
}

Codec_BCH::parameters* Codec_BCH::parameters
::clone() const
{
	return new Codec_BCH::parameters(*this);
}

void Codec_BCH::parameters
::register_arguments(CLI::App &app)
{
	auto p = get_prefix();

	Codec_SIHO_HIHO::parameters::register_arguments(app);

	enc->register_arguments(*sub_enc);
	dec->register_arguments(*sub_dec);

	CLI::remove_option(sub_dec, "--cw-size"  , dec->get_prefix());
	CLI::remove_option(sub_dec, "--info-bits", dec->get_prefix());
	CLI::remove_option(sub_dec, "--fra"      , dec->get_prefix());

	CLI::get_option(sub_enc, "--info-bits", enc->get_prefix())->required(false);
	// CLI::get_option(sub_dec, "--corr-pow", dec->get_prefix())->excludes(CLI::get_option(sub_enc, "--info-bits", enc->get_prefix());
}

void Codec_BCH::parameters
::callback_arguments()
{
	Codec_SIHO_HIHO::parameters::callback_arguments();

	enc->callback_arguments();

	if (!sub_enc->get_option("--info-bits")->empty())
		dec->K = enc->K;

	dec->N_cw     = enc->N_cw;
	dec->n_frames = enc->n_frames;

	dec->callback_arguments();

	if (dec->K != 0) // when -T has been given but not -K
		enc->K = dec->K;

	K    = enc->K;
	N_cw = enc->N_cw;
	N    = enc->N_cw;
}

void Codec_BCH::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	Codec_SIHO_HIHO::parameters::get_headers(headers, full);

	enc->get_headers(headers, full);
	dec->get_headers(headers, full);
}

template <typename B, typename Q>
module::Codec_BCH<B,Q>* Codec_BCH::parameters
::build(module::CRC<B> *crc) const
{
	return new module::Codec_BCH<B,Q>(dynamic_cast<const Encoder_BCH::parameters&>(*enc),
	                                  dynamic_cast<const Decoder_BCH::parameters&>(*dec));
}

template <typename B, typename Q>
module::Codec_BCH<B,Q>* Codec_BCH
::build(const parameters &params, module::CRC<B> *crc)
{
	return params.template build<B,Q>(crc);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Codec_BCH<B_8 ,Q_8 >* aff3ct::factory::Codec_BCH::parameters::build<B_8 ,Q_8 >(aff3ct::module::CRC<B_8 >*) const;
template aff3ct::module::Codec_BCH<B_16,Q_16>* aff3ct::factory::Codec_BCH::parameters::build<B_16,Q_16>(aff3ct::module::CRC<B_16>*) const;
template aff3ct::module::Codec_BCH<B_32,Q_32>* aff3ct::factory::Codec_BCH::parameters::build<B_32,Q_32>(aff3ct::module::CRC<B_32>*) const;
template aff3ct::module::Codec_BCH<B_64,Q_64>* aff3ct::factory::Codec_BCH::parameters::build<B_64,Q_64>(aff3ct::module::CRC<B_64>*) const;
template aff3ct::module::Codec_BCH<B_8 ,Q_8 >* aff3ct::factory::Codec_BCH::build<B_8 ,Q_8 >(const aff3ct::factory::Codec_BCH::parameters&, aff3ct::module::CRC<B_8 >*);
template aff3ct::module::Codec_BCH<B_16,Q_16>* aff3ct::factory::Codec_BCH::build<B_16,Q_16>(const aff3ct::factory::Codec_BCH::parameters&, aff3ct::module::CRC<B_16>*);
template aff3ct::module::Codec_BCH<B_32,Q_32>* aff3ct::factory::Codec_BCH::build<B_32,Q_32>(const aff3ct::factory::Codec_BCH::parameters&, aff3ct::module::CRC<B_32>*);
template aff3ct::module::Codec_BCH<B_64,Q_64>* aff3ct::factory::Codec_BCH::build<B_64,Q_64>(const aff3ct::factory::Codec_BCH::parameters&, aff3ct::module::CRC<B_64>*);
#else
template aff3ct::module::Codec_BCH<B,Q>* aff3ct::factory::Codec_BCH::parameters::build<B,Q>(aff3ct::module::CRC<B>*) const;
template aff3ct::module::Codec_BCH<B,Q>* aff3ct::factory::Codec_BCH::build<B,Q>(const aff3ct::factory::Codec_BCH::parameters&, aff3ct::module::CRC<B>*);
#endif
// ==================================================================================== explicit template instantiation

