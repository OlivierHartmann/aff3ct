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
	auto p   = get_prefix();
	auto naf = no_argflag();

	CLI::add_option(app, p, naf,
		"-K,--info-bits",
		K,
		"Useful number of bit transmitted (information bits).")
		->required()
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_option(app, p, naf,
		"-N,--cw-size",
		N_cw,
		"The codeword size.")
		->required()
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_option(app, p, naf,
		"-F,--fra",
		n_frames,
		"Set the number of inter frame level to process.",
		true)
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	type_option =
	CLI::add_set(app, p, naf,
		"--type",
		type,
		type_set,
		"Type of the encoder to use in the simulation.",
		true)
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--path",
		path,
		"Path to a file containing one or a set of pre-computed codewords, to use with \"--enc-type USER\".")
		->check(CLI::ExistingFile)
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--start-idx",
		start_idx,
		"Start idx to use in the USER type encoder.",
		true)
		->group("Standard");

	CLI::add_option(app, p, naf,
		"-S,--seed",
		seed,
		"Seed used to initialize the pseudo random generators.",
		true)
		->group("Standard");
}

bool Encoder::parameters
::type_option_set_by_user() const
{
	return type_option->count() != 0;
}

void Encoder::parameters
::callback_arguments()
{
	R = (float)K / (float)N_cw;
}

void Encoder::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_name();

	headers[p].push_back(std::make_pair("Type", type));
	if (full) headers[p].push_back(std::make_pair("Info. bits (K)", std::to_string(K)));
	if (full) headers[p].push_back(std::make_pair("Codeword size (N)", std::to_string(N_cw)));
	if (full) headers[p].push_back(std::make_pair("Code rate (R)", std::to_string(R)));
	if (full) headers[p].push_back(std::make_pair("Inter frame level", std::to_string(n_frames)));
	headers[p].push_back(std::make_pair("Systematic", ((not_systematic) ? "no" : "yes")));
	if (type == "USER")
		headers[p].push_back(std::make_pair("Path", path));
	if (type == "COSET" && full)
		headers[p].push_back(std::make_pair("Seed", std::to_string(seed)));
}

template <typename B>
module::Encoder<B>* Encoder::parameters
::build() const
{
	if (type == "NO"   ) return new module::Encoder_NO   <B>(K,             n_frames);
	if (type == "AZCW" ) return new module::Encoder_AZCW <B>(K, N_cw,       n_frames);
	if (type == "COSET") return new module::Encoder_coset<B>(K, N_cw, seed, n_frames);
	if (type == "USER" ) return new module::Encoder_user <B>(K, N_cw, path, n_frames, start_idx);

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
