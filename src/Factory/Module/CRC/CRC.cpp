#include "Tools/Exception/exception.hpp"
#include "Tools/types.h"

#include "Module/CRC/NO/CRC_NO.hpp"
#include "Module/CRC/Polynomial/CRC_polynomial.hpp"
#include "Module/CRC/Polynomial/CRC_polynomial_fast.hpp"
#include "Module/CRC/Polynomial/CRC_polynomial_inter.hpp"

#include "CRC.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::CRC_name   = "CRC";
const std::string aff3ct::factory::CRC_prefix = "crc";

CRC::parameters
::parameters(const std::string &prefix)
: Factory::parameters(CRC_name, CRC_name, prefix)
{
}

CRC::parameters* CRC::parameters
::clone() const
{
	return new CRC::parameters(*this);
}

void CRC::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	CLI::add_option(app, p, naf,
		"-K,--info-bits",
		K,
		"Number of generated bits (information bits, the CRC is not included).")
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

	CLI::add_option(app, p, naf,
		"--type,--poly",
		type,
		"Select the CRC type/polynomial you want to use (ex: \"8-DVB-S2\": 0xD5,"
		" \"16-IBM\": 0x8005, \"24-LTEA\": 0x864CFB, \"32-GZIP\": 0x04C11DB7).",
		true)
		->group("Standard");

	CLI::add_set(app, p, naf,
		"--implem",
		implem,
		{"STD", "FAST", "INTER"},
		"Select the CRC implementation you want to use.",
		true)
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--size",
		size,
		"Size of the CRC (divisor size in bit -1), required if you selected an unknown CRC.")
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");
}

void CRC::parameters
::callback_arguments()
{
	if (type != "NO" && !type.empty() && !size)
		size = module::CRC_polynomial<B>::get_size(type);
}

void CRC::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_name();

	if (type != "NO" && !type.empty())
	{
		auto poly_name = module::CRC_polynomial<B>::get_name(type);
		if (!poly_name.empty())
			headers[p].push_back(std::make_pair("Type", poly_name));
		else
		{
			std::stringstream poly_val;
			poly_val << "0x" << std::hex << module::CRC_polynomial<B>::get_value(type);
			headers[p].push_back(std::make_pair("Type", poly_val.str()));
		}
		std::stringstream poly_val;
		poly_val << "0x" << std::hex << module::CRC_polynomial<B>::get_value(type);
		headers[p].push_back(std::make_pair("Polynomial (hexadecimal)", poly_val.str()));

		auto poly_size = module::CRC_polynomial<B>::get_size(type);
		headers[p].push_back(std::make_pair("Size (in bit)", std::to_string(poly_size ? poly_size : size)));
	}
	else
		headers[p].push_back(std::make_pair("Type", "NO"));

	headers[p].push_back(std::make_pair("Implementation", implem));

	if (full) headers[p].push_back(std::make_pair("Info. bits (K)", std::to_string(K)));
	if (full) headers[p].push_back(std::make_pair("Inter frame level", std::to_string(n_frames)));
}

template <typename B>
module::CRC<B>* CRC::parameters
::build() const
{
	if (type != "NO" && !type.empty())
	{
		const auto poly = type;

		if (implem == "STD"  ) return new module::CRC_polynomial      <B>(K, poly, size, n_frames);
		if (implem == "FAST" ) return new module::CRC_polynomial_fast <B>(K, poly, size, n_frames);
		if (implem == "INTER") return new module::CRC_polynomial_inter<B>(K, poly, size, n_frames);
	}
	else                       return new module::CRC_NO              <B>(K,             n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B>
module::CRC<B>* CRC
::build(const parameters &params)
{
	return params.template build<B>();
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::CRC<B_8 >* aff3ct::factory::CRC::parameters::build<B_8 >() const;
template aff3ct::module::CRC<B_16>* aff3ct::factory::CRC::parameters::build<B_16>() const;
template aff3ct::module::CRC<B_32>* aff3ct::factory::CRC::parameters::build<B_32>() const;
template aff3ct::module::CRC<B_64>* aff3ct::factory::CRC::parameters::build<B_64>() const;
template aff3ct::module::CRC<B_8 >* aff3ct::factory::CRC::build<B_8 >(const aff3ct::factory::CRC::parameters&);
template aff3ct::module::CRC<B_16>* aff3ct::factory::CRC::build<B_16>(const aff3ct::factory::CRC::parameters&);
template aff3ct::module::CRC<B_32>* aff3ct::factory::CRC::build<B_32>(const aff3ct::factory::CRC::parameters&);
template aff3ct::module::CRC<B_64>* aff3ct::factory::CRC::build<B_64>(const aff3ct::factory::CRC::parameters&);
#else
template aff3ct::module::CRC<B>* aff3ct::factory::CRC::parameters::build<B>() const;
template aff3ct::module::CRC<B>* aff3ct::factory::CRC::build<B>(const aff3ct::factory::CRC::parameters&);
#endif
// ==================================================================================== explicit template instantiation
