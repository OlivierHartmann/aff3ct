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
: Launcher(argc, argv, stream)
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

	params.     register_arguments(app);
	params.src->register_arguments(app);
	params.mdm->register_arguments(app);
	params.chn->register_arguments(app);
	params.mnt->register_arguments(app);
	params.ter->register_arguments(app);

	auto psrc = params.src     ->get_prefix();
	auto penc = params.cdc->enc->get_prefix();
	auto pmdm = params.mdm     ->get_prefix();
	auto pchn = params.chn     ->get_prefix();
	auto pmnt = params.mnt     ->get_prefix();
	auto pter = params.ter     ->get_prefix();

	if (this->args.exist({penc+"-info-bits", "K"}))
		this->args.erase({psrc+"-info-bits", "K"});
	this->args.erase({psrc+"-seed",     "S"});
	this->args.erase({pmdm+"-fra-size", "N"});
	this->args.erase({pmdm+"-fra",      "F"});
	this->args.erase({pmdm+"-noise"       });
	this->args.erase({pchn+"-fra-size", "N"});
	this->args.erase({pchn+"-fra",      "F"});
	this->args.erase({pchn+"-noise"        });
	this->args.erase({pchn+"-seed",     "S"});
	this->args.erase({pchn+"-add-users"    });
	this->args.erase({pchn+"-complex"      });
	this->args.erase({pmnt+"-size",     "K"});
	this->args.erase({pmnt+"-fra",      "F"});
	this->args.erase({pter+"-cw-size",  "N"});
}

template <typename B, typename R>
void EXIT<B,R>
::callback_arguments()
{
	Launcher::callback_arguments();

	params.callback_arguments();

	params.src->seed = params.local_seed;

	params.src->callback_arguments();

	auto psrc = params.src->get_prefix();

	auto K = this->args.exist({psrc+"-info-bits", "K"}) ? params.src->K : params.cdc->K;
	auto N = this->args.exist({psrc+"-info-bits", "K"}) ? params.src->K : params.cdc->N;

	params.src->K = params.src->K == 0 ? K : params.src->K;
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

	auto pmnt = params.mnt->get_prefix();

	if (!this->arg_vals.exist({pmnt+"-trials", "n"}) && params.cdc->K != 0)
		params.mnt->n_trials = 200000 / params.cdc->K;
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