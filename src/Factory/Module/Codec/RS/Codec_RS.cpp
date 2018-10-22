#include "Codec_RS.hpp"

#include "Factory/Module/Encoder/RS/Encoder_RS.hpp"
#include "Factory/Module/Decoder/RS/Decoder_RS.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Codec_RS_name   = "Codec RS";
const std::string aff3ct::factory::Codec_RS_prefix = "cdc";

Codec_RS::parameters
::parameters(const std::string &prefix)
: Codec          ::parameters(Codec_RS_name, prefix),
  Codec_SIHO_HIHO::parameters(Codec_RS_name, prefix)
{
	Codec::parameters::set_enc(new Encoder_RS::parameters(""));
	Codec::parameters::set_dec(new Decoder_RS::parameters(""));
}

Codec_RS::parameters* Codec_RS::parameters
::clone() const
{
	return new Codec_RS::parameters(*this);
}

void Codec_RS::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();

	Codec_SIHO_HIHO::parameters::register_arguments(app);

	enc->register_arguments(*sub_enc);
	dec->register_arguments(*sub_dec);

	CLI::remove_option(sub_dec, "--cw-size"  , dec->get_prefix(), dec->no_argflag());
	CLI::remove_option(sub_dec, "--info-bits", dec->get_prefix(), dec->no_argflag());
	CLI::remove_option(sub_dec, "--fra"      , dec->get_prefix(), dec->no_argflag());

	CLI::get_option(sub_enc, "--info-bits", enc->get_prefix(), enc->no_argflag())->required(false);
	// CLI::get_option(sub_dec, "--corr-pow", dec->get_prefix(), dec->no_argflag())->excludes(CLI::get_option(sub_enc, "--info-bits", enc->get_prefix(), enc->no_argflag());
}

void Codec_RS::parameters
::callback_arguments()
{
	Codec_SIHO_HIHO::parameters::callback_arguments();

	auto dec_rs = dynamic_cast<Decoder_RS::parameters*>(dec.get());

	enc->callback_arguments();

	dec->K        = enc->K;
	dec->N_cw     = enc->N_cw;
	dec->n_frames = enc->n_frames;

	dec->callback_arguments();

	if (dec->K != enc->K) // when -T has been given but not -K
		enc->K = dec->K;

	K    = enc->K    * dec_rs->m;
	N_cw = enc->N_cw * dec_rs->m;
	N    = enc->N_cw * dec_rs->m;
}

void Codec_RS::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_name();

	Codec_SIHO_HIHO::parameters::get_headers(headers, full);

	headers[p].push_back(std::make_pair("Symbols Source size",   std::to_string(enc->K   )));
	headers[p].push_back(std::make_pair("Symbols Codeword size", std::to_string(enc->N_cw)));

	enc->get_headers(headers, full);
	dec->get_headers(headers, full);
}

template <typename B, typename Q>
module::Codec_RS<B,Q>* Codec_RS::parameters
::build(module::CRC<B> *crc) const
{
	return new module::Codec_RS<B,Q>(dynamic_cast<const Encoder_RS::parameters&>(*enc),
	                                 dynamic_cast<const Decoder_RS::parameters&>(*dec));
}

template <typename B, typename Q>
module::Codec_RS<B,Q>* Codec_RS
::build(const parameters &params, module::CRC<B> *crc)
{
	return params.template build<B,Q>(crc);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Codec_RS<B_8 ,Q_8 >* aff3ct::factory::Codec_RS::parameters::build<B_8 ,Q_8 >(aff3ct::module::CRC<B_8 >*) const;
template aff3ct::module::Codec_RS<B_16,Q_16>* aff3ct::factory::Codec_RS::parameters::build<B_16,Q_16>(aff3ct::module::CRC<B_16>*) const;
template aff3ct::module::Codec_RS<B_32,Q_32>* aff3ct::factory::Codec_RS::parameters::build<B_32,Q_32>(aff3ct::module::CRC<B_32>*) const;
template aff3ct::module::Codec_RS<B_64,Q_64>* aff3ct::factory::Codec_RS::parameters::build<B_64,Q_64>(aff3ct::module::CRC<B_64>*) const;
template aff3ct::module::Codec_RS<B_8 ,Q_8 >* aff3ct::factory::Codec_RS::build<B_8 ,Q_8 >(const aff3ct::factory::Codec_RS::parameters&, aff3ct::module::CRC<B_8 >*);
template aff3ct::module::Codec_RS<B_16,Q_16>* aff3ct::factory::Codec_RS::build<B_16,Q_16>(const aff3ct::factory::Codec_RS::parameters&, aff3ct::module::CRC<B_16>*);
template aff3ct::module::Codec_RS<B_32,Q_32>* aff3ct::factory::Codec_RS::build<B_32,Q_32>(const aff3ct::factory::Codec_RS::parameters&, aff3ct::module::CRC<B_32>*);
template aff3ct::module::Codec_RS<B_64,Q_64>* aff3ct::factory::Codec_RS::build<B_64,Q_64>(const aff3ct::factory::Codec_RS::parameters&, aff3ct::module::CRC<B_64>*);
#else
template aff3ct::module::Codec_RS<B,Q>* aff3ct::factory::Codec_RS::parameters::build<B,Q>(aff3ct::module::CRC<B>*) const;
template aff3ct::module::Codec_RS<B,Q>* aff3ct::factory::Codec_RS::build<B,Q>(const aff3ct::factory::Codec_RS::parameters&, aff3ct::module::CRC<B>*);
#endif
// ==================================================================================== explicit template instantiation

