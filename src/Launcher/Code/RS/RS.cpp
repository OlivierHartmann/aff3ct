#include <iostream>

#include "Factory/Module/Codec/RS/Codec_RS.hpp"

#include "RS.hpp"

using namespace aff3ct;
using namespace aff3ct::launcher;

template <class L, typename B, typename R, typename Q>
RS<L,B,R,Q>
::RS(const int argc, const char **argv, std::ostream &stream)
: L(argc, argv, stream), params_cdc(new factory::Codec_RS::parameters(""))
{
	this->params.set_cdc(params_cdc);
}

template <class L, typename B, typename R, typename Q>
void RS<L,B,R,Q>
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
void RS<L,B,R,Q>
::callback_arguments()
{
	params_cdc->callback_arguments();

	L::callback_arguments();

	params_cdc->enc->n_frames = this->params.src->n_frames;
	params_cdc->dec->n_frames = this->params.src->n_frames;
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#include "Launcher/Simulation/BFER_std.hpp"
#ifdef MULTI_PREC
template class aff3ct::launcher::RS<aff3ct::launcher::BFER_std<B_8 ,R_8 ,Q_8 >,B_8 ,R_8 ,Q_8 >;
template class aff3ct::launcher::RS<aff3ct::launcher::BFER_std<B_16,R_16,Q_16>,B_16,R_16,Q_16>;
template class aff3ct::launcher::RS<aff3ct::launcher::BFER_std<B_32,R_32,Q_32>,B_32,R_32,Q_32>;
template class aff3ct::launcher::RS<aff3ct::launcher::BFER_std<B_64,R_64,Q_64>,B_64,R_64,Q_64>;
#else
template class aff3ct::launcher::RS<aff3ct::launcher::BFER_std<B,R,Q>,B,R,Q>;
#endif
// ==================================================================================== explicit template instantiation
