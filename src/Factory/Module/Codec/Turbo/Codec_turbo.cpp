#include "Codec_turbo.hpp"

#include "Factory/Module/Encoder/Turbo/Encoder_turbo.hpp"
#include "Factory/Module/Decoder/Turbo/Decoder_turbo.hpp"
#include "Factory/Module/Puncturer/Turbo/Puncturer_turbo.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Codec_turbo_name   = "Codec Turbo";
const std::string aff3ct::factory::Codec_turbo_prefix = "cdc";

Codec_turbo::parameters
::parameters(const std::string &prefix)
: Codec     ::parameters(Codec_turbo_name, prefix),
  Codec_SIHO::parameters(Codec_turbo_name, prefix)
{
	auto enc_t = new Encoder_turbo::parameters<>("");
	auto dec_t = new Decoder_turbo::parameters<>("");
	enc_t->itl = nullptr;
	dec_t->itl = nullptr;

	Codec::parameters::set_enc(enc_t);
	Codec::parameters::set_dec(dec_t);
	Codec::parameters::set_itl(new Interleaver::parameters(""));
}

Codec_turbo::parameters* Codec_turbo::parameters
::clone() const
{
	return new Codec_turbo::parameters(*this);
}

void Codec_turbo::parameters
::enable_puncturer()
{
	set_pct(new Puncturer_turbo::parameters(""));
}

void Codec_turbo::parameters
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
	}

	enc->register_arguments(*sub_enc);
	dec->register_arguments(*sub_dec);


	CLI::remove_option(sub_dec, "--info-bits", dec->get_prefix(), dec->no_argflag());
	CLI::remove_option(sub_dec, "--fra"      , dec->get_prefix(), dec->no_argflag());
	CLI::remove_option(sub_dec, "--json"     , dec->get_prefix(), dec->no_argflag());


	// auto dec_tur = dynamic_cast<Decoder_turbo::parameters<>*>(dec.get());
	// auto des = dec_tur->sub1; // sub decoder

	// CLI::remove_option(sub_des, "--no-buff"  , des->get_prefix());
	// CLI::remove_option(sub_des, "--poly"     , des->get_prefix());
	// CLI::remove_option(sub_des, "--std"      , des->get_prefix());


	if (itl != nullptr)
	{
		itl->register_arguments(*sub_itl);

		CLI::remove_option(sub_itl, "--size", itl->get_prefix(), itl->no_argflag());
		CLI::remove_option(sub_itl, "--fra" , itl->get_prefix(), itl->no_argflag());
	}
}

void Codec_turbo::parameters
::callback_arguments()
{
	Codec_SIHO::parameters::callback_arguments();

	auto enc_tur = dynamic_cast<Encoder_turbo::parameters<>*>(enc.get());
	auto dec_tur = dynamic_cast<Decoder_turbo::parameters<>*>(dec.get());

	enc->callback_arguments();

	if (pct != nullptr)
	{
		auto pct_tur = dynamic_cast<Puncturer_turbo::parameters*>(pct.get());

		pct_tur->K            = enc_tur->K;
		pct_tur->N_cw         = enc_tur->N_cw;
		pct_tur->not_buffered =!enc_tur->sub1->not_buffered;
		pct_tur->n_frames     = enc_tur->n_frames;
		pct_tur->tail_length  = enc_tur->tail_length;

		pct->callback_arguments();
	}

	dec_tur->K                  = enc_tur->K;
	dec_tur->N_cw               = enc_tur->N_cw;
	dec_tur->sub1->not_buffered = enc_tur->sub1->not_buffered;
	dec_tur->sub2->not_buffered = enc_tur->sub2->not_buffered;
	dec_tur->n_frames           = enc_tur->n_frames;
	dec_tur->sub1->n_frames     = enc_tur->sub1->n_frames;
	dec_tur->sub2->n_frames     = enc_tur->sub2->n_frames;
	dec_tur->tail_length        = enc_tur->tail_length;
	dec_tur->sub1->tail_length  = enc_tur->sub1->tail_length;
	dec_tur->sub2->tail_length  = enc_tur->sub2->tail_length;
	dec_tur->sub1->poly         = enc_tur->sub1->poly;
	dec_tur->sub2->poly         = enc_tur->sub2->poly;
	dec_tur->sub1->standard     = enc_tur->sub1->standard;
	dec_tur->sub2->standard     = enc_tur->sub2->standard;
	dec_tur->enable_json        =!enc_tur->json_path.empty();

	dec->callback_arguments();

	K           = enc->K;
	N_cw        = enc->N_cw;
	N           = pct != nullptr ? pct->N : enc->N_cw;
	tail_length = enc->tail_length;

	if (itl != nullptr)
	{
		itl->core->size     = enc->K;
		itl->core->n_frames = enc->n_frames;

		itl->callback_arguments();

		if (!itl->core->type_option_set_by_user())
		{
			if (enc_tur->sub1->standard == "LTE")
				itl->core->type = "LTE";
			else if (enc_tur->sub1->standard == "CCSDS")
				itl->core->type = "CCSDS";
		}
	}
}

void Codec_turbo::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	Codec_SIHO::parameters::get_headers(headers, full);

	enc->get_headers(headers, full);
	dec->get_headers(headers, full);

	if (pct != nullptr)
		pct->get_headers(headers, full);

	if (itl != nullptr)
		itl->get_headers(headers, true);
}

template <typename B, typename Q>
module::Codec_turbo<B,Q>* Codec_turbo::parameters
::build(module::CRC<B> *crc) const
{
	return new module::Codec_turbo<B,Q>(dynamic_cast<const Encoder_turbo  ::parameters<>&>(*enc),
	                                    dynamic_cast<const Decoder_turbo  ::parameters<>&>(*dec),
	                                    *itl,
	                                    dynamic_cast<const Puncturer_turbo::parameters*  >(pct.get()),
	                                    crc);
}


template <typename B, typename Q>
module::Codec_turbo<B,Q>* Codec_turbo
::build(const parameters &params, module::CRC<B> *crc)
{
	return params.template build<B,Q>(crc);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Codec_turbo<B_8 ,Q_8 >* aff3ct::factory::Codec_turbo::parameters::build<B_8 ,Q_8 >(aff3ct::module::CRC<B_8 >*) const;
template aff3ct::module::Codec_turbo<B_16,Q_16>* aff3ct::factory::Codec_turbo::parameters::build<B_16,Q_16>(aff3ct::module::CRC<B_16>*) const;
template aff3ct::module::Codec_turbo<B_32,Q_32>* aff3ct::factory::Codec_turbo::parameters::build<B_32,Q_32>(aff3ct::module::CRC<B_32>*) const;
template aff3ct::module::Codec_turbo<B_64,Q_64>* aff3ct::factory::Codec_turbo::parameters::build<B_64,Q_64>(aff3ct::module::CRC<B_64>*) const;
template aff3ct::module::Codec_turbo<B_8 ,Q_8 >* aff3ct::factory::Codec_turbo::build<B_8 ,Q_8 >(const aff3ct::factory::Codec_turbo::parameters&, aff3ct::module::CRC<B_8 >*);
template aff3ct::module::Codec_turbo<B_16,Q_16>* aff3ct::factory::Codec_turbo::build<B_16,Q_16>(const aff3ct::factory::Codec_turbo::parameters&, aff3ct::module::CRC<B_16>*);
template aff3ct::module::Codec_turbo<B_32,Q_32>* aff3ct::factory::Codec_turbo::build<B_32,Q_32>(const aff3ct::factory::Codec_turbo::parameters&, aff3ct::module::CRC<B_32>*);
template aff3ct::module::Codec_turbo<B_64,Q_64>* aff3ct::factory::Codec_turbo::build<B_64,Q_64>(const aff3ct::factory::Codec_turbo::parameters&, aff3ct::module::CRC<B_64>*);
#else
template aff3ct::module::Codec_turbo<B,Q>* aff3ct::factory::Codec_turbo::parameters::build<B,Q>(aff3ct::module::CRC<B>*) const;
template aff3ct::module::Codec_turbo<B,Q>* aff3ct::factory::Codec_turbo::build<B,Q>(const aff3ct::factory::Codec_turbo::parameters&, aff3ct::module::CRC<B>*);
#endif
// ==================================================================================== explicit template instantiation

