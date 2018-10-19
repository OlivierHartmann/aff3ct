#include "Tools/Exception/exception.hpp"
#include "Tools/Code/Polar/Frozenbits_generator/Frozenbits_generator_file.hpp"
#include "Tools/Code/Polar/Frozenbits_generator/Frozenbits_generator_TV.hpp"
#include "Tools/Code/Polar/Frozenbits_generator/Frozenbits_generator_GA.hpp"

#include "Frozenbits_generator.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Frozenbits_generator_name   = "Frozen bits generator";
const std::string aff3ct::factory::Frozenbits_generator_prefix = "fbg";

Frozenbits_generator::parameters
::parameters(const std::string &prefix)
: Factory::parameters(Frozenbits_generator_name, Frozenbits_generator_name, prefix)
{
}

Frozenbits_generator::parameters* Frozenbits_generator::parameters
::clone() const
{
	return new Frozenbits_generator::parameters(*this);
}

void Frozenbits_generator::parameters
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
		"--sigma",
		sigma,
		"Sigma value for the polar codes generation (adaptive frozen bits if sigma is not set)")
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_set(app, p, naf,
		"--gen-method",
		type,
		{"GA", "FILE", "TV"},
		"Select the frozen bits generation method.",
		true)
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--awgn-path",
		path_fb,
		"Path to a file or a directory containing the best channels to use for information bits.")
		->check(CLI::ExistingPath)
		->group("Standard");

#ifdef ENABLE_POLAR_BOUNDS
	CLI::add_option(app, p, naf,
		"--pb-path",
		path_pb,
		"Path of the polar bounds code generator (generates best channels to use).")
		->check(CLI::ExistingFile)
		->group("Standard");
#endif
}

void Frozenbits_generator::parameters
::callback_arguments()
{
}

void Frozenbits_generator::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_short_name();

	headers[p].push_back(std::make_pair("Type", type));
	if (full) headers[p].push_back(std::make_pair("Info. bits (K)", std::to_string(K)));
	if (full) headers[p].push_back(std::make_pair("Codeword size (N)", std::to_string(N_cw)));
	headers[p].push_back(std::make_pair("Sigma", sigma == -1.0f ? "adaptive" : std::to_string(sigma)));
#ifdef ENABLE_POLAR_BOUNDS
	if (type == "TV")
		headers[p].push_back(std::make_pair("PB path", path_pb));
#endif
	if (type == "TV" || type == "FILE")
		headers[p].push_back(std::make_pair("Path", path_fb));
}

tools::Frozenbits_generator* Frozenbits_generator::parameters
::build() const
{
	if (type == "GA"  ) return new tools::Frozenbits_generator_GA  (K, N_cw,                               sigma);
	if (type == "TV"  ) return new tools::Frozenbits_generator_TV  (K, N_cw, path_fb, path_pb, sigma);
	if (type == "FILE") return new tools::Frozenbits_generator_file(K, N_cw, path_fb                            );

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

tools::Frozenbits_generator* Frozenbits_generator
::build(const parameters &params)
{
	return params.build();
}
