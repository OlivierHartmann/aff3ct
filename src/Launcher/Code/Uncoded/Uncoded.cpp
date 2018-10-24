#include <iostream>

#include "Factory/Module/Codec/Uncoded/Codec_uncoded.hpp"

#include "Uncoded.hpp"

using namespace aff3ct;
using namespace aff3ct::launcher;

template <class L, typename B, typename R, typename Q>
Uncoded<L,B,R,Q>
::Uncoded(const int argc, const char **argv, std::ostream &stream)
: L(argc, argv, stream), params_cdc(new factory::Codec_uncoded::parameters(""))
{
	this->params.set_cdc(params_cdc);
}

template <class L, typename B, typename R, typename Q>
void Uncoded<L,B,R,Q>
::register_arguments(CLI::App &app)
{
	params_cdc->register_arguments(app);

	// auto sub_dec = app.get_subcommand("dec");
	auto sub_enc = app.get_subcommand("enc");

	CLI::remove_option(sub_enc, "--info-bits", params_cdc->enc->get_prefix(), params_cdc->enc->no_argflag());
	CLI::remove_option(sub_enc, "--fra",       params_cdc->enc->get_prefix(), params_cdc->enc->no_argflag());

	L::register_arguments(app);
}

template <class L, typename B, typename R, typename Q>
void Uncoded<L,B,R,Q>
::callback_arguments()
{
	L::callback_arguments();

	params_cdc->enc->K = this->params.src->K;

	params_cdc->callback_arguments();

	params_cdc->enc->n_frames = this->params.src->n_frames;
	params_cdc->dec->n_frames = this->params.src->n_frames;
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#include "Launcher/Simulation/EXIT.hpp"
#include "Launcher/Simulation/BFER_std.hpp"
#include "Launcher/Simulation/BFER_ite.hpp"
#ifdef MULTI_PREC
template class aff3ct::launcher::Uncoded<aff3ct::launcher::EXIT    <B_32,R_32     >,B_32,R_32     >;
template class aff3ct::launcher::Uncoded<aff3ct::launcher::EXIT    <B_64,R_64     >,B_64,R_64     >;
template class aff3ct::launcher::Uncoded<aff3ct::launcher::BFER_std<B_8, R_8, Q_8 >,B_8 ,R_8 ,Q_8 >;
template class aff3ct::launcher::Uncoded<aff3ct::launcher::BFER_std<B_16,R_16,Q_16>,B_16,R_16,Q_16>;
template class aff3ct::launcher::Uncoded<aff3ct::launcher::BFER_std<B_32,R_32,Q_32>,B_32,R_32,Q_32>;
template class aff3ct::launcher::Uncoded<aff3ct::launcher::BFER_std<B_64,R_64,Q_64>,B_64,R_64,Q_64>;
template class aff3ct::launcher::Uncoded<aff3ct::launcher::BFER_ite<B_8, R_8, Q_8 >,B_8 ,R_8 ,Q_8 >;
template class aff3ct::launcher::Uncoded<aff3ct::launcher::BFER_ite<B_16,R_16,Q_16>,B_16,R_16,Q_16>;
template class aff3ct::launcher::Uncoded<aff3ct::launcher::BFER_ite<B_32,R_32,Q_32>,B_32,R_32,Q_32>;
template class aff3ct::launcher::Uncoded<aff3ct::launcher::BFER_ite<B_64,R_64,Q_64>,B_64,R_64,Q_64>;
#else
#if defined(PREC_32_BIT) || defined(PREC_64_BIT)
template class aff3ct::launcher::Uncoded<aff3ct::launcher::EXIT    <B,R  >,B,R  >;
#endif
template class aff3ct::launcher::Uncoded<aff3ct::launcher::BFER_std<B,R,Q>,B,R,Q>;
template class aff3ct::launcher::Uncoded<aff3ct::launcher::BFER_ite<B,R,Q>,B,R,Q>;
#endif
// ==================================================================================== explicit template instantiation
