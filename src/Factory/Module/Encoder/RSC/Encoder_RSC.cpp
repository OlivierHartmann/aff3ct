#include <algorithm>
#include <cmath>

#include "Tools/Exception/exception.hpp"

#include "Module/Encoder/RSC/Encoder_RSC_generic_sys.hpp"
#include "Module/Encoder/RSC/Encoder_RSC_generic_json_sys.hpp"

#include "Encoder_RSC.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Encoder_RSC_name   = "Encoder RSC";
const std::string aff3ct::factory::Encoder_RSC_prefix = "enc";

Encoder_RSC::parameters
::parameters(const std::string &prefix)
: Encoder::parameters(Encoder_RSC_name, prefix)
{
	type = "RSC";

	type_set.insert({"RSC"});
}

Encoder_RSC::parameters* Encoder_RSC::parameters
::clone() const
{
	return new Encoder_RSC::parameters(*this);
}

void Encoder_RSC::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	Encoder::parameters::register_arguments(app);

	CLI::remove_option(app, "--cw-size", p, naf);

	CLI::add_flag(app, p, naf,
		"--no-buff",
		not_buffered,
		"Disable the buffered encoding.")
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--poly",
		poly_str,
		"The polynomials describing RSC code, should be of the form \"{A,B}\" in octal base.",
		true)
		->group("Standard");

	CLI::add_set(app, p, naf,
		"--std",
		standard,
		{"LTE", "CCSDS"},
		"Select a standard and set automatically some parameters (overwritten by \"--poly\").",
		true)
		->group("Standard");
}

void Encoder_RSC::parameters
::callback_arguments()
{
	Encoder::parameters::callback_arguments();

	auto p = get_prefix();

	if (standard == "LTE")
		poly = {013, 015};

	if (standard == "CCSDS")
		poly = {023, 033};

	if (poly_str != "")
	{
		standard = "";

#ifdef _MSC_VER
		sscanf_s   (poly_str.c_str(), "{%o,%o}", &poly[0], &poly[1]);
#else
		std::sscanf(poly_str.c_str(), "{%o,%o}", &poly[0], &poly[1]);
#endif
	}

	if (poly[0] == 013 && poly[1] == 015)
		standard = "LTE";

	if (poly[0] == 023 && poly[1] == 033)
		standard = "CCSDS";

	tail_length = (int)(2 * std::floor(std::log2((float)std::max(poly[0], poly[1]))));
	N_cw        = 2 * K + tail_length;
	R           = (float)K / (float)N_cw;
}

void Encoder_RSC::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_short_name();

	Encoder::parameters::get_headers(headers, full);

	if (tail_length)
		headers[p].push_back(std::make_pair("Tail length", std::to_string(tail_length)));

	headers[p].push_back(std::make_pair("Buffered", (!not_buffered ? "on" : "off")));

	if (!standard.empty())
		headers[p].push_back(std::make_pair("Standard", standard));

	std::stringstream ss_poly;
	ss_poly << "{0" << std::oct << poly[0] << ",0" << std::oct << poly[1] << "}";
	headers[p].push_back(std::make_pair(std::string("Polynomials"), ss_poly.str()));
}

template <typename B>
module::Encoder_RSC_sys<B>* Encoder_RSC::parameters
::build(std::ostream &stream) const
{
	if (type == "RSC_JSON") return new module::Encoder_RSC_generic_json_sys<B>(K, N_cw, !not_buffered, poly, stream, n_frames);
	if (type == "RSC"     ) return new module::Encoder_RSC_generic_sys     <B>(K, N_cw, !not_buffered, poly,         n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B>
module::Encoder_RSC_sys<B>* Encoder_RSC
::build(const parameters &params, std::ostream &stream)
{
	return params.template build<B>(stream);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Encoder_RSC_sys<B_8 >* aff3ct::factory::Encoder_RSC::parameters::build<B_8 >(std::ostream&) const;
template aff3ct::module::Encoder_RSC_sys<B_16>* aff3ct::factory::Encoder_RSC::parameters::build<B_16>(std::ostream&) const;
template aff3ct::module::Encoder_RSC_sys<B_32>* aff3ct::factory::Encoder_RSC::parameters::build<B_32>(std::ostream&) const;
template aff3ct::module::Encoder_RSC_sys<B_64>* aff3ct::factory::Encoder_RSC::parameters::build<B_64>(std::ostream&) const;
template aff3ct::module::Encoder_RSC_sys<B_8 >* aff3ct::factory::Encoder_RSC::build<B_8 >(const aff3ct::factory::Encoder_RSC::parameters&, std::ostream&);
template aff3ct::module::Encoder_RSC_sys<B_16>* aff3ct::factory::Encoder_RSC::build<B_16>(const aff3ct::factory::Encoder_RSC::parameters&, std::ostream&);
template aff3ct::module::Encoder_RSC_sys<B_32>* aff3ct::factory::Encoder_RSC::build<B_32>(const aff3ct::factory::Encoder_RSC::parameters&, std::ostream&);
template aff3ct::module::Encoder_RSC_sys<B_64>* aff3ct::factory::Encoder_RSC::build<B_64>(const aff3ct::factory::Encoder_RSC::parameters&, std::ostream&);
#else
template aff3ct::module::Encoder_RSC_sys<B>* aff3ct::factory::Encoder_RSC::parameters::build<B>(std::ostream&) const;
template aff3ct::module::Encoder_RSC_sys<B>* aff3ct::factory::Encoder_RSC::build<B>(const aff3ct::factory::Encoder_RSC::parameters&, std::ostream&);
#endif
// ==================================================================================== explicit template instantiation
