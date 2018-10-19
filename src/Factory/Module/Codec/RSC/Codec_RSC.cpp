#include "Codec_RSC.hpp"

#include "Factory/Module/Encoder/RSC/Encoder_RSC.hpp"
#include "Factory/Module/Decoder/RSC/Decoder_RSC.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Codec_RSC_name   = "Codec RSC";
const std::string aff3ct::factory::Codec_RSC_prefix = "cdc";

Codec_RSC::parameters
::parameters(const std::string &prefix)
: Codec          ::parameters(Codec_RSC_name, prefix),
  Codec_SISO_SIHO::parameters(Codec_RSC_name, prefix)
{
	Codec::parameters::set_enc(new Encoder_RSC::parameters(""));
	Codec::parameters::set_dec(new Decoder_RSC::parameters(""));
}

Codec_RSC::parameters* Codec_RSC::parameters
::clone() const
{
	return new Codec_RSC::parameters(*this);
}

void Codec_RSC::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();

	Codec_SISO_SIHO::parameters::register_arguments(app);

	// enc->register_arguments(*sub_enc);
	dec->register_arguments(*sub_dec);

	CLI::remove_option(sub_dec, "--cw-size"  , dec->get_prefix(), dec->no_argflag());
	CLI::remove_option(sub_dec, "--info-bits", dec->get_prefix(), dec->no_argflag());
	CLI::remove_option(sub_dec, "--fra"      , dec->get_prefix(), dec->no_argflag());
	CLI::remove_option(sub_dec, "--no-buff"  , dec->get_prefix(), dec->no_argflag());
	CLI::remove_option(sub_dec, "--poly"     , dec->get_prefix(), dec->no_argflag());
	CLI::remove_option(sub_dec, "--std"      , dec->get_prefix(), dec->no_argflag());
}

void Codec_RSC::parameters
::callback_arguments()
{
	Codec_SISO_SIHO::parameters::callback_arguments();

	auto enc_rsc = dynamic_cast<Encoder_RSC::parameters*>(enc.get());
	auto dec_rsc = dynamic_cast<Decoder_RSC::parameters*>(dec.get());

	enc->callback_arguments();

	dec_rsc->K            = enc_rsc->K;
	dec_rsc->N_cw         = enc_rsc->N_cw;
	dec_rsc->n_frames     = enc_rsc->n_frames;
	dec_rsc->not_buffered = enc_rsc->not_buffered;
	dec_rsc->poly         = enc_rsc->poly;
	dec_rsc->standard     = enc_rsc->standard;

	dec->callback_arguments();

	K           = enc->K;
	N_cw        = enc->N_cw;
	N           = enc->N_cw;
	tail_length = enc->tail_length;
}

void Codec_RSC::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	Codec_SISO_SIHO::parameters::get_headers(headers, full);

	enc->get_headers(headers, full);
	dec->get_headers(headers, full);
}

template <typename B, typename Q>
module::Codec_RSC<B,Q>* Codec_RSC::parameters
::build(module::CRC<B>* crc) const
{
	return new module::Codec_RSC<B,Q>(dynamic_cast<const Encoder_RSC::parameters&>(*enc),
	                                  dynamic_cast<const Decoder_RSC::parameters&>(*dec));
}

template <typename B, typename Q>
module::Codec_RSC<B,Q>* Codec_RSC
::build(const parameters &params, module::CRC<B>* crc)
{
	return params.template build<B,Q>(crc);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Codec_RSC<B_8 ,Q_8 >* aff3ct::factory::Codec_RSC::parameters::build<B_8 ,Q_8 >(aff3ct::module::CRC<B_8 >*) const;
template aff3ct::module::Codec_RSC<B_16,Q_16>* aff3ct::factory::Codec_RSC::parameters::build<B_16,Q_16>(aff3ct::module::CRC<B_16>*) const;
template aff3ct::module::Codec_RSC<B_32,Q_32>* aff3ct::factory::Codec_RSC::parameters::build<B_32,Q_32>(aff3ct::module::CRC<B_32>*) const;
template aff3ct::module::Codec_RSC<B_64,Q_64>* aff3ct::factory::Codec_RSC::parameters::build<B_64,Q_64>(aff3ct::module::CRC<B_64>*) const;
template aff3ct::module::Codec_RSC<B_8 ,Q_8 >* aff3ct::factory::Codec_RSC::build<B_8 ,Q_8 >(const aff3ct::factory::Codec_RSC::parameters&, aff3ct::module::CRC<B_8 >*);
template aff3ct::module::Codec_RSC<B_16,Q_16>* aff3ct::factory::Codec_RSC::build<B_16,Q_16>(const aff3ct::factory::Codec_RSC::parameters&, aff3ct::module::CRC<B_16>*);
template aff3ct::module::Codec_RSC<B_32,Q_32>* aff3ct::factory::Codec_RSC::build<B_32,Q_32>(const aff3ct::factory::Codec_RSC::parameters&, aff3ct::module::CRC<B_32>*);
template aff3ct::module::Codec_RSC<B_64,Q_64>* aff3ct::factory::Codec_RSC::build<B_64,Q_64>(const aff3ct::factory::Codec_RSC::parameters&, aff3ct::module::CRC<B_64>*);
#else
template aff3ct::module::Codec_RSC<B,Q>* aff3ct::factory::Codec_RSC::parameters::build<B,Q>(aff3ct::module::CRC<B>*) const;
template aff3ct::module::Codec_RSC<B,Q>* aff3ct::factory::Codec_RSC::build<B,Q>(const aff3ct::factory::Codec_RSC::parameters&, aff3ct::module::CRC<B>*);
#endif
// ==================================================================================== explicit template instantiation
