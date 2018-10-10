#include "Tools/Exception/exception.hpp"

#include "Module/Source/AZCW/Source_AZCW.hpp"
#include "Module/Source/Random/Source_random.hpp"
#include "Module/Source/Random/Source_random_fast.hpp"
#include "Module/Source/User/Source_user.hpp"

#include "Source.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Source_name   = "Source";
const std::string aff3ct::factory::Source_prefix = "src";

Source::parameters
::parameters(const std::string &prefix)
: Factory::parameters(Source_name, Source_name, prefix)
{
}

Source::parameters* Source::parameters
::clone() const
{
	return new Source::parameters(*this);
}

void Source::parameters
::register_arguments(CLI::App &app)
{
	auto sub = CLI::make_subcommand(app, get_prefix(), get_name() + " parameters");

	sub->add_option(
		"-K,--info-bits",
		K,
		"Useful number of bit transmitted (information bits).")
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
		{"RAND", "AZCW", "USER"},
		"Method used to generate the codewords.",
		true)
		->group("Standard");

	sub->add_set(
		"--implem",
		implem,
		{"STD", "FAST"},
		"Select the implementation of the algorithm to generate the information bits.",
		true)
		->group("Standard");

	sub->add_option(
		"--path",
		path,
		"Path to a file containing one or a set of pre-computed source bits (to use with \"--src-type USER\").")
		->check(CLI::ExistingFile)
		->group("Standard");

	sub->add_option(
		"--start-idx",
		start_idx,
		"Start idx to use in the USER type source.",
		true)
		->group("Standard");

	sub->add_option(
		"-S,--seed",
		seed,
		"Seed used to initialize the pseudo random generators.",
		true)
		->group("Standard");
}

void Source::parameters
::callback_arguments()
{
}

void Source::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = this->get_prefix();

	headers[p].push_back(std::make_pair("Type", this->type));
	headers[p].push_back(std::make_pair("Implementation", this->implem));
	headers[p].push_back(std::make_pair("Info. bits (K_info)", std::to_string(this->K)));
	if (full) headers[p].push_back(std::make_pair("Inter frame level", std::to_string(this->n_frames)));
	if (this->type == "USER")
		headers[p].push_back(std::make_pair("Path", this->path));
	if (this->type == "RAND" && full)
		headers[p].push_back(std::make_pair("Seed", std::to_string(this->seed)));
}

template <typename B>
module::Source<B>* Source::parameters
::build() const
{
	if (this->type == "RAND")
	{
		if (this->implem == "STD")
			return new module::Source_random     <B>(this->K, this->seed, this->n_frames);
		else if (this->implem == "FAST")
			return new module::Source_random_fast<B>(this->K, this->seed, this->n_frames);
	}

	if (this->type == "AZCW") return new module::Source_AZCW<B>(this->K,             this->n_frames);
	if (this->type == "USER") return new module::Source_user<B>(this->K, this->path, this->n_frames, this->start_idx);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B>
module::Source<B>* Source
::build(const parameters& params)
{
	return params.template build<B>();
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Source<B_8 >* aff3ct::factory::Source::parameters::build<B_8 >() const;
template aff3ct::module::Source<B_16>* aff3ct::factory::Source::parameters::build<B_16>() const;
template aff3ct::module::Source<B_32>* aff3ct::factory::Source::parameters::build<B_32>() const;
template aff3ct::module::Source<B_64>* aff3ct::factory::Source::parameters::build<B_64>() const;
template aff3ct::module::Source<B_8 >* aff3ct::factory::Source::build<B_8 >(const aff3ct::factory::Source::parameters&);
template aff3ct::module::Source<B_16>* aff3ct::factory::Source::build<B_16>(const aff3ct::factory::Source::parameters&);
template aff3ct::module::Source<B_32>* aff3ct::factory::Source::build<B_32>(const aff3ct::factory::Source::parameters&);
template aff3ct::module::Source<B_64>* aff3ct::factory::Source::build<B_64>(const aff3ct::factory::Source::parameters&);
#else
template aff3ct::module::Source<B>* aff3ct::factory::Source::parameters::build<B>() const;
template aff3ct::module::Source<B>* aff3ct::factory::Source::build<B>(const aff3ct::factory::Source::parameters&);
#endif
// ==================================================================================== explicit template instantiation
