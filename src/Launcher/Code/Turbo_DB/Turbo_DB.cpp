#include <iostream>

#include "Launcher/Simulation/BFER_std.hpp"

#include "Factory/Module/Codec/Turbo_DB/Codec_turbo_DB.hpp"

#include "Turbo_DB.hpp"

using namespace aff3ct;
using namespace aff3ct::launcher;

template <class L, typename B, typename R, typename Q>
Turbo_DB<L,B,R,Q>
::Turbo_DB(const int argc, const char **argv, std::ostream &stream)
: L(argc, argv, stream), params_cdc(new factory::Codec_turbo_DB::parameters(""))
{
	this->params.set_cdc(params_cdc);

	if (std::is_same<L, BFER_std<B,R,Q>>::value)
		params_cdc->enable_puncturer();

	if (std::is_same<Q,int8_t>())
	{
		this->params.qnt->n_bits     = 6;
		this->params.qnt->n_decimals = 2;
	}
	else if (std::is_same<Q,int16_t>())
	{
		this->params.qnt->n_bits     = 6;
		this->params.qnt->n_decimals = 3;
	}
}

template <class L, typename B, typename R, typename Q>
void Turbo_DB<L,B,R,Q>
::register_arguments(CLI::App &app)
{
	params_cdc->register_arguments(app);

	// auto sub_dec = app.get_subcommand("dec");
	auto sub_enc = app.get_subcommand("enc");

	CLI::remove_option(sub_enc, "--seed", params_cdc->enc->get_prefix(), params_cdc->enc->no_argflag());
	CLI::remove_option(sub_enc, "--fra",  params_cdc->enc->get_prefix(), params_cdc->enc->no_argflag());

	if (params_cdc->itl != nullptr)
	{
		auto sub_itl = app.get_subcommand("itl");
		CLI::remove_option(sub_itl, "--seed", params_cdc->itl->get_prefix(), params_cdc->itl->no_argflag());
	}

	L::register_arguments(app);
}

template <class L, typename B, typename R, typename Q>
void Turbo_DB<L,B,R,Q>
::callback_arguments()
{
	auto enc_tur = dynamic_cast<factory::Encoder_turbo_DB::parameters*>(params_cdc->enc.get());
	auto dec_tur = dynamic_cast<factory::Decoder_turbo_DB::parameters*>(params_cdc->dec.get());

	params_cdc->callback_arguments();

	L::callback_arguments();

	params_cdc->enc      ->n_frames = this->params.src->n_frames;
	if (params_cdc->pct != nullptr)
	params_cdc->pct      ->n_frames = this->params.src->n_frames;
	params_cdc->dec      ->n_frames = this->params.src->n_frames;
	if (params_cdc->itl != nullptr)
	params_cdc->itl->core->n_frames = this->params.src->n_frames;
	enc_tur->sub         ->n_frames = this->params.src->n_frames;
	dec_tur->sub         ->n_frames = this->params.src->n_frames;

	params_cdc->itl->core->seed = this->params.global_seed;
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#include "Launcher/Simulation/BFER_std.hpp"
#ifdef MULTI_PREC
template class aff3ct::launcher::Turbo_DB<aff3ct::launcher::BFER_std<B_8 ,R_8 ,Q_8 >,B_8 ,R_8 ,Q_8 >;
template class aff3ct::launcher::Turbo_DB<aff3ct::launcher::BFER_std<B_16,R_16,Q_16>,B_16,R_16,Q_16>;
template class aff3ct::launcher::Turbo_DB<aff3ct::launcher::BFER_std<B_32,R_32,Q_32>,B_32,R_32,Q_32>;
template class aff3ct::launcher::Turbo_DB<aff3ct::launcher::BFER_std<B_64,R_64,Q_64>,B_64,R_64,Q_64>;
#else
template class aff3ct::launcher::Turbo_DB<aff3ct::launcher::BFER_std<B,R,Q>,B,R,Q>;
#endif
// ==================================================================================== explicit template instantiation
