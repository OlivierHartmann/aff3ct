#include <iostream>
#include <mipp.h>

#include "Launcher/Simulation/BFER_std.hpp"

#include "Factory/Module/Codec/RSC/Codec_RSC.hpp"

#include "RSC.hpp"

using namespace aff3ct;
using namespace aff3ct::launcher;

template <class L, typename B, typename R, typename Q>
RSC<L,B,R,Q>
::RSC(const int argc, const char **argv, std::ostream &stream)
: L(argc, argv, stream), params_cdc(new factory::Codec_RSC::parameters("cdc"))
{
	this->params.set_cdc(params_cdc);

	if (std::is_same<Q,int8_t>())
	{
		this->params.qnt->n_bits     = 6;
		this->params.qnt->n_decimals = 1;
	}
	else if (std::is_same<Q,int16_t>())
	{
		this->params.qnt->n_bits     = 6;
		this->params.qnt->n_decimals = 3;
	}
}

template <class L, typename B, typename R, typename Q>
void RSC<L,B,R,Q>
::register_arguments(CLI::App &app)
{
	params_cdc->register_arguments(app);

	// auto sub_dec = app.get_subcommand("dec");
	auto sub_enc = app.get_subcommand("enc");

	CLI::remove_option(sub_enc, "--seed", params_cdc->enc->get_prefix(), params_cdc->enc->no_argflag());
	CLI::remove_option(sub_enc, "--fra",  params_cdc->enc->get_prefix(), params_cdc->enc->no_argflag());

	L::register_arguments(app);
}

template <class L, typename B, typename R, typename Q>
void RSC<L,B,R,Q>
::callback_arguments()
{
	auto dec_rsc = dynamic_cast<factory::Decoder_RSC::parameters*>(params_cdc->dec.get());

	params_cdc->callback_arguments();

	if (dec_rsc->simd_strategy == "INTER")
		this->params.src->n_frames = mipp::N<Q>();
	if (dec_rsc->simd_strategy == "INTRA")
		this->params.src->n_frames = (int)std::ceil(mipp::N<Q>() / 8.f);

	L::callback_arguments();

	params_cdc->enc->n_frames = this->params.src->n_frames;
	params_cdc->dec->n_frames = this->params.src->n_frames;
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#include "Launcher/Simulation/EXIT.hpp"
#include "Launcher/Simulation/BFER_std.hpp"
#include "Launcher/Simulation/BFER_ite.hpp"
#ifdef MULTI_PREC
template class aff3ct::launcher::RSC<aff3ct::launcher::EXIT    <B_32,R_32     >,B_32,R_32     >;
template class aff3ct::launcher::RSC<aff3ct::launcher::EXIT    <B_64,R_64     >,B_64,R_64     >;
template class aff3ct::launcher::RSC<aff3ct::launcher::BFER_std<B_8, R_8, Q_8 >,B_8 ,R_8 ,Q_8 >;
template class aff3ct::launcher::RSC<aff3ct::launcher::BFER_std<B_16,R_16,Q_16>,B_16,R_16,Q_16>;
template class aff3ct::launcher::RSC<aff3ct::launcher::BFER_std<B_32,R_32,Q_32>,B_32,R_32,Q_32>;
template class aff3ct::launcher::RSC<aff3ct::launcher::BFER_std<B_64,R_64,Q_64>,B_64,R_64,Q_64>;
template class aff3ct::launcher::RSC<aff3ct::launcher::BFER_ite<B_8, R_8, Q_8 >,B_8 ,R_8 ,Q_8 >;
template class aff3ct::launcher::RSC<aff3ct::launcher::BFER_ite<B_16,R_16,Q_16>,B_16,R_16,Q_16>;
template class aff3ct::launcher::RSC<aff3ct::launcher::BFER_ite<B_32,R_32,Q_32>,B_32,R_32,Q_32>;
template class aff3ct::launcher::RSC<aff3ct::launcher::BFER_ite<B_64,R_64,Q_64>,B_64,R_64,Q_64>;
#else
#if defined(PREC_32_BIT) || defined(PREC_64_BIT)
template class aff3ct::launcher::RSC<aff3ct::launcher::EXIT    <B,R  >,B,R  >;
#endif
template class aff3ct::launcher::RSC<aff3ct::launcher::BFER_std<B,R,Q>,B,R,Q>;
template class aff3ct::launcher::RSC<aff3ct::launcher::BFER_ite<B,R,Q>,B,R,Q>;
#endif
// ==================================================================================== explicit template instantiation
