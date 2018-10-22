#include "Tools/Exception/exception.hpp"

#include "Simulation/BFER/Iterative/SystemC/SC_BFER_ite.hpp"
#include "Simulation/BFER/Iterative/Threads/BFER_ite_threads.hpp"

#include "BFER_ite.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::BFER_ite_name   = "Simulation BFER iterative";
const std::string aff3ct::factory::BFER_ite_prefix = "sim";

BFER_ite::parameters
::parameters(const std::string &prefix)
: BFER::parameters(BFER_ite_name, prefix)
{
}

BFER_ite::parameters* BFER_ite::parameters
::clone() const
{
	return new BFER_ite::parameters(*this);
}

std::vector<std::string> BFER_ite::parameters
::get_names() const
{
	auto n = BFER::parameters::get_names();
	if (itl != nullptr) { auto nn = itl->get_names(); for (auto &x : nn) n.push_back(x); }
	return n;
}

std::vector<std::string> BFER_ite::parameters
::get_short_names() const
{
	auto sn = BFER::parameters::get_short_names();
	if (itl != nullptr) { auto nn = itl->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	return sn;
}

std::vector<std::string> BFER_ite::parameters
::get_prefixes() const
{
	auto p = BFER::parameters::get_prefixes();
	if (itl != nullptr) { auto nn = itl->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	return p;
}

void BFER_ite::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	BFER::parameters::register_arguments(app);

	CLI::add_option(app, p, naf,
		"-I,--ite",
		n_ite,
		"Number of global iterations between the demodulator and the decoder.",
		true)
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--crc-start",
		crc_start,
		"Iteration number to start the CRC checking in the turbo demodulation process.",
		true)
		->group("Standard");
}

void BFER_ite::parameters
::callback_arguments()
{
	BFER::parameters::callback_arguments();

	this->mutinfo = false;
}

void BFER_ite::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_name();

	BFER::parameters::get_headers(headers, full);

	headers[p].push_back(std::make_pair("Global iterations (I)", std::to_string(this->n_ite)));

	if (this->crc != nullptr && this->crc->type != "NO")
		headers[p].push_back(std::make_pair("CRC start ite.", std::to_string(this->crc_start)));

	if (this->src    != nullptr) { this->src->   get_headers(headers, full); }
	if (this->crc    != nullptr) { this->crc->   get_headers(headers, full); }
	if (this->cdc    != nullptr) { this->cdc->   get_headers(headers, full); }
	if (this->itl    != nullptr) { this->itl->   get_headers(headers, full); }
	if (this->mdm    != nullptr) { this->mdm->   get_headers(headers, full); }
	if (this->chn    != nullptr) { this->chn->   get_headers(headers, full); }
	if (this->qnt    != nullptr) { this->qnt->   get_headers(headers, full); }
	if (this->mnt_er != nullptr) { this->mnt_er->get_headers(headers, full); }
	if (this->ter    != nullptr) { this->ter->   get_headers(headers, full); }
}

const Codec_SISO_SIHO::parameters* BFER_ite::parameters
::get_cdc() const
{
	return dynamic_cast<Codec_SISO_SIHO::parameters*>(this->cdc.get());
}



template <typename B, typename R, typename Q>
simulation::BFER_ite<B,R,Q>* BFER_ite::parameters
::build() const
{
#if defined(SYSTEMC)
	return new simulation::SC_BFER_ite<B,R,Q>(*this);
#else
	return new simulation::BFER_ite_threads<B,R,Q>(*this);
#endif
}

template <typename B, typename R, typename Q>
simulation::BFER_ite<B,R,Q>* BFER_ite
::build(const parameters &params)
{
	return params.template build<B,R,Q>();
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::simulation::BFER_ite<B_8 ,R_8 ,Q_8 >* aff3ct::factory::BFER_ite::build<B_8 ,R_8 ,Q_8 >(const aff3ct::factory::BFER_ite::parameters&);
template aff3ct::simulation::BFER_ite<B_16,R_16,Q_16>* aff3ct::factory::BFER_ite::build<B_16,R_16,Q_16>(const aff3ct::factory::BFER_ite::parameters&);
template aff3ct::simulation::BFER_ite<B_32,R_32,Q_32>* aff3ct::factory::BFER_ite::build<B_32,R_32,Q_32>(const aff3ct::factory::BFER_ite::parameters&);
template aff3ct::simulation::BFER_ite<B_64,R_64,Q_64>* aff3ct::factory::BFER_ite::build<B_64,R_64,Q_64>(const aff3ct::factory::BFER_ite::parameters&);
#else
template aff3ct::simulation::BFER_ite<B,R,Q>* aff3ct::factory::BFER_ite::build<B,R,Q>(const aff3ct::factory::BFER_ite::parameters&);
#endif
// ==================================================================================== explicit template instantiation

