#include "Codec_uncoded.hpp"

#include "Factory/Module/Decoder/Hard_decision/Decoder_HD.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Codec_uncoded_name   = "Codec Uncoded";
const std::string aff3ct::factory::Codec_uncoded_prefix = "cdc";

Codec_uncoded::parameters
::parameters(const std::string &prefix)
: Codec          ::parameters(Codec_uncoded_name, prefix),
  Codec_SISO_SIHO::parameters(Codec_uncoded_name, prefix)
{
	Codec::parameters::set_enc(new Encoder   ::parameters(""));
	Codec::parameters::set_dec(new Decoder_HD::parameters(""));

	// default value
	enc->type = "NO";
}

Codec_uncoded::parameters* Codec_uncoded::parameters
::clone() const
{
	return new Codec_uncoded::parameters(*this);
}

void Codec_uncoded::parameters
::register_arguments(CLI::App &app)
{
	Codec_SISO_SIHO::parameters::register_arguments(app);

	enc->register_arguments(*sub_enc);
	dec->register_arguments(*sub_dec);

	CLI::remove_option(sub_enc, "--cw-size", enc->get_prefix(), enc->no_argflag());
	CLI::remove_option(sub_enc, "--path"   , enc->get_prefix(), enc->no_argflag());
	CLI::remove_option(sub_enc, "--type"   , enc->get_prefix(), enc->no_argflag());
	CLI::remove_option(sub_enc, "--seed"   , enc->get_prefix(), enc->no_argflag());

	CLI::remove_option(sub_dec, "--cw-size"  , dec->get_prefix(), dec->no_argflag());
	CLI::remove_option(sub_dec, "--info-bits", dec->get_prefix(), dec->no_argflag());
	CLI::remove_option(sub_dec, "--fra"      , dec->get_prefix(), dec->no_argflag());
}

void Codec_uncoded::parameters
::callback_arguments()
{
	Codec_SISO_SIHO::parameters::callback_arguments();

	enc->callback_arguments();

	enc->N_cw     = enc->K;
	dec->K        = enc->K;
	dec->N_cw     = enc->N_cw;
	dec->n_frames = enc->n_frames;

	dec->callback_arguments();

	K    = enc->K;
	N_cw = enc->N_cw;
	N    = enc->N_cw;
}

void Codec_uncoded::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	Codec_SISO_SIHO::parameters::get_headers(headers, full);

	enc->get_headers(headers, full);
	dec->get_headers(headers, full);
}

template <typename B, typename Q>
module::Codec_uncoded<B,Q>* Codec_uncoded::parameters
::build(module::CRC<B>* crc) const
{
	return new module::Codec_uncoded<B,Q>(*enc, dynamic_cast<const Decoder_HD::parameters&>(*dec));
}

template <typename B, typename Q>
module::Codec_uncoded<B,Q>* Codec_uncoded
::build(const parameters &params, module::CRC<B>* crc)
{
	return params.template build<B,Q>(crc);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Codec_uncoded<B_8 ,Q_8 >* aff3ct::factory::Codec_uncoded::parameters::build<B_8 ,Q_8 >(aff3ct::module::CRC<B_8 >*) const;
template aff3ct::module::Codec_uncoded<B_16,Q_16>* aff3ct::factory::Codec_uncoded::parameters::build<B_16,Q_16>(aff3ct::module::CRC<B_16>*) const;
template aff3ct::module::Codec_uncoded<B_32,Q_32>* aff3ct::factory::Codec_uncoded::parameters::build<B_32,Q_32>(aff3ct::module::CRC<B_32>*) const;
template aff3ct::module::Codec_uncoded<B_64,Q_64>* aff3ct::factory::Codec_uncoded::parameters::build<B_64,Q_64>(aff3ct::module::CRC<B_64>*) const;
template aff3ct::module::Codec_uncoded<B_8 ,Q_8 >* aff3ct::factory::Codec_uncoded::build<B_8 ,Q_8 >(const aff3ct::factory::Codec_uncoded::parameters&, aff3ct::module::CRC<B_8 >*);
template aff3ct::module::Codec_uncoded<B_16,Q_16>* aff3ct::factory::Codec_uncoded::build<B_16,Q_16>(const aff3ct::factory::Codec_uncoded::parameters&, aff3ct::module::CRC<B_16>*);
template aff3ct::module::Codec_uncoded<B_32,Q_32>* aff3ct::factory::Codec_uncoded::build<B_32,Q_32>(const aff3ct::factory::Codec_uncoded::parameters&, aff3ct::module::CRC<B_32>*);
template aff3ct::module::Codec_uncoded<B_64,Q_64>* aff3ct::factory::Codec_uncoded::build<B_64,Q_64>(const aff3ct::factory::Codec_uncoded::parameters&, aff3ct::module::CRC<B_64>*);
#else
template aff3ct::module::Codec_uncoded<B,Q>* aff3ct::factory::Codec_uncoded::parameters::build<B,Q>(aff3ct::module::CRC<B>*) const;
template aff3ct::module::Codec_uncoded<B,Q>* aff3ct::factory::Codec_uncoded::build<B,Q>(const aff3ct::factory::Codec_uncoded::parameters&, aff3ct::module::CRC<B>*);
#endif
// ==================================================================================== explicit template instantiation
