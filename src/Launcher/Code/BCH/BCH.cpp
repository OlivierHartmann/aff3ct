#include <iostream>

#include "Factory/Module/Codec/BCH/Codec_BCH.hpp"

#include "BCH.hpp"

using namespace aff3ct;
using namespace aff3ct::launcher;

template <class L, typename B, typename R, typename Q>
BCH<L,B,R,Q>
::BCH(const int argc, const char **argv, std::ostream &stream)
: L(argc, argv, stream), params_cdc(new factory::Codec_BCH::parameters("cdc"))
{
	this->params.set_cdc(params_cdc);
}

template <class L, typename B, typename R, typename Q>
void BCH<L,B,R,Q>
::register_arguments(CLI::App &app)
{
	params_cdc->register_arguments(app);

	// auto sub_dec = app.get_subcommand(params_cdc->dec->get_prefix());
	auto sub_enc = app.get_subcommand(params_cdc->enc->get_prefix());

	CLI::remove_option(sub_enc, "--fra" );
	CLI::remove_option(sub_enc, "--seed");

	L::register_arguments(app);
}

template <class L, typename B, typename R, typename Q>
void BCH<L,B,R,Q>
::callback_arguments()
{
	params_cdc->callback_arguments();

	if (params_cdc->dec->implem == "FAST")
		this->params.src->n_frames = mipp::N<B>();

	L::callback_arguments();

	params_cdc->enc->n_frames = this->params.src->n_frames;
	params_cdc->dec->n_frames = this->params.src->n_frames;
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#include "Launcher/Simulation/BFER_std.hpp"
#ifdef MULTI_PREC
template class aff3ct::launcher::BCH<aff3ct::launcher::BFER_std<B_8 ,R_8 ,Q_8 >,B_8 ,R_8 ,Q_8 >;
template class aff3ct::launcher::BCH<aff3ct::launcher::BFER_std<B_16,R_16,Q_16>,B_16,R_16,Q_16>;
template class aff3ct::launcher::BCH<aff3ct::launcher::BFER_std<B_32,R_32,Q_32>,B_32,R_32,Q_32>;
template class aff3ct::launcher::BCH<aff3ct::launcher::BFER_std<B_64,R_64,Q_64>,B_64,R_64,Q_64>;
#else
template class aff3ct::launcher::BCH<aff3ct::launcher::BFER_std<B,R,Q>,B,R,Q>;
#endif
// ==================================================================================== explicit template instantiation
