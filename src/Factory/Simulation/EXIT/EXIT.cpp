#if !defined(PREC_8_BIT) && !defined(PREC_16_BIT)

#include "Simulation/EXIT/EXIT.hpp"
#include "Tools/general_utils.h"

#include "EXIT.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::EXIT_name   = "Simulation EXIT";
const std::string aff3ct::factory::EXIT_prefix = "sim";

EXIT::parameters
::parameters(const std::string &prefix)
: Simulation::parameters(EXIT_name, prefix)
{
	this->n_threads = 1;
}

EXIT::parameters* EXIT::parameters
::clone() const
{
	return new EXIT::parameters(*this);
}

std::vector<std::string> EXIT::parameters
::get_names() const
{
	auto n = Simulation::parameters::get_names();
	if (src != nullptr) { auto nn = src->get_names(); for (auto &x : nn) n.push_back(x); }
	if (cdc != nullptr) { auto nn = cdc->get_names(); for (auto &x : nn) n.push_back(x); }
	if (mdm != nullptr) { auto nn = mdm->get_names(); for (auto &x : nn) n.push_back(x); }
	if (chn != nullptr) { auto nn = chn->get_names(); for (auto &x : nn) n.push_back(x); }
	if (qnt != nullptr) { auto nn = qnt->get_names(); for (auto &x : nn) n.push_back(x); }
	if (mnt != nullptr) { auto nn = mnt->get_names(); for (auto &x : nn) n.push_back(x); }
	if (ter != nullptr) { auto nn = ter->get_names(); for (auto &x : nn) n.push_back(x); }
	return n;
}

std::vector<std::string> EXIT::parameters
::get_short_names() const
{
	auto sn = Factory::parameters::get_short_names();
	if (src != nullptr) { auto nn = src->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	if (cdc != nullptr) { auto nn = cdc->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	if (mdm != nullptr) { auto nn = mdm->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	if (chn != nullptr) { auto nn = chn->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	if (qnt != nullptr) { auto nn = qnt->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	if (mnt != nullptr) { auto nn = mnt->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	if (ter != nullptr) { auto nn = ter->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	return sn;
}

std::vector<std::string> EXIT::parameters
::get_prefixes() const
{
	auto p = Factory::parameters::get_prefixes();
	if (src != nullptr) { auto nn = src->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	if (cdc != nullptr) { auto nn = cdc->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	if (mdm != nullptr) { auto nn = mdm->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	if (chn != nullptr) { auto nn = chn->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	if (qnt != nullptr) { auto nn = qnt->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	if (mnt != nullptr) { auto nn = mnt->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	if (ter != nullptr) { auto nn = ter->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	return p;
}

void EXIT::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	Simulation::parameters::register_arguments(app);

	auto opt_siga_range =
	CLI::add_option(app, p, naf,
		"--siga-range",
		sig_a_range,
		"Sigma range used in EXIT charts (Matlab style: \"0.5:2.5,2.55,2.6:0.05:3\" with a default step of 0.1).")
		->required()
		->check(CLI::PositiveRange(0.f))
		->group("Standard");

	sig_a_min_option =
	CLI::add_option(app, p, naf,
		"-a,--siga-min",
		sig_a_min,
		"Sigma min value used in EXIT charts.")
		->required()
		->check(CLI::PositiveRange(0.f))
		->excludes(opt_siga_range)
		->group("Standard");

	sig_a_max_option =
	CLI::add_option(app, p, naf,
		"-A,--siga-max",
		sig_a_max,
		"Sigma max value used in EXIT charts.")
		->required()
		->check(CLI::PositiveRange(0.f))
		->excludes(opt_siga_range)
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--siga-step",
		sig_a_step,
		"Sigma step value used in EXIT charts.",
		true)
		->check(CLI::StrictlyPositiveRange(0.f))
		->excludes(opt_siga_range)
		->group("Standard");


	CLI::remove_option(app, "--threads", p, naf);
}

void EXIT::parameters
::callback_arguments()
{
	Simulation::parameters::callback_arguments();

	if (sig_a_range.empty() && !sig_a_min_option->empty() && !sig_a_max_option->empty())
		sig_a_range = tools::generate_range({{sig_a_min, sig_a_max}}, sig_a_step);
}

void EXIT::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_name();

	Simulation::parameters::get_headers(headers, full);

	if (!sig_a_range.empty())
	{
		std::stringstream range_str;
		range_str << *sig_a_range.begin() << " -> " << *sig_a_range.rbegin() << " dB";
		headers[p].push_back(std::make_pair("Sigma-A range (a)", range_str.str()));
	}

	if (this->src != nullptr && this->cdc != nullptr)
	{
		const auto bit_rate = (float)this->src->K / (float)this->cdc->N_cw;
		headers[p].push_back(std::make_pair("Bit rate", std::to_string(bit_rate)));
	}

	if (this->src != nullptr)
		headers[p].push_back(std::make_pair("Inter frame level", std::to_string(this->src->n_frames)));

	if (this->src != nullptr) { this->src->get_headers(headers, full); }
	if (this->cdc != nullptr) { this->cdc->get_headers(headers, full); }
	if (this->mdm != nullptr) { this->mdm->get_headers(headers, full); }
	if (this->chn != nullptr) { this->chn->get_headers(headers, full); }
	if (this->qnt != nullptr) { this->qnt->get_headers(headers, full); }
	if (this->mnt != nullptr) { this->mnt->get_headers(headers, full); }
	if (this->ter != nullptr) { this->ter->get_headers(headers, full); }
}

void EXIT::parameters
::set_src(Source::parameters *src)
{
	this->src.reset(src);
}

void EXIT::parameters
::set_cdc(Codec_SISO::parameters *cdc)
{
	this->cdc.reset(cdc);
}

void EXIT::parameters
::set_mdm(Modem::parameters *mdm)
{
	this->mdm.reset(mdm);
}

void EXIT::parameters
::set_chn(Channel::parameters *chn)
{
	this->chn.reset(chn);
}

void EXIT::parameters
::set_qnt(Quantizer::parameters *qnt)
{
	this->qnt.reset(qnt);
}

void EXIT::parameters
::set_mnt(Monitor_EXIT::parameters *mnt)
{
	this->mnt.reset(mnt);
}

void EXIT::parameters
::set_ter(Terminal::parameters *ter)
{
	this->ter.reset(ter);
}


template <typename B, typename R>
simulation::EXIT<B,R>* EXIT::parameters
::build() const
{
#if defined(SYSTEMC)
	throw tools::invalid_argument(__FILE__, __LINE__, __func__, "SystemC/TLM  simulation is not available.");
#else
	return new simulation::EXIT<B,R>(*this);
#endif

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename R>
simulation::EXIT<B,R>* EXIT
::build(const parameters &params)
{
	return params.template build<B,R>();
}



// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::simulation::EXIT<B_32,R_32>* aff3ct::factory::EXIT::build<B_32,R_32>(const aff3ct::factory::EXIT::parameters&);
template aff3ct::simulation::EXIT<B_64,R_64>* aff3ct::factory::EXIT::build<B_64,R_64>(const aff3ct::factory::EXIT::parameters&);
#else
template aff3ct::simulation::EXIT<B,R>* aff3ct::factory::EXIT::build<B,R>(const aff3ct::factory::EXIT::parameters&);
#endif
// ==================================================================================== explicit template instantiation

#endif