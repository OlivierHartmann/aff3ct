#if !defined(PREC_8_BIT) && !defined(PREC_16_BIT)

#include "Simulation/EXIT/EXIT.hpp"

#include <thread>
#include <string>
#include <iostream>

#include "Factory/Module/Monitor/BFER/Monitor_BFER.hpp"
#include "Factory/Module/Interleaver/Interleaver.hpp"

#include "EXIT.hpp"

using namespace aff3ct;
using namespace aff3ct::launcher;

template <typename B, typename R>
EXIT<B,R>
::EXIT(const int argc, const char **argv, std::ostream &stream)
: Launcher(argc, argv, stream), params("")
{
	params.set_src(new factory::Source      ::parameters("src"));
	params.set_mdm(new factory::Modem       ::parameters("mdm"));
	params.set_chn(new factory::Channel     ::parameters("chn"));
	params.set_qnt(new factory::Quantizer   ::parameters("qnt"));
	params.set_mnt(new factory::Monitor_EXIT::parameters("mnt"));
	params.set_ter(new factory::Terminal    ::parameters("ter"));

	set_params(&params);
}

template <typename B, typename R>
void EXIT<B,R>
::register_arguments(CLI::App &app)
{
	Launcher::register_arguments(app);

	auto sub_sim = CLI::make_subcommand(app, "sim", params.     get_name() + " parameters");
	auto sub_src = CLI::make_subcommand(app, "src", params.src->get_name() + " parameters");
	auto sub_mdm = CLI::make_subcommand(app, "mdm", params.mdm->get_name() + " parameters");
	auto sub_chn = CLI::make_subcommand(app, "chn", params.chn->get_name() + " parameters");
	auto sub_mnt = CLI::make_subcommand(app, "mnt", params.mnt->get_name() + " parameters"); // same than mnt_mi
	auto sub_ter = CLI::make_subcommand(app, "ter", params.ter->get_name() + " parameters");

	params.     register_arguments(*sub_sim);
	params.glb->register_arguments(     app);
	params.src->register_arguments(*sub_src);
	params.mdm->register_arguments(*sub_mdm);
	params.chn->register_arguments(*sub_chn);
	params.mnt->register_arguments(*sub_mnt);
	params.ter->register_arguments(*sub_ter);


	auto sub_enc = app.get_subcommand("enc");

	if (CLI::has_option(sub_enc, "--info-bits", params.cdc->enc->get_prefix(), params.cdc->enc->no_argflag()))
		CLI::remove_option(sub_src, "--info-bits", params.src->get_prefix(), params.src->no_argflag());
	CLI::remove_option(sub_src, "--seed"     , params.src->get_prefix(), params.src->no_argflag());

	CLI::remove_option(sub_mdm, "--fra-size" , params.mdm->get_prefix(), params.mdm->no_argflag());
	CLI::remove_option(sub_mdm, "--fra"      , params.mdm->get_prefix(), params.mdm->no_argflag());
	CLI::remove_option(sub_mdm, "--noise"    , params.mdm->get_prefix(), params.mdm->no_argflag());

	CLI::remove_option(sub_chn, "--fra-size" , params.chn->get_prefix(), params.chn->no_argflag());
	CLI::remove_option(sub_chn, "--fra"      , params.chn->get_prefix(), params.chn->no_argflag());
	CLI::remove_option(sub_chn, "--noise"    , params.chn->get_prefix(), params.chn->no_argflag());
	CLI::remove_option(sub_chn, "--seed"     , params.chn->get_prefix(), params.chn->no_argflag());
	CLI::remove_option(sub_chn, "--add-users", params.chn->get_prefix(), params.chn->no_argflag());
	CLI::remove_option(sub_chn, "--complex"  , params.chn->get_prefix(), params.chn->no_argflag());

	CLI::remove_option(sub_mnt, "--info-bits", params.mnt->get_prefix(), params.mnt->no_argflag());
	CLI::remove_option(sub_mnt, "--fra"      , params.mnt->get_prefix(), params.mnt->no_argflag());
	CLI::remove_option(sub_mnt, "--trials"   , params.mnt->get_prefix(), params.mnt->no_argflag());
}

template <typename B, typename R>
void EXIT<B,R>
::callback_arguments()
{
	Launcher::callback_arguments();

	params.callback_arguments();
	params.glb->callback_arguments();

	params.src->seed = params.local_seed;

	auto K = params.src->K != 0 ? params.src->K : params.cdc->K;
	auto N = params.src->K != 0 ? params.src->K : params.cdc->N;

	params.src->callback_arguments();
	params.src->K = params.src->K != 0 ? params.src->K : K;


	params.mdm->N = N;
	params.mdm->callback_arguments();

	params.chn->N         = params.mdm->N_mod;
	params.chn->complex   = params.mdm->complex;
	params.chn->add_users = params.mdm->type == "SCMA";
	params.chn->seed      = params.local_seed;

	params.chn->callback_arguments();

	params.mnt->size = K;
	params.mnt->callback_arguments();

	params.ter->callback_arguments();

	if (params.src->type == "AZCW" || params.cdc->enc->type == "AZCW")
	{
		params.src     ->type = "AZCW";
		params.cdc->enc->type = "AZCW";
	}

	params.cdc->enc->seed = params.local_seed;

	params.mdm->n_frames = params.src->n_frames;
	params.chn->n_frames = params.src->n_frames;
}

template <typename B, typename R>
simulation::Simulation* EXIT<B,R>
::build_simu()
{
#if !defined(SYSTEMC)
	return factory::EXIT::build<B,R>(params);
#else
	throw tools::invalid_argument(__FILE__, __LINE__, __func__, "SystemC/TLM  simulation is not available.");
#endif
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template class aff3ct::launcher::EXIT<B_32,R_32>;
template class aff3ct::launcher::EXIT<B_64,R_64>;
#else
template class aff3ct::launcher::EXIT<B,R>;
#endif
// ==================================================================================== explicit template instantiation

#endif