#include "Codec_turbo_product.hpp"

#include "Factory/Module/Encoder/Turbo_product/Encoder_turbo_product.hpp"
#include "Factory/Module/Decoder/Turbo_product/Decoder_turbo_product.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Codec_turbo_product_name   = "Codec Turbo Prod";
const std::string aff3ct::factory::Codec_turbo_product_prefix = "cdc";

Codec_turbo_product::parameters
::parameters(const std::string &prefix)
: Codec          ::parameters(Codec_turbo_product_name, prefix),
  Codec_SISO_SIHO::parameters(Codec_turbo_product_name, prefix)
{
	auto enc_t = new Encoder_turbo_product::parameters("");
	auto dec_t = new Decoder_turbo_product::parameters("");
	enc_t->itl = nullptr;
	dec_t->itl = nullptr;

	Codec::parameters::set_enc(enc_t);
	Codec::parameters::set_dec(dec_t);
	Codec::parameters::set_itl(new Interleaver::parameters(""));
}

Codec_turbo_product::parameters* Codec_turbo_product::parameters
::clone() const
{
	return new Codec_turbo_product::parameters(*this);
}

void Codec_turbo_product::parameters
::register_arguments(CLI::App &app)
{
	Codec_SISO_SIHO::parameters::register_arguments(app);

	enc->register_arguments(*sub_enc);
	dec->register_arguments(*sub_dec);


	// auto dec_tur = dynamic_cast<Decoder_turbo_product::parameters*>(dec.get());
	// auto& decsub = dec_tur->sub; // sub decoder -> auto_cloned pointeur so need the ref & !

	CLI::remove_option(sub_dec, "--cw-size",   dec->get_prefix(), dec->no_argflag());
	CLI::remove_option(sub_dec, "--info-bits", dec->get_prefix(), dec->no_argflag());
	CLI::remove_option(sub_dec, "--fra",       dec->get_prefix(), dec->no_argflag());
	CLI::remove_option(sub_dec, "--ext",       dec->get_prefix(), dec->no_argflag());

	if (itl != nullptr)
	{
		itl->register_arguments(*sub_itl);

		CLI::remove_option(sub_itl, "--size", itl->get_prefix(), itl->no_argflag());
		CLI::remove_option(sub_itl, "--fra" , itl->get_prefix(), itl->no_argflag());
	}
}

void Codec_turbo_product::parameters
::callback_arguments()
{
	Codec_SIHO::parameters::callback_arguments();

	auto enc_tur = dynamic_cast<Encoder_turbo_product::parameters*>(enc.get());
	auto dec_tur = dynamic_cast<Decoder_turbo_product::parameters*>(dec.get());

	enc->callback_arguments();

	dec_tur->K                = enc_tur->sub->K;
	dec_tur->N_cw             = enc_tur->sub->N_cw;
	dec_tur->n_frames         = enc_tur->n_frames;
	dec_tur->parity_extended  = enc_tur->parity_extended;

	dec->callback_arguments();

	if (itl != nullptr)
	{
		itl->core->n_frames = enc_tur->n_frames;
		itl->core->type     = "ROW_COL";

		if (enc_tur->parity_extended)
			itl->core->n_cols = enc_tur->sub->N_cw +1;
		else
			itl->core->n_cols = enc_tur->sub->N_cw;

		itl->core->size = itl->core->n_cols * itl->core->n_cols;
		enc->N_cw = itl->core->size;
		dec->N_cw = itl->core->size;

		itl->callback_arguments();
	}

	K    = enc->K;
	N_cw = enc->N_cw;
	N    = enc->N_cw;
}

void Codec_turbo_product::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	Codec_SIHO::parameters::get_headers(headers, full);
	enc->get_headers(headers, full);
	dec->get_headers(headers, full);

	if (itl != nullptr)
		itl->get_headers(headers, full);
}

template <typename B, typename Q>
module::Codec_turbo_product<B,Q>* Codec_turbo_product::parameters
::build(module::CRC<B> *crc) const
{
	return new module::Codec_turbo_product<B,Q>(dynamic_cast<const Encoder_turbo_product::parameters&>(*enc),
	                                            dynamic_cast<const Decoder_turbo_product::parameters&>(*dec),
	                                            *itl);
}


template <typename B, typename Q>
module::Codec_turbo_product<B,Q>* Codec_turbo_product
::build(const parameters &params, module::CRC<B> *crc)
{
	return params.template build<B,Q>();
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Codec_turbo_product<B_8 ,Q_8 >* aff3ct::factory::Codec_turbo_product::parameters::build<B_8 ,Q_8 >(module::CRC<B_8 > *) const;
template aff3ct::module::Codec_turbo_product<B_16,Q_16>* aff3ct::factory::Codec_turbo_product::parameters::build<B_16,Q_16>(module::CRC<B_16> *) const;
template aff3ct::module::Codec_turbo_product<B_32,Q_32>* aff3ct::factory::Codec_turbo_product::parameters::build<B_32,Q_32>(module::CRC<B_32> *) const;
template aff3ct::module::Codec_turbo_product<B_64,Q_64>* aff3ct::factory::Codec_turbo_product::parameters::build<B_64,Q_64>(module::CRC<B_64> *) const;
template aff3ct::module::Codec_turbo_product<B_8 ,Q_8 >* aff3ct::factory::Codec_turbo_product::build<B_8 ,Q_8 >(const aff3ct::factory::Codec_turbo_product::parameters&, module::CRC<B_8 > *);
template aff3ct::module::Codec_turbo_product<B_16,Q_16>* aff3ct::factory::Codec_turbo_product::build<B_16,Q_16>(const aff3ct::factory::Codec_turbo_product::parameters&, module::CRC<B_16> *);
template aff3ct::module::Codec_turbo_product<B_32,Q_32>* aff3ct::factory::Codec_turbo_product::build<B_32,Q_32>(const aff3ct::factory::Codec_turbo_product::parameters&, module::CRC<B_32> *);
template aff3ct::module::Codec_turbo_product<B_64,Q_64>* aff3ct::factory::Codec_turbo_product::build<B_64,Q_64>(const aff3ct::factory::Codec_turbo_product::parameters&, module::CRC<B_64> *);
#else
template aff3ct::module::Codec_turbo_product<B,Q>* aff3ct::factory::Codec_turbo_product::parameters::build<B,Q>(module::CRC<B> *) const;
template aff3ct::module::Codec_turbo_product<B,Q>* aff3ct::factory::Codec_turbo_product::build<B,Q>(const aff3ct::factory::Codec_turbo_product::parameters&, module::CRC<B> *);
#endif
// ==================================================================================== explicit template instantiation

