#include "Tools/Exception/exception.hpp"
#include "Tools/Code/SCMA/modem_SCMA_functions.hpp"

#include "Module/Modem/OOK/Modem_OOK_BSC.hpp"
#include "Module/Modem/OOK/Modem_OOK_BEC.hpp"
#include "Module/Modem/OOK/Modem_OOK_AWGN.hpp"
#include "Module/Modem/OOK/Modem_OOK_optical.hpp"
#include "Module/Modem/BPSK/Modem_BPSK.hpp"
#include "Module/Modem/BPSK/Modem_BPSK_fast.hpp"
#include "Module/Modem/PAM/Modem_PAM.hpp"
#include "Module/Modem/QAM/Modem_QAM.hpp"
#include "Module/Modem/PSK/Modem_PSK.hpp"
#include "Module/Modem/CPM/Modem_CPM.hpp"
#include "Module/Modem/SCMA/Modem_SCMA.hpp"
#include "Module/Modem/User/Modem_user.hpp"

#include "Modem.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Modem_name   = "Modem";
const std::string aff3ct::factory::Modem_prefix = "mdm";

Modem::parameters
::parameters(const std::string &prefix)
: Factory::parameters(Modem_name, Modem_name, prefix)
{
}

Modem::parameters* Modem::parameters
::clone() const
{
	return new Modem::parameters(*this);
}

void Modem::parameters
::register_arguments(CLI::App &app)
{
	auto p = get_prefix();

	// ----------------------------------------------------------------------------------------------------- modulator
	CLI::add_option(app, p,
		"-N,--fra-size",
		N,
		"Number of symbols by frame.")
		->required()
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_option(app, p,
		"-F,--fra",
		n_frames,
		"Set the number of inter frame level to process.",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_set(app, p,
		"--type",
		type,
		{"BPSK", "OOK", "PSK", "PAM", "QAM", "CPM", "USER", "SCMA"},
		"Type of the modulation to use in the simulation.",
		true)
		->group("Standard");

	CLI::add_set(app, p,
		"--implem",
		implem,
		{"STD", "FAST"},
		"Select the implementation of the modem.",
		true)
		->group("Standard");


	CLI::add_option(app, p,
		"--bps",
		bps,
		"Select the number of bits per symbol.",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_option(app, p,
		"--ups",
		ups,
		"Select the symbol sampling factor.",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_option(app, p,
		"--const-path",
		const_path,
		"Path to the ordered modulation symbols (constellation), to use with \"--type USER\".")
		->check(CLI::ExistingFile)
		->group("Standard");

	CLI::add_set(app, p,
		"--cpm-std",
		cpm_std,
		{"GSM"},
		"The selection of a default CPM standard hardly implemented (those parameters overwrite"
		" the other arguments given by the user).")
		->group("Standard");

	CLI::add_option(app, p,
		"--cpm-L",
		cpm_L,
		"CPM pulse width or CPM memory.",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_option(app, p,
		"--cpm-k",
		cpm_k,
		"Modulation index numerator.",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_option(app, p,
		"--cpm-p",
		cpm_p,
		"Modulation index denominator.",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_set(app, p,
		"--cpm-map",
		mapping,
		{"NATURAL", "GRAY"},
		"Symbols mapping layout.",
		true)
		->group("Standard");

	CLI::add_set(app, p,
		"--cpm-ws",
		wave_shape,
		{"GMSK", "REC", "RCOS"},
		"Wave shape.",
		true)
		->group("Standard");


	// --------------------------------------------------------------------------------------------------- demodulator
	CLI::add_set(app, p,
		"--max",
		wave_shape,
		{"MAX", "MAXL", "MAXS", "MAXSS"},
		"Select the type of the max operation to use in the demodulator.",
		true)
		->group("Standard");

	CLI::add_option(app, p,
		"--noise",
		noise,
		"Noise variance value for the demodulator.",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_flag(app, p,
		"--no-sig2",
		no_sig2,
		"Turn off the division by sigma square in the demodulator.")
		->group("Standard");

	CLI::add_set(app, p,
		"--psi",
		psi,
		{"PSI0", "PSI1", "PSI2", "PSI3"},
		"Select the type of the psi function to use in the SCMA demodulator.",
		true)
		->group("Standard");

	CLI::add_option(app, p,
		"--ite",
		n_ite,
		"Number of iteration in the demodulator.",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");
}

void Modem::parameters
::callback_arguments()
{
	// ----------------------------------------------------------------------------------------------------- modulator
	if (type == "CPM")
	{
		if (!cpm_std.empty())
		{
			if (cpm_std == "GSM")
			{
				cpm_L      = 3;
				cpm_k      = 1;
				cpm_p      = 2;
				bps        = 1;
				ups        = 5;
				mapping    = "NATURAL";
				wave_shape = "GMSK";
			}
			else
			{
				std::stringstream message;
				message << "Unknown CPM standard ('cpm_std' = " << cpm_std << ").";
				throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
			}
		}
	}

	// force the number of bits per symbol to 1 when BPSK mod
	if (type == "BPSK" || type == "OOK")
		bps = 1;
	// force the number of bits per symbol to 3 when SCMA mod
	else if (type == "SCMA")
		bps = 3;

	complex = is_complex_mod(type, bps);

	N_mod = get_buffer_size_after_modulation(type,
	                                         N,
	                                         bps,
	                                         ups,
	                                         cpm_L,
	                                         cpm_p);

	N_fil = get_buffer_size_after_filtering (type,
	                                         N,
	                                         bps,
	                                         cpm_L,
	                                         cpm_p);
}

void Modem::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_short_name();

	// ----------------------------------------------------------------------------------------------------- modulator
	headers[p].push_back(std::make_pair("Type",           type  ));
	headers[p].push_back(std::make_pair("Implementation", implem));
	if (full) headers[p].push_back(std::make_pair("Frame size (N)", std::to_string(N)));
	if (full) headers[p].push_back(std::make_pair("Inter frame level", std::to_string(n_frames)));
	if (type == "CPM")
	{
		if (cpm_std.size())
			headers[p].push_back(std::make_pair("CPM standard", cpm_std));

		headers[p].push_back(std::make_pair("CPM L memory", std::to_string(cpm_L)));
		headers[p].push_back(std::make_pair("CPM h index", (std::to_string(cpm_k) + std::string("/") +
		                                                    std::to_string(cpm_p))));
		headers[p].push_back(std::make_pair("CPM wave shape", wave_shape));
		headers[p].push_back(std::make_pair("CPM mapping", mapping));
	}

	headers[p].push_back(std::make_pair("Bits per symbol", std::to_string(bps)));
	headers[p].push_back(std::make_pair("Sampling factor", std::to_string(ups)));

	// --------------------------------------------------------------------------------------------------- demodulator
	std::string demod_sig2 = (no_sig2) ? "off" : "on";
	std::string demod_max  = (type == "BPSK") ||
	                         (type == "OOK" ) ||
	                         (type == "SCMA") ?
	                         "unused" : max;
	std::string demod_ite  = std::to_string(n_ite);
	std::string demod_psi  = psi;

	if (noise != -1.f && full)
		headers[p].push_back(std::make_pair("Sigma value", std::to_string(noise)));
	headers[p].push_back(std::make_pair("Sigma square", demod_sig2));
	if (demod_max != "unused")
		headers[p].push_back(std::make_pair("Max type", demod_max));
	if (type == "SCMA")
	{
		headers[p].push_back(std::make_pair("Number of iterations", demod_ite));
		headers[p].push_back(std::make_pair("Psi function", demod_psi));
	}
}

template <typename B, typename R, typename Q, tools::proto_max<Q> MAX>
module::Modem<B,R,Q>* Modem::parameters
::_build() const
{
	if (type == "BPSK" && implem == "STD" ) return new module::Modem_BPSK     <B,R,Q    >(N,                   tools::Sigma<R>((R)noise),                                                                                               no_sig2, n_frames);
	if (type == "BPSK" && implem == "FAST") return new module::Modem_BPSK_fast<B,R,Q    >(N,                   tools::Sigma<R>((R)noise),                                                                                               no_sig2, n_frames);
	if (type == "PAM"  && implem == "STD" ) return new module::Modem_PAM      <B,R,Q,MAX>(N,                   tools::Sigma<R>((R)noise), bps,                                                                                    no_sig2, n_frames);
	if (type == "QAM"  && implem == "STD" ) return new module::Modem_QAM      <B,R,Q,MAX>(N,                   tools::Sigma<R>((R)noise), bps,                                                                                    no_sig2, n_frames);
	if (type == "PSK"  && implem == "STD" ) return new module::Modem_PSK      <B,R,Q,MAX>(N,                   tools::Sigma<R>((R)noise), bps,                                                                                    no_sig2, n_frames);
	if (type == "USER" && implem == "STD" ) return new module::Modem_user     <B,R,Q,MAX>(N, const_path, tools::Sigma<R>((R)noise), bps,                                                                                    no_sig2, n_frames);
	if (type == "CPM"  && implem == "STD" ) return new module::Modem_CPM      <B,R,Q,MAX>(N,                   tools::Sigma<R>((R)noise), bps, ups, cpm_L, cpm_k, cpm_p, mapping, wave_shape, no_sig2, n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename R, typename Q>
module::Modem<B,R,Q>* Modem::parameters
::_build_scma() const
{
	if (psi == "PSI0") return new module::Modem_SCMA <B,R,Q,tools::psi_0<Q>>(N, tools::Sigma<R>((R)noise), bps, no_sig2, n_ite, n_frames);
	if (psi == "PSI1") return new module::Modem_SCMA <B,R,Q,tools::psi_1<Q>>(N, tools::Sigma<R>((R)noise), bps, no_sig2, n_ite, n_frames);
	if (psi == "PSI2") return new module::Modem_SCMA <B,R,Q,tools::psi_2<Q>>(N, tools::Sigma<R>((R)noise), bps, no_sig2, n_ite, n_frames);
	if (psi == "PSI3") return new module::Modem_SCMA <B,R,Q,tools::psi_3<Q>>(N, tools::Sigma<R>((R)noise), bps, no_sig2, n_ite, n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename R, typename Q>
module::Modem<B,R,Q>* Modem::parameters
::build(const std::string& chn_type) const
{
	if (type == "SCMA" && implem == "STD")
	{
		return _build_scma<B,R,Q>();
	}
	else if (type == "OOK" && implem == "STD")
	{
		if (chn_type == "AWGN") return new module::Modem_OOK_AWGN<B,R,Q>(N, tools::Sigma<R>((R)noise), no_sig2, n_frames);
		if (chn_type == "BEC" ) return new module::Modem_OOK_BEC <B,R,Q>(N, tools::EP   <R>((R)noise),                n_frames);
		if (chn_type == "BSC" ) return new module::Modem_OOK_BSC <B,R,Q>(N, tools::EP   <R>((R)noise),                n_frames);
	}
	else
	{
		if (max == "MAX"  ) return _build<B,R,Q,tools::max          <Q>>();
		if (max == "MAXL" ) return _build<B,R,Q,tools::max_linear   <Q>>();
		if (max == "MAXS" ) return _build<B,R,Q,tools::max_star     <Q>>();
		if (max == "MAXSS") return _build<B,R,Q,tools::max_star_safe<Q>>();
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename R, typename Q>
module::Modem<B,R,Q>* Modem::parameters
::build(const tools::Distributions<R>& dist, const std::string& chn_type) const
{
	if (type == "OOK" && implem == "STD" && chn_type == "OPTICAL")
		return new module::Modem_OOK_optical<B,R,Q>(N, dist, tools::ROP<R>((R)noise), n_frames);

	return build<B,R,Q>(chn_type);
}

template <typename B, typename R, typename Q>
module::Modem<B,R,Q>* Modem
::build(const parameters &params, const std::string& chn_type)
{
	return params.template build<B,R,Q>(chn_type);
}

template <typename B, typename R, typename Q>
module::Modem<B,R,Q>* Modem
::build(const parameters &params, const tools::Distributions<R>& dist, const std::string& chn_type)
{
	return params.template build<B,R,Q>(dist, chn_type);
}

int Modem
::get_buffer_size_after_modulation(const std::string &type,
                                   const int         N,
                                   const int         bps,
                                   const int         ups,
                                   const int         cpm_L,
                                   const int         cpm_p)
{
	if (type == "BPSK") return module::Modem_BPSK     <>::size_mod(N                        );
	if (type == "OOK" ) return module::Modem_OOK      <>::size_mod(N                        );
	if (type == "SCMA") return module::Modem_SCMA     <>::size_mod(N, bps                   );
	if (type == "PAM" ) return module::Modem_PAM      <>::size_mod(N, bps                   );
	if (type == "QAM" ) return module::Modem_QAM      <>::size_mod(N, bps                   );
	if (type == "PSK" ) return module::Modem_PSK      <>::size_mod(N, bps                   );
	if (type == "USER") return module::Modem_user     <>::size_mod(N, bps                   );
	if (type == "CPM" ) return module::Modem_CPM      <>::size_mod(N, bps, cpm_L, cpm_p, ups);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

int Modem
::get_buffer_size_after_filtering(const std::string &type,
                                  const int         N,
                                  const int         bps,
                                  const int         cpm_L,
                                  const int         cpm_p)
{
	if (type == "BPSK") return module::Modem_BPSK<>::size_fil(N                   );
	if (type == "OOK" ) return module::Modem_OOK <>::size_fil(N                   );
	if (type == "SCMA") return module::Modem_SCMA<>::size_fil(N, bps              );
	if (type == "PAM" ) return module::Modem_PAM <>::size_fil(N, bps              );
	if (type == "QAM" ) return module::Modem_QAM <>::size_fil(N, bps              );
	if (type == "PSK" ) return module::Modem_PSK <>::size_fil(N, bps              );
	if (type == "USER") return module::Modem_user<>::size_fil(N, bps              );
	if (type == "CPM" ) return module::Modem_CPM <>::size_fil(N, bps, cpm_L, cpm_p);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

bool Modem
::is_complex_mod(const std::string &type, const int bps)
{
	if (type == "BPSK") return module::Modem_BPSK<>::is_complex_mod();
	if (type == "OOK" ) return module::Modem_OOK <>::is_complex_mod();
	if (type == "SCMA") return module::Modem_SCMA<>::is_complex_mod();
	if (type == "PAM" ) return module::Modem_PAM <>::is_complex_mod();
	if (type == "QAM" ) return module::Modem_QAM <>::is_complex_mod();
	if (type == "PSK" ) return module::Modem_PSK <>::is_complex_mod();
	if (type == "USER") return module::Modem_user<>::is_complex_mod();
	if (type == "CPM" ) return module::Modem_CPM <>::is_complex_mod();

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

bool Modem
::is_complex_fil(const std::string &type, const int bps)
{
	if (type == "BPSK") return module::Modem_BPSK<>::is_complex_fil();
	if (type == "OOK" ) return module::Modem_OOK <>::is_complex_fil();
	if (type == "SCMA") return module::Modem_SCMA<>::is_complex_fil();
	if (type == "PAM" ) return module::Modem_PAM <>::is_complex_fil();
	if (type == "QAM" ) return module::Modem_QAM <>::is_complex_fil();
	if (type == "PSK" ) return module::Modem_PSK <>::is_complex_fil();
	if (type == "USER") return module::Modem_user<>::is_complex_fil();
	if (type == "CPM" ) return module::Modem_CPM <>::is_complex_fil();

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Modem<B_8 ,R_8 ,Q_8 >* aff3ct::factory::Modem::parameters::build<B_8 ,R_8 ,Q_8 >(const std::string&) const;
template aff3ct::module::Modem<B_8 ,R_8 ,R_8 >* aff3ct::factory::Modem::parameters::build<B_8 ,R_8 ,R_8 >(const std::string&) const;
template aff3ct::module::Modem<B_16,R_16,Q_16>* aff3ct::factory::Modem::parameters::build<B_16,R_16,Q_16>(const std::string&) const;
template aff3ct::module::Modem<B_16,R_16,R_16>* aff3ct::factory::Modem::parameters::build<B_16,R_16,R_16>(const std::string&) const;
template aff3ct::module::Modem<B_32,R_32,Q_32>* aff3ct::factory::Modem::parameters::build<B_32,R_32,Q_32>(const std::string&) const;
template aff3ct::module::Modem<B_64,R_64,Q_64>* aff3ct::factory::Modem::parameters::build<B_64,R_64,Q_64>(const std::string&) const;
template aff3ct::module::Modem<B_8 ,R_8 ,Q_8 >* aff3ct::factory::Modem::build<B_8 ,R_8 ,Q_8 >(const aff3ct::factory::Modem::parameters&, const std::string&);
template aff3ct::module::Modem<B_8 ,R_8 ,R_8 >* aff3ct::factory::Modem::build<B_8 ,R_8 ,R_8 >(const aff3ct::factory::Modem::parameters&, const std::string&);
template aff3ct::module::Modem<B_16,R_16,Q_16>* aff3ct::factory::Modem::build<B_16,R_16,Q_16>(const aff3ct::factory::Modem::parameters&, const std::string&);
template aff3ct::module::Modem<B_16,R_16,R_16>* aff3ct::factory::Modem::build<B_16,R_16,R_16>(const aff3ct::factory::Modem::parameters&, const std::string&);
template aff3ct::module::Modem<B_32,R_32,Q_32>* aff3ct::factory::Modem::build<B_32,R_32,Q_32>(const aff3ct::factory::Modem::parameters&, const std::string&);
template aff3ct::module::Modem<B_64,R_64,Q_64>* aff3ct::factory::Modem::build<B_64,R_64,Q_64>(const aff3ct::factory::Modem::parameters&, const std::string&);

template aff3ct::module::Modem<B_8 ,R_8 ,Q_8 >* aff3ct::factory::Modem::parameters::build<B_8 ,R_8 ,Q_8 >(const tools::Distributions<R_8 >&, const std::string&) const;
template aff3ct::module::Modem<B_8 ,R_8 ,R_8 >* aff3ct::factory::Modem::parameters::build<B_8 ,R_8 ,R_8 >(const tools::Distributions<R_8 >&, const std::string&) const;
template aff3ct::module::Modem<B_16,R_16,Q_16>* aff3ct::factory::Modem::parameters::build<B_16,R_16,Q_16>(const tools::Distributions<R_16>&, const std::string&) const;
template aff3ct::module::Modem<B_16,R_16,R_16>* aff3ct::factory::Modem::parameters::build<B_16,R_16,R_16>(const tools::Distributions<R_16>&, const std::string&) const;
template aff3ct::module::Modem<B_32,R_32,Q_32>* aff3ct::factory::Modem::parameters::build<B_32,R_32,Q_32>(const tools::Distributions<R_32>&, const std::string&) const;
template aff3ct::module::Modem<B_64,R_64,Q_64>* aff3ct::factory::Modem::parameters::build<B_64,R_64,Q_64>(const tools::Distributions<R_64>&, const std::string&) const;
template aff3ct::module::Modem<B_8 ,R_8 ,Q_8 >* aff3ct::factory::Modem::build<B_8 ,R_8 ,Q_8 >(const aff3ct::factory::Modem::parameters&, const tools::Distributions<R_8 >&, const std::string&);
template aff3ct::module::Modem<B_8 ,R_8 ,R_8 >* aff3ct::factory::Modem::build<B_8 ,R_8 ,R_8 >(const aff3ct::factory::Modem::parameters&, const tools::Distributions<R_8 >&, const std::string&);
template aff3ct::module::Modem<B_16,R_16,Q_16>* aff3ct::factory::Modem::build<B_16,R_16,Q_16>(const aff3ct::factory::Modem::parameters&, const tools::Distributions<R_16>&, const std::string&);
template aff3ct::module::Modem<B_16,R_16,R_16>* aff3ct::factory::Modem::build<B_16,R_16,R_16>(const aff3ct::factory::Modem::parameters&, const tools::Distributions<R_16>&, const std::string&);
template aff3ct::module::Modem<B_32,R_32,Q_32>* aff3ct::factory::Modem::build<B_32,R_32,Q_32>(const aff3ct::factory::Modem::parameters&, const tools::Distributions<R_32>&, const std::string&);
template aff3ct::module::Modem<B_64,R_64,Q_64>* aff3ct::factory::Modem::build<B_64,R_64,Q_64>(const aff3ct::factory::Modem::parameters&, const tools::Distributions<R_64>&, const std::string&);
#else
template aff3ct::module::Modem<B,R,Q>* aff3ct::factory::Modem::parameters::build<B,R,Q>(const std::string&) const;
template aff3ct::module::Modem<B,R,Q>* aff3ct::factory::Modem::build<B,R,Q>(const aff3ct::factory::Modem::parameters&, const std::string&);

template aff3ct::module::Modem<B,R,Q>* aff3ct::factory::Modem::parameters::build<B,R,Q>(const tools::Distributions<R>&, const std::string&) const;
template aff3ct::module::Modem<B,R,Q>* aff3ct::factory::Modem::build<B,R,Q>(const aff3ct::factory::Modem::parameters&, const tools::Distributions<R>&, const std::string&);
#if !defined(PREC_32_BIT) && !defined(PREC_64_BIT)
template aff3ct::module::Modem<B,R,R>* aff3ct::factory::Modem::parameters::build<B,R,R>(const std::string&) const;
template aff3ct::module::Modem<B,R,R>* aff3ct::factory::Modem::build<B,R,R>(const aff3ct::factory::Modem::parameters&, const std::string&);

template aff3ct::module::Modem<B,R,R>* aff3ct::factory::Modem::parameters::build<B,R,R>(const tools::Distributions<R>&, const std::string&) const;
template aff3ct::module::Modem<B,R,R>* aff3ct::factory::Modem::build<B,R,R>(const aff3ct::factory::Modem::parameters&, const tools::Distributions<R>&, const std::string&);
#endif
#endif
// ==================================================================================== explicit template instantiation
