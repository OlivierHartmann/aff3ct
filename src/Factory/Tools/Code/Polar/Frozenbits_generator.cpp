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
	auto sub = make_subcommand(app, get_prefix(), get_name() + " parameters");

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
		"--sigma",
		sigma,
		"Sigma value for the polar codes generation (adaptive frozen bits if sigma is not set)")
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	sub->add_set(
		"--gen-method",
		type,
		{"GA", "FILE", "TV"},
		"Select the frozen bits generation method.",
		true)
		->group("Standard");

	sub->add_option(
		"--awgn-path",
		path_fb,
		"Path to a file or a directory containing the best channels to use for information bits.")
		->check(CLI::ExistingFile)
		->group("Standard");

#ifdef ENABLE_POLAR_BOUNDS
	sub->add_option(
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
	auto p = this->get_prefix();

	headers[p].push_back(std::make_pair("Type", this->type));
	if (full) headers[p].push_back(std::make_pair("Info. bits (K)", std::to_string(this->K)));
	if (full) headers[p].push_back(std::make_pair("Codeword size (N)", std::to_string(this->N_cw)));
	headers[p].push_back(std::make_pair("Sigma", this->sigma == -1.0f ? "adaptive" : std::to_string(this->sigma)));
#ifdef ENABLE_POLAR_BOUNDS
	if (this->type == "TV")
		headers[p].push_back(std::make_pair("PB path", this->path_pb));
#endif
	if (this->type == "TV" || this->type == "FILE")
		headers[p].push_back(std::make_pair("Path", this->path_fb));
}

tools::Frozenbits_generator* Frozenbits_generator::parameters
::build() const
{
	if (this->type == "GA"  ) return new tools::Frozenbits_generator_GA  (this->K, this->N_cw,                               this->sigma);
	if (this->type == "TV"  ) return new tools::Frozenbits_generator_TV  (this->K, this->N_cw, this->path_fb, this->path_pb, this->sigma);
	if (this->type == "FILE") return new tools::Frozenbits_generator_file(this->K, this->N_cw, this->path_fb                            );

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

tools::Frozenbits_generator* Frozenbits_generator
::build(const parameters &params)
{
	return params.build();
}
