#include "Module/Puncturer/NO/Puncturer_NO.hpp"

#include "Puncturer.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Puncturer_name   = "Puncturer";
const std::string aff3ct::factory::Puncturer_prefix = "pct";

Puncturer::parameters
::parameters(const std::string &prefix)
: Factory::parameters(Puncturer_name, Puncturer_name, prefix)
{
}

Puncturer::parameters
::parameters(const std::string &name, const std::string &prefix)
: Factory::parameters(name, Puncturer_name, prefix)
{
}

Puncturer::parameters* Puncturer::parameters
::clone() const
{
	return new Puncturer::parameters(*this);
}

void Puncturer::parameters
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
		"-N,--fra-size",
		N,
		"The frame size.")
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
		"Code puncturer type.",
		true)
		->group("Standard");
}

void Puncturer::parameters
::callback_arguments()
{
	N_cw = N;
}

void Puncturer::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_prefix();

	headers[p].push_back(std::make_pair("Type", type));
	if (full) headers[p].push_back(std::make_pair("Info. bits (K)", std::to_string(K)));
	if (full) headers[p].push_back(std::make_pair("Frame size (N)", std::to_string(N)));
	if (full) headers[p].push_back(std::make_pair("Codeword size",  std::to_string(N_cw)));
	if (full) headers[p].push_back(std::make_pair("Inter frame level", std::to_string(n_frames)));
}

template <typename B, typename Q>
module::Puncturer<B,Q>* Puncturer::parameters
::build() const
{
	if (type == "NO") return new module::Puncturer_NO<B,Q>(K, N, n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Puncturer<B,Q>* Puncturer
::build(const parameters &params)
{
	return params.template build<B,Q>();
}
// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Puncturer<B_8 ,Q_8 >* aff3ct::factory::Puncturer::parameters::build<B_8 ,Q_8 >() const;
template aff3ct::module::Puncturer<B_16,Q_16>* aff3ct::factory::Puncturer::parameters::build<B_16,Q_16>() const;
template aff3ct::module::Puncturer<B_32,Q_32>* aff3ct::factory::Puncturer::parameters::build<B_32,Q_32>() const;
template aff3ct::module::Puncturer<B_64,Q_64>* aff3ct::factory::Puncturer::parameters::build<B_64,Q_64>() const;
template aff3ct::module::Puncturer<B_8 ,Q_8 >* aff3ct::factory::Puncturer::build<B_8 ,Q_8 >(const aff3ct::factory::Puncturer::parameters&);
template aff3ct::module::Puncturer<B_16,Q_16>* aff3ct::factory::Puncturer::build<B_16,Q_16>(const aff3ct::factory::Puncturer::parameters&);
template aff3ct::module::Puncturer<B_32,Q_32>* aff3ct::factory::Puncturer::build<B_32,Q_32>(const aff3ct::factory::Puncturer::parameters&);
template aff3ct::module::Puncturer<B_64,Q_64>* aff3ct::factory::Puncturer::build<B_64,Q_64>(const aff3ct::factory::Puncturer::parameters&);
#else
template aff3ct::module::Puncturer<B,Q>* aff3ct::factory::Puncturer::parameters::build<B,Q>() const;
template aff3ct::module::Puncturer<B,Q>* aff3ct::factory::Puncturer::build<B,Q>(const aff3ct::factory::Puncturer::parameters&);
#endif
// ==================================================================================== explicit template instantiation
