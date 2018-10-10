#include "Tools/Exception/exception.hpp"

#include "Module/Channel/NO/Channel_NO.hpp"
#include "Module/Channel/User/Channel_user.hpp"
#include "Module/Channel/AWGN/Channel_AWGN_LLR.hpp"
#include "Module/Channel/Rayleigh/Channel_Rayleigh_LLR.hpp"
#include "Module/Channel/Rayleigh/Channel_Rayleigh_LLR_user.hpp"
#include "Module/Channel/Optical/Channel_optical.hpp"
#include "Module/Channel/Binary_erasure/Channel_binary_erasure.hpp"
#include "Module/Channel/Binary_symmetric/Channel_binary_symmetric.hpp"

#include "Tools/Algo/Draw_generator/Gaussian_noise_generator/Standard/Gaussian_noise_generator_std.hpp"
#include "Tools/Algo/Draw_generator/Gaussian_noise_generator/Fast/Gaussian_noise_generator_fast.hpp"
#include "Tools/Algo/Draw_generator/Event_generator/Standard/Event_generator_std.hpp"
#include "Tools/Algo/Draw_generator/Event_generator/Fast/Event_generator_fast.hpp"
#include "Tools/Algo/Draw_generator/User_pdf_noise_generator/Standard/User_pdf_noise_generator_std.hpp"
#include "Tools/Algo/Draw_generator/User_pdf_noise_generator/Fast/User_pdf_noise_generator_fast.hpp"
#ifdef CHANNEL_MKL
#include "Tools/Algo/Draw_generator/Event_generator/MKL/Event_generator_MKL.hpp"
#include "Tools/Algo/Draw_generator/Gaussian_noise_generator/MKL/Gaussian_noise_generator_MKL.hpp"
#include "Tools/Algo/Draw_generator/User_pdf_noise_generator/MKL/User_pdf_noise_generator_MKL.hpp"
#endif
#ifdef CHANNEL_GSL
#include "Tools/Algo/Draw_generator/Event_generator/GSL/Event_generator_GSL.hpp"
#include "Tools/Algo/Draw_generator/Gaussian_noise_generator/GSL/Gaussian_noise_generator_GSL.hpp"
#include "Tools/Algo/Draw_generator/User_pdf_noise_generator/GSL/User_pdf_noise_generator_GSL.hpp"
#endif

#include "Channel.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Channel_name   = "Channel";
const std::string aff3ct::factory::Channel_prefix = "chn";

Channel::parameters
::parameters(const std::string &prefix)
: Factory::parameters(Channel_name, Channel_name, prefix)
{
}

Channel::parameters* Channel::parameters
::clone() const
{
	return new Channel::parameters(*this);
}

void Channel::parameters
::register_arguments(CLI::App &app)
{
	auto sub = CLI::make_subcommand(app, get_prefix(), get_name() + " parameters");

	sub->add_option(
		"-N,--fra-size",
		N,
		"Number of symbols by frame.")
		->required()
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	sub->add_option(
		"-F,--fra",
		n_frames,
		"Set the number of inter frame level to process.",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");


	sub->add_set(
		"--type",
		type,
		type_set,
		"Type of the channel to use in the simulation ('USER' has an output got from a file when 'USER_ADD' has an"
		" additive noise got from a file).",
		true)
		->group("Standard");

	sub->add_set(
		"--implem",
		implem,
		implem_set,
		"Select the implementation of the algorithm to generate noise.",
		true)
		->group("Standard");


#ifdef CHANNEL_GSL
	implem_set.insert("GSL");
#endif
#ifdef CHANNEL_MKL
	implem_set.insert("MKL");
#endif

	sub->add_option(
		"--path",
		path,
		"Path to a noisy file, to use with \"--chn-type USER,OPTICAL\" or to a gain file (used with \"--chn-type RAYLEIGH_USER\").")
		->check(CLI::ExistingFile)
		->group("Standard");

	sub->add_set(
		"--blk-fad",
		block_fading,
		{"NO", "FRAME", "ONETAP"},
		"Block fading policy for the RAYLEIGH channel.",
		true)
		->group("Standard");

	sub->add_option(
		"--noise",
		noise,
		"Noise value (for SIGMA, ROP or EP noise type).")
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	sub->add_option(
		"-S,--seed",
		seed,
		"Seed used to initialize the pseudo random generators.",
		true)
		->group("Standard");

	sub->add_flag(
		"--add-users",
		add_users,
		"Add all the users (= frames) before generating the noise.")
		->group("Standard");

	sub->add_flag(
		"--complex",
		complex,
		"Enable complex noise generation.")
		->group("Standard");

	sub->add_option(
		"--gain-occur",
		gain_occur,
		"The number of times a gain is used on consecutive symbols (used with \"--chn-type RAYLEIGH_USER\")",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");
}

void Channel::parameters
::callback_arguments()
{
}

void Channel::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = this->get_prefix();

	headers[p].push_back(std::make_pair("Type",           this->type  ));
	headers[p].push_back(std::make_pair("Implementation", this->implem));

	if (full) headers[p].push_back(std::make_pair("Frame size (N)", std::to_string(this->N)));
	if (full) headers[p].push_back(std::make_pair("Inter frame level", std::to_string(this->n_frames)));

	if (this->noise != -1.f)
		headers[p].push_back(std::make_pair("Sigma value", std::to_string(this->noise)));

	if (this->type == "USER" || this->type == "USER_ADD" || this->type == "RAYLEIGH_USER")
		headers[p].push_back(std::make_pair("Path", this->path));

	if (this->type == "RAYLEIGH_USER")
		headers[p].push_back(std::make_pair("Gain occurrences", std::to_string(this->gain_occur)));

	if (this->type.find("RAYLEIGH") != std::string::npos)
		headers[p].push_back(std::make_pair("Block fading policy", this->block_fading));

	if ((this->type != "NO" && this->type != "USER" && this->type != "USER_ADD") && full)
		headers[p].push_back(std::make_pair("Seed", std::to_string(this->seed)));

	headers[p].push_back(std::make_pair("Complex", this->complex ? "on" : "off"));
	headers[p].push_back(std::make_pair("Add users", this->add_users ? "on" : "off"));
}

template <typename R>
module::Channel<R>* Channel::parameters
::build_event() const
{
	std::unique_ptr<tools::Event_generator<R>> n;
	     if (implem == "STD" ) n.reset(new tools::Event_generator_std <R>(seed));
	else if (implem == "FAST") n.reset(new tools::Event_generator_fast<R>(seed));
#ifdef CHANNEL_MKL
	else if (implem == "MKL" ) n.reset(new tools::Event_generator_MKL<R>(seed));
#endif
#ifdef CHANNEL_GSL
	else if (implem == "GSL" ) n.reset(new tools::Event_generator_GSL<R>(seed));
#endif
	else
		throw tools::cannot_allocate(__FILE__, __LINE__, __func__);

	if (type == "BEC") return new module::Channel_binary_erasure  <R>(N, std::move(n), tools::Event_probability<R>((R)noise), n_frames);
	if (type == "BSC") return new module::Channel_binary_symmetric<R>(N, std::move(n), tools::Event_probability<R>((R)noise), n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename R>
module::Channel<R>* Channel::parameters
::build_gaussian() const
{
	std::unique_ptr<tools::Gaussian_noise_generator<R>> n = nullptr;
	     if (implem == "STD" ) n.reset(new tools::Gaussian_noise_generator_std <R>(seed));
	else if (implem == "FAST") n.reset(new tools::Gaussian_noise_generator_fast<R>(seed));
#ifdef CHANNEL_MKL
	else if (implem == "MKL" ) n.reset(new tools::Gaussian_noise_generator_MKL <R>(seed));
#endif
#ifdef CHANNEL_GSL
	else if (implem == "GSL" ) n.reset(new tools::Gaussian_noise_generator_GSL <R>(seed));
#endif
	else
		throw tools::cannot_allocate(__FILE__, __LINE__, __func__);

	if (type == "AWGN"         ) return new module::Channel_AWGN_LLR         <R>(N,                std::move(n),             add_users, tools::Sigma<R>((R)noise), n_frames);
	if (type == "RAYLEIGH"     ) return new module::Channel_Rayleigh_LLR     <R>(N, complex,       std::move(n),             add_users, tools::Sigma<R>((R)noise), n_frames);
	if (type == "RAYLEIGH_USER") return new module::Channel_Rayleigh_LLR_user<R>(N, complex, path, std::move(n), gain_occur, add_users, tools::Sigma<R>((R)noise), n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename R>
module::Channel<R>* Channel::parameters
::build_userpdf(const tools::Distributions<R>& dist) const
{
	std::unique_ptr<tools::User_pdf_noise_generator<R>> n = nullptr;
	     if (implem == "STD" ) n.reset(new tools::User_pdf_noise_generator_std <R>(dist, seed));
	else if (implem == "FAST") n.reset(new tools::User_pdf_noise_generator_fast<R>(dist, seed));
#ifdef CHANNEL_MKL
	else if (implem == "MKL" ) n.reset(new tools::User_pdf_noise_generator_MKL <R>(dist, seed));
#endif
#ifdef CHANNEL_GSL
	else if (implem == "GSL" ) n.reset(new tools::User_pdf_noise_generator_GSL <R>(dist, seed));
#endif
	else
		throw tools::cannot_allocate(__FILE__, __LINE__, __func__);

	if (type == "OPTICAL") return new module::Channel_optical<R>(N, std::move(n), tools::Received_optical_power<R>((R) this->noise), n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename R>
module::Channel<R>* Channel::parameters
::build() const
{
	try	{
		return build_gaussian<R>();
	} catch (tools::cannot_allocate&) {}

	try	{
		return build_event<R>();
	} catch (tools::cannot_allocate&) {}

	if (type == "USER"    ) return new module::Channel_user<R>(N, path, add_users, false, n_frames);
	if (type == "USER_ADD") return new module::Channel_user<R>(N, path, add_users,  true, n_frames);
	if (type == "NO"      ) return new module::Channel_NO  <R>(N,       add_users,        n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename R>
module::Channel<R>* Channel::parameters
::build(const tools::Distributions<R>& dist) const
{
	try	{
		return build_userpdf<R>(dist);
	} catch (tools::cannot_allocate&) {}

	return build<R>();
}

template <typename R>
module::Channel<R>* Channel
::build(const parameters &params)
{
	return params.template build<R>();
}

template <typename R>
module::Channel<R>* Channel
::build(const parameters &params, const tools::Distributions<R>& dist)
{
	return params.template build<R>(dist);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Channel<R_32>* aff3ct::factory::Channel::parameters::build<R_32>() const;
template aff3ct::module::Channel<R_64>* aff3ct::factory::Channel::parameters::build<R_64>() const;
template aff3ct::module::Channel<R_32>* aff3ct::factory::Channel::build<R_32>(const aff3ct::factory::Channel::parameters&);
template aff3ct::module::Channel<R_64>* aff3ct::factory::Channel::build<R_64>(const aff3ct::factory::Channel::parameters&);

template aff3ct::module::Channel<R_32>* aff3ct::factory::Channel::parameters::build<R_32>(const tools::Distributions<R_32>&) const;
template aff3ct::module::Channel<R_64>* aff3ct::factory::Channel::parameters::build<R_64>(const tools::Distributions<R_64>&) const;
template aff3ct::module::Channel<R_32>* aff3ct::factory::Channel::build<R_32>(const aff3ct::factory::Channel::parameters&, const tools::Distributions<R_32>&);
template aff3ct::module::Channel<R_64>* aff3ct::factory::Channel::build<R_64>(const aff3ct::factory::Channel::parameters&, const tools::Distributions<R_64>&);
#else
template aff3ct::module::Channel<R>* aff3ct::factory::Channel::parameters::build<R>() const;
template aff3ct::module::Channel<R>* aff3ct::factory::Channel::build<R>(const aff3ct::factory::Channel::parameters&);

template aff3ct::module::Channel<R>* aff3ct::factory::Channel::parameters::build<R>(const tools::Distributions<R>&) const;
template aff3ct::module::Channel<R>* aff3ct::factory::Channel::build<R>(const aff3ct::factory::Channel::parameters&, const tools::Distributions<R>&);
#endif
// ==================================================================================== explicit template instantiation
