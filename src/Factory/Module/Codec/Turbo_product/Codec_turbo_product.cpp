#include "Codec_turbo_product.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Codec_turbo_product_name   = "Codec Turbo Prod";
const std::string aff3ct::factory::Codec_turbo_product_prefix = "cdc";

Codec_turbo_product::parameters
::parameters(const std::string &prefix)
: Codec          ::parameters(Codec_turbo_product_name, prefix),
  Codec_SISO_SIHO::parameters(Codec_turbo_product_name, prefix),
  enc(new Encoder_turbo_product::parameters("enc")),
  dec(new Decoder_turbo_product::parameters("dec"))
{
	Codec::parameters::enc = enc;
	Codec::parameters::dec = dec;
	Codec::parameters::itl = enc->itl;
	delete dec->itl; dec->itl = enc->itl;
}

Codec_turbo_product::parameters
::~parameters()
{
	if (enc != nullptr) { enc->itl = nullptr; delete enc; enc = nullptr; }
	if (dec != nullptr) { dec->itl = nullptr; delete dec; dec = nullptr; }

	Codec::parameters::enc = nullptr;
	Codec::parameters::dec = nullptr;
	if (Codec::parameters::itl != nullptr)
	{
		delete Codec::parameters::itl;
		Codec::parameters::itl = nullptr;
	}
}

Codec_turbo_product::parameters* Codec_turbo_product::parameters
::clone() const
{
	auto clone = new Codec_turbo_product::parameters(*this);

	if (enc != nullptr) { clone->enc = enc->clone(); }
	if (dec != nullptr) { clone->dec = dec->clone(); }

	clone->set_enc(clone->enc);
	clone->set_dec(clone->dec);
	clone->set_itl(clone->enc->itl);
	delete clone->dec->itl; clone->dec->itl = clone->enc->itl;

	return clone;
}

void Codec_turbo_product::parameters
::get_description(tools::Argument_map_info &args) const
{
	Codec_SIHO::parameters::get_description(args);

	enc->get_description(args);
	dec->get_description(args);

	auto pdec = dec->get_prefix();
	auto pdes = dec->sub->get_prefix();

	args.erase({pdes+"-cw-size",   "N"});
	args.erase({pdes+"-info-bits", "K"});
	args.erase({pdec+"-fra",       "F"});
	args.erase({pdec+"-ext",          });
}

void Codec_turbo_product::parameters
::store(const tools::Argument_map_value &vals)
{
	Codec_SIHO::parameters::store(vals);

	enc->store(vals);

	this->dec->sub->K           = this->enc->sub->K;
	this->dec->sub->N_cw        = this->enc->sub->N_cw;
	this->dec->n_frames         = this->enc->n_frames;
	this->dec->parity_extended  = this->enc->parity_extended;

	dec->store(vals);

	this->K    = this->enc->K;
	this->N_cw = this->enc->N_cw;
	this->N    = this->enc->N_cw;
}

void Codec_turbo_product::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	Codec_SIHO::parameters::get_headers(headers, full);
	enc->get_headers(headers, full);
	dec->get_headers(headers, full);
}

template <typename B, typename Q>
module::Codec_turbo_product<B,Q>* Codec_turbo_product::parameters
::build(module::CRC<B> *crc) const
{
	return new module::Codec_turbo_product<B,Q>(*enc, *dec);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
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
