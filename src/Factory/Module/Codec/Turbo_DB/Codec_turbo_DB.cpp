#include "Codec_turbo_DB.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Codec_turbo_DB_name   = "Codec Turbo DB";
const std::string aff3ct::factory::Codec_turbo_DB_prefix = "cdc";

Codec_turbo_DB::parameters
::parameters(const std::string &prefix)
: Codec     ::parameters(Codec_turbo_DB_name, prefix),
  Codec_SIHO::parameters(Codec_turbo_DB_name, prefix)
{
	auto enc_t = new Encoder_turbo_DB::parameters("");
	auto dec_t = new Decoder_turbo_DB::parameters("");
	enc_t->itl = nullptr;
	dec_t->itl = nullptr;

	Codec::parameters::set_enc(enc_t);
	Codec::parameters::set_dec(dec_t);
	Codec::parameters::set_itl(new Interleaver::parameters(""));
}

Codec_turbo_DB::parameters* Codec_turbo_DB::parameters
::clone() const
{
	return new Codec_turbo_DB::parameters(*this);
}

void Codec_turbo_DB::parameters
::enable_puncturer()
{
	set_pct(new Puncturer_turbo_DB::parameters(""));
}

void Codec_turbo_DB::parameters
::register_arguments(CLI::App &app)
{
	Codec_SIHO::parameters::register_arguments(app);

	if (pct != nullptr)
	{
		pct->register_arguments(*sub_pct);

		CLI::remove_option(sub_pct, "--info-bits"  , pct->get_prefix(), pct->no_argflag());
		CLI::remove_option(sub_pct, "--fra"        , pct->get_prefix(), pct->no_argflag());
		CLI::remove_option(sub_pct, "--no-buff"    , pct->get_prefix(), pct->no_argflag());
		CLI::remove_option(sub_pct, "--tail-length", pct->get_prefix(), pct->no_argflag());

		CLI::get_option(sub_pct, "--fra-size", pct->get_prefix(), pct->no_argflag())->required(false);
	}

	enc->register_arguments(*sub_enc);
	dec->register_arguments(*sub_dec);

	CLI::remove_option(sub_dec, "--info-bits", dec->get_prefix(), dec->no_argflag());
	CLI::remove_option(sub_dec, "--fra"      , dec->get_prefix(), dec->no_argflag());


	// auto dec_tur = dynamic_cast<Decoder_turbo_DB::parameters*>(dec.get());
	// auto des = dec_tur->sub; // sub decoder

	// CLI::remove_option(sub_des, "--no-buff"  , des->get_prefix());
	// CLI::remove_option(sub_des, "--std"      , des->get_prefix());

	if (itl != nullptr)
	{
		itl->register_arguments(*sub_itl);

		CLI::remove_option(sub_itl, "--size", itl->get_prefix(), itl->no_argflag());
		CLI::remove_option(sub_itl, "--fra" , itl->get_prefix(), itl->no_argflag());
	}
}

void Codec_turbo_DB::parameters
::callback_arguments()
{
	Codec_SIHO::parameters::callback_arguments();

	auto enc_tur = dynamic_cast<Encoder_turbo_DB::parameters*>(enc.get());
	auto dec_tur = dynamic_cast<Decoder_turbo_DB::parameters*>(dec.get());

	enc->callback_arguments();

	if (pct != nullptr)
	{
		pct->K        = enc->K;
		pct->N        = enc->N_cw;
		pct->N_cw     = enc->N_cw;
		pct->n_frames = enc->n_frames;

		pct->callback_arguments();
	}

	dec_tur->K                 = enc_tur->K;
	dec_tur->N_cw              = enc_tur->N_cw;
	dec_tur->sub->not_buffered = enc_tur->sub->not_buffered;
	dec_tur->n_frames          = enc_tur->n_frames;
	dec_tur->sub->n_frames     = enc_tur->sub->n_frames;

	dec->callback_arguments();

	auto pdes = dec_tur->sub->get_prefix();

	if (!enc_tur->sub->standard.empty() && !vals.exist({pdes+"-implem"}))
		dec_tur->sub->implem = enc_tur->sub->standard;
	enc_tur->sub->standard = dec_tur->sub->implem;

	K    = enc->K;
	N_cw = enc->N_cw;
	N    = pct != nullptr ? pct->N : enc->N_cw;


	if (itl != nullptr)
	{
		itl->core->size     = enc->K >> 1;
		itl->core->n_frames = enc->n_frames;

		itl->callback_arguments();

		if (!itl->core->type_option_set_by_user())
		{
			if (enc_tur->sub->type == "DVB-RCS1")
				itl->core->type = "DVB-RCS1";
			else if (enc_tur->sub->type == "DVB-RCS2")
				itl->core->type = "DVB-RCS2";
		}
	}
}

void Codec_turbo_DB::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	Codec_SIHO::parameters::get_headers(headers, full);

	enc->get_headers(headers, full);
	dec->get_headers(headers, full);
	if (pct != nullptr)
		pct->get_headers(headers, full);
	if (itl != nullptr)
		itl->get_headers(headers, full);
}

template <typename B, typename Q>
module::Codec_turbo_DB<B,Q>* Codec_turbo_DB::parameters
::build(module::CRC<B> *crc) const
{
	return new module::Codec_turbo_DB<B,Q>(dynamic_cast<const Encoder_turbo_DB  ::parameters&>(*enc),
	                                       dynamic_cast<const Decoder_turbo_DB  ::parameters&>(*dec),
	                                       *itl,
	                                       dynamic_cast<const Puncturer_turbo_DB::parameters*>(pct.get()),
	                                       crc);
}

template <typename B, typename Q>
module::Codec_turbo_DB<B,Q>* Codec_turbo_DB
::build(const parameters &params, module::CRC<B> *crc)
{
	return params.template build<B,Q>(crc);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Codec_turbo_DB<B_8 ,Q_8 >* aff3ct::factory::Codec_turbo_DB::parameters::build<B_8 ,Q_8 >(aff3ct::module::CRC<B_8 >*) const;
template aff3ct::module::Codec_turbo_DB<B_16,Q_16>* aff3ct::factory::Codec_turbo_DB::parameters::build<B_16,Q_16>(aff3ct::module::CRC<B_16>*) const;
template aff3ct::module::Codec_turbo_DB<B_32,Q_32>* aff3ct::factory::Codec_turbo_DB::parameters::build<B_32,Q_32>(aff3ct::module::CRC<B_32>*) const;
template aff3ct::module::Codec_turbo_DB<B_64,Q_64>* aff3ct::factory::Codec_turbo_DB::parameters::build<B_64,Q_64>(aff3ct::module::CRC<B_64>*) const;
template aff3ct::module::Codec_turbo_DB<B_8 ,Q_8 >* aff3ct::factory::Codec_turbo_DB::build<B_8 ,Q_8 >(const aff3ct::factory::Codec_turbo_DB::parameters&, aff3ct::module::CRC<B_8 >*);
template aff3ct::module::Codec_turbo_DB<B_16,Q_16>* aff3ct::factory::Codec_turbo_DB::build<B_16,Q_16>(const aff3ct::factory::Codec_turbo_DB::parameters&, aff3ct::module::CRC<B_16>*);
template aff3ct::module::Codec_turbo_DB<B_32,Q_32>* aff3ct::factory::Codec_turbo_DB::build<B_32,Q_32>(const aff3ct::factory::Codec_turbo_DB::parameters&, aff3ct::module::CRC<B_32>*);
template aff3ct::module::Codec_turbo_DB<B_64,Q_64>* aff3ct::factory::Codec_turbo_DB::build<B_64,Q_64>(const aff3ct::factory::Codec_turbo_DB::parameters&, aff3ct::module::CRC<B_64>*);
#else
template aff3ct::module::Codec_turbo_DB<B,Q>* aff3ct::factory::Codec_turbo_DB::parameters::build<B,Q>(aff3ct::module::CRC<B>*) const;
template aff3ct::module::Codec_turbo_DB<B,Q>* aff3ct::factory::Codec_turbo_DB::build<B,Q>(const aff3ct::factory::Codec_turbo_DB::parameters&, aff3ct::module::CRC<B>*);
#endif
// ==================================================================================== explicit template instantiation
