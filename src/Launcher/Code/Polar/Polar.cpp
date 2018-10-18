#include <iostream>
#include <typeinfo>

#include "Launcher/Simulation/BFER_std.hpp"

#include "Factory/Module/Codec/Polar/Codec_polar.hpp"

#include "Polar.hpp"

using namespace aff3ct;
using namespace aff3ct::launcher;

template <class L, typename B, typename R, typename Q>
Polar<L,B,R,Q>
::Polar(const int argc, const char **argv, std::ostream &stream)
: L(argc, argv, stream), params_cdc(new factory::Codec_polar::parameters("cdc"))
{
	this->params.set_cdc(params_cdc);

	if (std::is_same<L, BFER_std<B,R,Q>>::value)
		params_cdc->enable_puncturer();

	if (std::is_same<Q,int8_t>() || std::is_same<Q,int16_t>())
	{
		this->params.qnt->n_bits     = 6;
		this->params.qnt->n_decimals = 1;
	}
}

template <class L, typename B, typename R, typename Q>
void Polar<L,B,R,Q>
::register_arguments(CLI::App &app)
{
	params_cdc->register_arguments(app);

	// auto sub_dec = app.get_subcommand("dec");
	auto sub_enc = app.get_subcommand("enc");

	CLI::remove_option(sub_enc, "--seed", params_cdc->enc->get_prefix());

	if (params_cdc->pct != nullptr)
	{
		auto sub_pct = app.get_subcommand("pct");
		CLI::remove_option(sub_pct, "--fra", params_cdc->pct->get_prefix());
	}
	else
		CLI::remove_option(sub_enc, "--fra", params_cdc->enc->get_prefix());

	L::register_arguments(app);
}

template <class L, typename B, typename R, typename Q>
void Polar<L,B,R,Q>
::callback_arguments()
{
	auto dec_polar = dynamic_cast<factory::Decoder_polar::parameters*>(params_cdc->dec.get());

	params_cdc->callback_arguments();

	if (dec_polar->simd_strategy == "INTER")
		this->params.src->n_frames = mipp::N<Q>();

	L::callback_arguments();

	params_cdc->enc->n_frames = this->params.src->n_frames;
	if (params_cdc->pct != nullptr)
	params_cdc->pct->n_frames = this->params.src->n_frames;
	params_cdc->dec->n_frames = this->params.src->n_frames;
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#include "Launcher/Simulation/EXIT.hpp"
#include "Launcher/Simulation/BFER_std.hpp"
#include "Launcher/Simulation/BFER_ite.hpp"
#ifdef MULTI_PREC
template class aff3ct::launcher::Polar<aff3ct::launcher::EXIT    <B_32,R_32     >,B_32,R_32     >;
template class aff3ct::launcher::Polar<aff3ct::launcher::EXIT    <B_64,R_64     >,B_64,R_64     >;
template class aff3ct::launcher::Polar<aff3ct::launcher::BFER_std<B_8, R_8, Q_8 >,B_8 ,R_8 ,Q_8 >;
template class aff3ct::launcher::Polar<aff3ct::launcher::BFER_std<B_16,R_16,Q_16>,B_16,R_16,Q_16>;
template class aff3ct::launcher::Polar<aff3ct::launcher::BFER_std<B_32,R_32,Q_32>,B_32,R_32,Q_32>;
template class aff3ct::launcher::Polar<aff3ct::launcher::BFER_std<B_64,R_64,Q_64>,B_64,R_64,Q_64>;
template class aff3ct::launcher::Polar<aff3ct::launcher::BFER_ite<B_8, R_8, Q_8 >,B_8 ,R_8 ,Q_8 >;
template class aff3ct::launcher::Polar<aff3ct::launcher::BFER_ite<B_16,R_16,Q_16>,B_16,R_16,Q_16>;
template class aff3ct::launcher::Polar<aff3ct::launcher::BFER_ite<B_32,R_32,Q_32>,B_32,R_32,Q_32>;
template class aff3ct::launcher::Polar<aff3ct::launcher::BFER_ite<B_64,R_64,Q_64>,B_64,R_64,Q_64>;
#else
#if defined(PREC_32_BIT) || defined(PREC_64_BIT)
template class aff3ct::launcher::Polar<aff3ct::launcher::EXIT    <B,R  >,B,R  >;
#endif
template class aff3ct::launcher::Polar<aff3ct::launcher::BFER_std<B,R,Q>,B,R,Q>;
template class aff3ct::launcher::Polar<aff3ct::launcher::BFER_ite<B,R,Q>,B,R,Q>;
#endif
// ==================================================================================== explicit template instantiation
