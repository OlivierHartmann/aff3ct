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
: Launcher(argc, argv, stream)
{
	params.set_src   (new factory::Source      ::parameters("src"));
	params.set_crc   (new factory::CRC         ::parameters("crc"));
	params.set_mdm   (new factory::Modem       ::parameters("mdm"));
	params.set_chn   (new factory::Channel     ::parameters("chn"));
	params.set_qnt   (new factory::Quantizer   ::parameters("qnt"));
	params.set_mnt_mi(new factory::Monitor_MI  ::parameters("mnt"));
	params.set_mnt_er(new factory::Monitor_BFER::parameters("mnt"));
	params.set_ter   (new factory::Terminal    ::parameters("ter"));

	set_params(&params);
}

template <typename B, typename R, typename Q>
void BFER_std<B,R,Q>
::register_arguments(CLI::App &app)
{
	Launcher::register_arguments(app);

	params.        register_arguments(app);
	params.src   ->register_arguments(app);
	params.crc   ->register_arguments(app);
	params.mdm   ->register_arguments(app);
	params.chn   ->register_arguments(app);
	if (std::is_integral<Q>())
	params.qnt   ->register_arguments(app);
	params.mnt_er->register_arguments(app);
	params.mnt_mi->register_arguments(app);
	params.ter   ->register_arguments(app);

	auto sub_src = app.get_subcommand(params.src     ->get_prefix());
	auto sub_crc = app.get_subcommand(params.crc     ->get_prefix());
	auto sub_enc = app.get_subcommand(params.cdc->enc->get_prefix());
	auto sub_mdm = app.get_subcommand(params.mdm     ->get_prefix());
	auto sub_chn = app.get_subcommand(params.chn     ->get_prefix());
	auto sub_mnt = app.get_subcommand(params.mnt_er  ->get_prefix()); // same than mnt_mi
	// auto sub_ter = app.get_subcommand(params.ter     ->get_prefix());

	CLI::App* sub_pct = nullptr;
	if (params.cdc->pct != nullptr && CLI::has_subcommand(app, params.cdc->pct->get_prefix()))
		sub_pct = app.get_subcommand(params.cdc->pct->get_prefix());

	CLI::App* sub_qnt = nullptr;
	if (CLI::has_subcommand(app, params.qnt->get_prefix()))
		sub_qnt = app.get_subcommand(params.qnt->get_prefix());

	if (CLI::has_option(sub_enc, "--info-bits") || CLI::has_option(sub_pct, "--info-bits"))
		CLI::remove_option(sub_src, "--info-bits");
	CLI::remove_option(sub_src, "--seed"     );

	CLI::remove_option(sub_crc, "--info-bits");
	CLI::remove_option(sub_crc, "--fra"      );

	CLI::remove_option(sub_mdm, "--fra-size" );
	CLI::remove_option(sub_mdm, "--fra"      );
	CLI::remove_option(sub_mdm, "--noise"    );

	CLI::remove_option(sub_chn, "--fra-size" );
	CLI::remove_option(sub_chn, "--fra"      );
	CLI::remove_option(sub_chn, "--noise"    );
	CLI::remove_option(sub_chn, "--seed"     );
	CLI::remove_option(sub_chn, "--add-users");
	CLI::remove_option(sub_chn, "--complex"  );

	CLI::remove_option(sub_qnt, "--size"     );
	CLI::remove_option(sub_qnt, "--fra"      );

	CLI::remove_option(sub_mnt, "--info-bits");
	CLI::remove_option(sub_mnt, "--fra-size" );
	CLI::remove_option(sub_mnt, "--fra"      );
	CLI::remove_option(sub_mnt, "--max-frame");
	CLI::remove_option(sub_mnt, "--trials"   );

#ifdef ENABLE_MPI
	CLI::remove_option(sub_ter, "--freq"     );
#endif

}

template <typename B, typename R, typename Q>
void BFER_std<B,R,Q>
::callback_arguments()
{
	Launcher::callback_arguments();

	params.callback_arguments();

	params.src->seed = params.local_seed;


	auto K    = params.src->K != 0 ? params.src->K : params.cdc->K;
	auto N    = params.src->K != 0 ? params.src->K : params.cdc->N;
	auto N_cw = params.src->K != 0 ? params.src->K : params.cdc->N_cw;

	params.crc->K = K - params.crc->size;
	params.crc->callback_arguments();

	params.src->K = params.src->K != 0 ? params.src->K : params.crc->K;
	params.src->callback_arguments();


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

