#include "Codec_polar.hpp"

#include "Factory/Module/Encoder/Polar/Encoder_polar.hpp"
#include "Factory/Module/Decoder/Polar/Decoder_polar.hpp"
#include "Factory/Module/Puncturer/Polar/Puncturer_polar.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Codec_polar_name   = "Codec Polar";
const std::string aff3ct::factory::Codec_polar_prefix = "cdc";

Codec_polar::parameters
::parameters(const std::string &prefix)
: Codec          ::parameters(Codec_polar_name, prefix),
  Codec_SISO_SIHO::parameters(Codec_polar_name, prefix)
{
	Codec::parameters::set_enc(new Encoder_polar::parameters(""));
	Codec::parameters::set_dec(new Decoder_polar::parameters(""));

  	fbg = new Frozenbits_generator::parameters("fb");
}

Codec_polar::parameters* Codec_polar::parameters
::clone() const
{
	return new Codec_polar::parameters(*this);
}

void Codec_polar::parameters
::enable_puncturer()
{
	set_pct(new Puncturer_polar::parameters(""));
}

std::vector<std::string> Codec_polar::parameters
::get_names() const
{
	auto n = Codec::parameters::get_names();
	std::vector<std::string> n2;

	for (size_t i = 0; i < n.size(); i++)
	{
		n2.push_back(n[i]);
		if (enc != nullptr && n[i] == enc->get_name() && fbg != nullptr)
		{
			auto nn = fbg->get_names();
			for (auto &x : nn)
				n2.push_back(x);
		}
	}

//	if (fbg != nullptr) { auto nn = fbg->get_names(); for (auto &x : nn) n.push_back(x); }
	return n2;
}

std::vector<std::string> Codec_polar::parameters
::get_short_names() const
{
	auto sn = Codec::parameters::get_short_names();
	std::vector<std::string> sn2;

	for (size_t i = 0; i < sn.size(); i++)
	{
		sn2.push_back(sn[i]);
		if (enc != nullptr && sn[i] == enc->get_short_name() && fbg != nullptr)
		{
			auto nn = fbg->get_short_names();
			for (auto &x : nn)
				sn2.push_back(x);
		}
	}

//	if (fbg != nullptr) { auto nn = fbg->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	return sn2;
}

std::vector<std::string> Codec_polar::parameters
::get_prefixes() const
{
	auto p = Codec::parameters::get_prefixes();
	std::vector<std::string> p2;

	for (size_t i = 0; i < p.size(); i++)
	{
		p2.push_back(p[i]);
		if (enc != nullptr && p[i] == enc->get_prefix() && fbg != nullptr)
		{
			auto nn = fbg->get_prefixes();
			for (auto &x : nn)
				p2.push_back(x);
		}
	}

//	if (fbg != nullptr) { auto nn = fbg->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	return p2;
}

void Codec_polar::parameters
::register_arguments(CLI::App &app)
{
	Codec_SISO_SIHO::parameters::register_arguments(app);

	auto sub_fbg = sub_enc;

	enc->register_arguments(*sub_enc);
	fbg->register_arguments(*sub_fbg);
	dec->register_arguments(*sub_dec);

	CLI::remove_option(sub_dec, "--cw-size"  , dec->get_prefix(), dec->no_argflag());
	CLI::remove_option(sub_dec, "--info-bits", dec->get_prefix(), dec->no_argflag());
	CLI::remove_option(sub_dec, "--fra"      , dec->get_prefix(), dec->no_argflag());
	CLI::remove_option(sub_dec, "--no-sys"   , dec->get_prefix(), dec->no_argflag());
	CLI::remove_option(sub_fbg, "--cw-size"  , fbg->get_prefix(), fbg->no_argflag());
	CLI::remove_option(sub_fbg, "--info-bits", fbg->get_prefix(), fbg->no_argflag());

	if (pct != nullptr)
	{
		pct->register_arguments(*sub_pct);

		CLI::remove_option(sub_enc, "--cw-size"  , enc->get_prefix(), enc->no_argflag());
		CLI::remove_option(sub_enc, "--info-bits", enc->get_prefix(), enc->no_argflag());
		CLI::remove_option(sub_enc, "--fra"      , enc->get_prefix(), enc->no_argflag());
	}
}

void Codec_polar::parameters
::callback_arguments()
{
	Codec_SISO_SIHO::parameters::callback_arguments();

	if (pct != nullptr)
	{
		pct->callback_arguments();

		enc->K        = pct->K;
		enc->N_cw     = pct->N_cw;
		enc->n_frames = pct->n_frames;
	}

	dec->K        = fbg->K    = enc->K;
	dec->N_cw     = fbg->N_cw = enc->N_cw;
	dec->n_frames             = enc->n_frames;

	enc->callback_arguments();
	fbg->callback_arguments();

	dec->not_systematic = enc->not_systematic;

	dec->callback_arguments();

	K    = pct != nullptr ? pct->K    : enc->K;
	N_cw = pct != nullptr ? pct->N_cw : enc->N_cw;
	N    = pct != nullptr ? pct->N    : enc->N_cw;
}

void Codec_polar::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	Codec_SISO_SIHO::parameters::get_headers(headers, full);

	enc->get_headers(headers, full);
	fbg->get_headers(headers, full);
	dec->get_headers(headers, full);
	if (pct != nullptr)
		pct->get_headers(headers, full);
}

template <typename B, typename Q>
module::Codec_polar<B,Q>* Codec_polar::parameters
::build(module::CRC<B> *crc) const
{
	return new module::Codec_polar<B,Q>(*fbg,
	                                    dynamic_cast<const Encoder_polar  ::parameters&>(*enc),
	                                    dynamic_cast<const Decoder_polar  ::parameters&>(*dec),
	                                    dynamic_cast<const Puncturer_polar::parameters*>(pct.get()),
	                                    crc);
}

template <typename B, typename Q>
module::Codec_polar<B,Q>* Codec_polar
::build(const parameters &params, module::CRC<B> *crc)
{
	return params.template build<B,Q>(crc);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Codec_polar<B_8 ,Q_8 >* aff3ct::factory::Codec_polar::parameters::build<B_8 ,Q_8 >(aff3ct::module::CRC<B_8 >*) const;
template aff3ct::module::Codec_polar<B_16,Q_16>* aff3ct::factory::Codec_polar::parameters::build<B_16,Q_16>(aff3ct::module::CRC<B_16>*) const;
template aff3ct::module::Codec_polar<B_32,Q_32>* aff3ct::factory::Codec_polar::parameters::build<B_32,Q_32>(aff3ct::module::CRC<B_32>*) const;
template aff3ct::module::Codec_polar<B_64,Q_64>* aff3ct::factory::Codec_polar::parameters::build<B_64,Q_64>(aff3ct::module::CRC<B_64>*) const;
template aff3ct::module::Codec_polar<B_8 ,Q_8 >* aff3ct::factory::Codec_polar::build<B_8 ,Q_8 >(const aff3ct::factory::Codec_polar::parameters&, aff3ct::module::CRC<B_8 >*);
template aff3ct::module::Codec_polar<B_16,Q_16>* aff3ct::factory::Codec_polar::build<B_16,Q_16>(const aff3ct::factory::Codec_polar::parameters&, aff3ct::module::CRC<B_16>*);
template aff3ct::module::Codec_polar<B_32,Q_32>* aff3ct::factory::Codec_polar::build<B_32,Q_32>(const aff3ct::factory::Codec_polar::parameters&, aff3ct::module::CRC<B_32>*);
template aff3ct::module::Codec_polar<B_64,Q_64>* aff3ct::factory::Codec_polar::build<B_64,Q_64>(const aff3ct::factory::Codec_polar::parameters&, aff3ct::module::CRC<B_64>*);
#else
template aff3ct::module::Codec_polar<B,Q>* aff3ct::factory::Codec_polar::parameters::build<B,Q>(aff3ct::module::CRC<B>*) const;
template aff3ct::module::Codec_polar<B,Q>* aff3ct::factory::Codec_polar::build<B,Q>(const aff3ct::factory::Codec_polar::parameters&, aff3ct::module::CRC<B>*);
#endif
// ==================================================================================== explicit template instantiation
