#include "Tools/Exception/exception.hpp"

#include "Module/Encoder/NO/Encoder_NO.hpp"
#include "Module/Encoder/AZCW/Encoder_AZCW.hpp"
#include "Module/Encoder/Coset/Encoder_coset.hpp"
#include "Module/Encoder/User/Encoder_user.hpp"

#include "Encoder.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Encoder_name   = "Encoder";
const std::string aff3ct::factory::Encoder_prefix = "enc";

Encoder::parameters
::parameters(const std::string &prefix)
: Factory::parameters(Encoder_name, Encoder_name, prefix)
{
}

Encoder::parameters
::parameters(const std::string &name, const std::string &prefix)
: Factory::parameters(name, Encoder_name, prefix)
{
}

Encoder::parameters* Encoder::parameters
::clone() const
{
	return new Encoder::parameters(*this);
}

void Encoder::parameters
::register_arguments(CLI::App &app)
{
	auto p = this->get_prefix();

	auto sub = CLI::make_subcommand(app, get_prefix(), get_name() + " parameters");

	sub->add_option(
		"-K,--info-bits",
		K,
		"Useful number of bit transmitted (information bits).")
		->required()
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	sub->add_option(
		"-N,--cw-size",
		N_cw,
		"The codeword size.")
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
		"Type of the encoder to use in the simulation.",
		true)
		->group("Standard");

	sub->add_option(
		"--path",
		path,
		"Path to a file containing one or a set of pre-computed codewords, to use with \"--enc-type USER\".")
		->check(CLI::ExistingFile)
		->group("Standard");

	sub->add_option(
		"--start-idx",
		start_idx,
		"Start idx to use in the USER type encoder.",
		true)
		->group("Standard");

	sub->add_option(
		"-S,--seed",
		seed,
		"Seed used to initialize the pseudo random generators.",
		true)
		->group("Standard");
}

void Encoder::parameters
::callback_arguments()
{
	this->R = (float)this->K / (float)this->N_cw;
}

void Encoder::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = this->get_prefix();

	headers[p].push_back(std::make_pair("Type", this->type));
	if (full) headers[p].push_back(std::make_pair("Info. bits (K)", std::to_string(this->K)));
	if (full) headers[p].push_back(std::make_pair("Codeword size (N)", std::to_string(this->N_cw)));
	if (full) headers[p].push_back(std::make_pair("Code rate (R)", std::to_string(this->R)));
	if (full) headers[p].push_back(std::make_pair("Inter frame level", std::to_string(this->n_frames)));
	headers[p].push_back(std::make_pair("Systematic", ((this->not_systematic) ? "no" : "yes")));
	if (this->type == "USER")
		headers[p].push_back(std::make_pair("Path", this->path));
	if (this->type == "COSET" && full)
		headers[p].push_back(std::make_pair("Seed", std::to_string(this->seed)));
}

template <typename B>
module::Encoder<B>* Encoder::parameters
::build() const
{
	if (this->type == "NO"   ) return new module::Encoder_NO   <B>(this->K,                         this->n_frames);
	if (this->type == "AZCW" ) return new module::Encoder_AZCW <B>(this->K, this->N_cw,             this->n_frames);
	if (this->type == "COSET") return new module::Encoder_coset<B>(this->K, this->N_cw, this->seed, this->n_frames);
	if (this->type == "USER" ) return new module::Encoder_user <B>(this->K, this->N_cw, this->path, this->n_frames, this->start_idx);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B>
module::Encoder<B>* Encoder
::build(const parameters &params)
{
	return params.template build<B>();
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Encoder<B_8 >* aff3ct::factory::Encoder::parameters::build<B_8 >() const;
template aff3ct::module::Encoder<B_16>* aff3ct::factory::Encoder::parameters::build<B_16>() const;
template aff3ct::module::Encoder<B_32>* aff3ct::factory::Encoder::parameters::build<B_32>() const;
template aff3ct::module::Encoder<B_64>* aff3ct::factory::Encoder::parameters::build<B_64>() const;
template aff3ct::module::Encoder<B_8 >* aff3ct::factory::Encoder::build<B_8 >(const aff3ct::factory::Encoder::parameters&);
template aff3ct::module::Encoder<B_16>* aff3ct::factory::Encoder::build<B_16>(const aff3ct::factory::Encoder::parameters&);
template aff3ct::module::Encoder<B_32>* aff3ct::factory::Encoder::build<B_32>(const aff3ct::factory::Encoder::parameters&);
template aff3ct::module::Encoder<B_64>* aff3ct::factory::Encoder::build<B_64>(const aff3ct::factory::Encoder::parameters&);
#else
template aff3ct::module::Encoder<B>* aff3ct::factory::Encoder::parameters::build<B>() const;
template aff3ct::module::Encoder<B>* aff3ct::factory::Encoder::build<B>(const aff3ct::factory::Encoder::parameters&);
#endif
// ==================================================================================== explicit template instantiation
