#include "Simulation/BFER/Standard/BFER_std.hpp"

#include <thread>
#include <string>
#include <iostream>

#include "Factory/Module/Monitor/BFER/Monitor_BFER.hpp"
#include "Factory/Module/Monitor/MI/Monitor_MI.hpp"
#include "Factory/Module/Interleaver/Interleaver.hpp"

#include "BFER_std.hpp"

using namespace aff3ct;
using namespace aff3ct::launcher;

template <typename B, typename R, typename Q>
BFER_std<B,R,Q>
::BFER_std(const int argc, const char **argv, std::ostream &stream)
: Launcher(argc, argv, stream), params("")
{
	params.set_src   (new factory::Source      ::parameters(""));
	params.set_crc   (new factory::CRC         ::parameters(""));
	params.set_mdm   (new factory::Modem       ::parameters(""));
	params.set_chn   (new factory::Channel     ::parameters(""));
	params.set_qnt   (new factory::Quantizer   ::parameters(""));
	params.set_mnt_mi(new factory::Monitor_MI  ::parameters(""));
	params.set_mnt_er(new factory::Monitor_BFER::parameters(""));
	params.set_ter   (new factory::Terminal    ::parameters(""));

	set_params(&params);
}

template <typename B, typename R, typename Q>
void BFER_std<B,R,Q>
::register_arguments(CLI::App &app)
{
	Launcher::register_arguments(app);

	auto sub_sim = CLI::make_subcommand(app, "sim", params.        get_name() + " parameters");
	auto sub_src = CLI::make_subcommand(app, "src", params.src   ->get_name() + " parameters");
	auto sub_crc = CLI::make_subcommand(app, "crc", params.crc   ->get_name() + " parameters");
	auto sub_mdm = CLI::make_subcommand(app, "mdm", params.mdm   ->get_name() + " parameters");
	auto sub_chn = CLI::make_subcommand(app, "chn", params.chn   ->get_name() + " parameters");
	auto sub_mnt = CLI::make_subcommand(app, "mnt", params.mnt_er->get_name() + " parameters"); // same than mnt_mi
	auto sub_ter = CLI::make_subcommand(app, "ter", params.ter   ->get_name() + " parameters");

	params.        register_arguments(*sub_sim);
	params.glb   ->register_arguments(     app);
	params.src   ->register_arguments(*sub_src);
	params.crc   ->register_arguments(*sub_crc);
	params.mdm   ->register_arguments(*sub_mdm);
	params.chn   ->register_arguments(*sub_chn);
	params.mnt_er->register_arguments(*sub_mnt);
	params.mnt_mi->register_arguments(*sub_mnt);
	params.ter   ->register_arguments(*sub_ter);

	CLI::App* sub_qnt = nullptr;
	if (std::is_integral<Q>())
	{
		sub_qnt = CLI::make_subcommand(app, "qnt", params.qnt->get_name() + " parameters");
		params.qnt->register_arguments(*sub_qnt);
	}

	CLI::App* sub_pct = nullptr;
	if (params.cdc->pct != nullptr && CLI::has_subcommand(app, "pct"))
		sub_pct = app.get_subcommand("pct");

	auto sub_enc = app.get_subcommand("enc");

	if (CLI::has_option(sub_enc, "--info-bits", params.cdc->enc->get_prefix(), params.cdc->enc->no_argflag()) ||
		(params.cdc->pct != nullptr && CLI::has_option(sub_pct, "--info-bits", params.cdc->pct->get_prefix(), params.cdc->pct->no_argflag())))
		CLI::remove_option(sub_src, "--info-bits", params.src->get_prefix(), params.src->no_argflag());
	CLI::remove_option(sub_src, "--seed"     , params.src->get_prefix(), params.src->no_argflag());

	CLI::remove_option(sub_crc, "--info-bits", params.crc->get_prefix(), params.crc->no_argflag());
	CLI::remove_option(sub_crc, "--fra"      , params.crc->get_prefix(), params.crc->no_argflag());

	CLI::remove_option(sub_mdm, "--fra-size" , params.mdm->get_prefix(), params.mdm->no_argflag());
	CLI::remove_option(sub_mdm, "--fra"      , params.mdm->get_prefix(), params.mdm->no_argflag());
	CLI::remove_option(sub_mdm, "--noise"    , params.mdm->get_prefix(), params.mdm->no_argflag());

	CLI::remove_option(sub_chn, "--fra-size" , params.chn->get_prefix(), params.chn->no_argflag());
	CLI::remove_option(sub_chn, "--fra"      , params.chn->get_prefix(), params.chn->no_argflag());
	CLI::remove_option(sub_chn, "--noise"    , params.chn->get_prefix(), params.chn->no_argflag());
	CLI::remove_option(sub_chn, "--seed"     , params.chn->get_prefix(), params.chn->no_argflag());
	CLI::remove_option(sub_chn, "--add-users", params.chn->get_prefix(), params.chn->no_argflag());
	CLI::remove_option(sub_chn, "--complex"  , params.chn->get_prefix(), params.chn->no_argflag());

	CLI::remove_option(sub_qnt, "--size"     , params.qnt->get_prefix(), params.qnt->no_argflag());
	CLI::remove_option(sub_qnt, "--fra"      , params.qnt->get_prefix(), params.qnt->no_argflag());

	CLI::remove_option(sub_mnt, "--info-bits", params.mnt_er->get_prefix(), params.mnt_er->no_argflag());
	CLI::remove_option(sub_mnt, "--fra-size" , params.mnt_er->get_prefix(), params.mnt_er->no_argflag());
	CLI::remove_option(sub_mnt, "--fra"      , params.mnt_er->get_prefix(), params.mnt_er->no_argflag());
	CLI::remove_option(sub_mnt, "--max-frame", params.mnt_er->get_prefix(), params.mnt_er->no_argflag());
	CLI::remove_option(sub_mnt, "--trials"   , params.mnt_er->get_prefix(), params.mnt_er->no_argflag());

#ifdef ENABLE_MPI
	CLI::remove_option(sub_ter, "--freq"     , params.ter->get_prefix(), params.ter->no_argflag());
#endif


	CLI::add_flag(*sub_mnt, params.mnt_er->get_prefix(), params.mnt_er->no_argflag(),
		"--mutinfo",
		params.mutinfo,
		"Allow the computation of the mutual information.")
		->group("Standard");
}

template <typename B, typename R, typename Q>
void BFER_std<B,R,Q>
::callback_arguments()
{
	Launcher::callback_arguments();

	params.callback_arguments();
	params.glb->callback_arguments();

	params.src->seed = params.local_seed;


	auto K    = params.src->K != 0 ? params.src->K : params.cdc->K;
	auto N    = params.src->K != 0 ? params.src->K : params.cdc->N;
	auto N_cw = params.src->K != 0 ? params.src->K : params.cdc->N_cw;

	params.crc->callback_arguments();
	params.crc->K = K - params.crc->size;

	params.src->callback_arguments();
	params.src->K = params.src->K != 0 ? params.src->K : params.crc->K;


	params.mdm->N = N;
	params.mdm->callback_arguments();

	params.chn->N         = params.mdm->N_mod;
	params.chn->complex   = params.mdm->complex;
	params.chn->add_users = params.mdm->type == "SCMA";
	params.chn->seed      = params.local_seed;
	params.chn->callback_arguments();

	if (params.chn->type == "OPTICAL")
		params.noise->type = "ROP";
	else if (params.chn->type == "BEC" || params.chn->type == "BSC")
		params.noise->type = "EP";
	// else let the default value EBN0 or ESNO

	params.qnt->size = params.mdm->N;

	if (std::is_integral<Q>())
		params.qnt->callback_arguments();

	params.mnt_er->K = params.coded_monitoring ? N_cw : params.src->K;
	params.mnt_mi->N = N;
	params.mnt_er->callback_arguments();
	params.mnt_mi->callback_arguments();


	params.ter->callback_arguments();

	if (!std::is_integral<Q>())
		params.qnt->type = "NO";

	if (params.coset)
		params.cdc->enc->type = "COSET";
	else if (params.cdc->enc->type == "COSET")
		params.coset = true;

	if (params.src->type == "AZCW" || params.cdc->enc->type == "AZCW")
	{
		params.src     ->type = "AZCW";
		params.cdc->enc->type = "AZCW";
	}

	if (params.err_track_revert)
	{
		params.src->type = "USER";
		params.src->path = params.err_track_path + std::string("_$noise.src");

		params.cdc->enc->type = "USER";
		params.cdc->enc->path = params.err_track_path + std::string("_$noise.enc");

		if (params.cdc->itl != nullptr && params.cdc->itl->core->uniform)
		{
			params.cdc->itl->core->type = "USER";
			params.cdc->itl->core->path = params.err_track_path + std::string("_$noise.itl");
		}

		if (params.chn->type == "USER_ADD" || params.chn->type == "AWGN" || params.chn->type == "RAYLEIGH" || params.chn->type == "RAYLEIGH_USER")
			params.chn->type = "USER_ADD";
		else if (params.chn->type == "USER" || params.chn->type == "BEC" || params.chn->type == "OPTICAL")
			params.chn->type = "USER";
		// else params.chn->type == "NO" stays as it is
		params.chn->path = params.err_track_path + std::string("_$snr.chn");
	}

	params.cdc->enc->seed = params.local_seed;

	params.crc   ->n_frames = params.src->n_frames;
	params.mdm   ->n_frames = params.src->n_frames;
	params.chn   ->n_frames = params.src->n_frames;
	params.qnt   ->n_frames = params.src->n_frames;
	params.mnt_er->n_frames = params.src->n_frames;
	params.mnt_mi->n_frames = params.src->n_frames;

	params.mnt_er->max_frame = params.max_frame;
	params.mnt_mi->n_trials  = 0;

#ifdef ENABLE_MPI
	params.ter->frequency = params.mpi_comm_freq;
#endif

}

template <typename B, typename R, typename Q>
simulation::Simulation* BFER_std<B,R,Q>
::build_simu()
{
	return factory::BFER_std::build<B,R,Q>(params);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template class aff3ct::launcher::BFER_std<B_8,R_8,Q_8>;
template class aff3ct::launcher::BFER_std<B_16,R_16,Q_16>;
template class aff3ct::launcher::BFER_std<B_32,R_32,Q_32>;
template class aff3ct::launcher::BFER_std<B_64,R_64,Q_64>;
#else
template class aff3ct::launcher::BFER_std<B,R,Q>;
#endif
// ==================================================================================== explicit template instantiation

