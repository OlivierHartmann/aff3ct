#include <sstream>

#include "Tools/Exception/exception.hpp"
#include "Tools/Math/Distribution/Distributions.hpp"
#include "Tools/general_utils.h"
#include "Tools/Noise/noise_utils.h"

#include "Noise.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Noise_name   = "Noise";
const std::string aff3ct::factory::Noise_prefix = "sim";

Noise::parameters
::parameters(const std::string &name, const std::string &prefix)
: Factory::parameters(name, name, prefix)
{
}

Noise::parameters* Noise::parameters
::clone() const
{
	return new Noise::parameters(*this);
}

void Noise::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	auto noise_range_option =
	CLI::add_option(app, p, naf,
		"-R,--noise-range",
		range,
		"Noise energy range to run (Matlab style: \"0.5:2.5,2.55,2.6:0.05:3\" with a default step of 0.1).")
		// ->required()
		->group("Standard");

	noise_min_option =
	CLI::add_option(app, p, naf,
		"-m,--noise-min",
		noise_min,
		"Minimal noise energy to simulate.")
		// ->required()
		->group("Standard");

	noise_max_option =
	CLI::add_option(app, p, naf,
		"-M,--noise-max",
		noise_max,
		"Maximal noise energy to simulate.")
		// ->required()
		->group("Standard");

	noise_step_option =
	CLI::add_option(app, p, naf,
		"-s,--noise-step",
		noise_step,
		"Noise energy step between each simulation iteration.",
		true)
		->check(CLI::StrictlyPositiveRange((float)0))
		->group("Standard");

	pdf_path_option =
	CLI::add_option(app, p, naf,
		"--pdf-path",
		pdf_path,
		"A file that contains PDF for different SNR. Set the SNR range from the given ones."
		" Overwritten by -R or limited by -m and -M with a minimum step of -s")
		->check(CLI::ExistingFile)
		->group("Standard");


	noise_range_option->excludes(noise_min_option );
	noise_range_option->excludes(noise_max_option );
	noise_range_option->excludes(noise_step_option);

	CLI::add_set(app, p, naf,
		"-E,--noise-type",
		type,
		{"ESN0", "EBN0", "ROP", "EP"},
		"Select the type of NOISE: SNR per Symbol / SNR per information Bit"
		" / Received Optical Power / Erasure Probability.",
		true)
		->group("Standard");
}

void Noise::parameters
::callback_arguments()
{
	if (!pdf_path_option->empty())
	{
		if (range.empty())
		{
			range = tools::Distributions<>(pdf_path).get_noise_range();
		}
		else
		{
			if (!noise_min_option->empty())
			{
				auto it = std::lower_bound(range.begin(), range.end(), noise_min);
				range.erase(range.begin(), it);
			}

			if (!noise_max_option->empty())
			{
				auto it = std::upper_bound(range.begin(), range.end(), noise_max);
				range.erase(it, range.end());
			}

			if (!noise_step_option->empty())
			{
				auto it = range.begin();
				float start_val = *it++;

				while (it != range.end())
				{
					if ((start_val + noise_step) > *it) // then original step is too short
						it = range.erase(it);
					else
						start_val = *it++; // step large enough, take this new val as new comparative point
				}
			}
		}
	}
	else if (range.empty() && !noise_min_option->empty() && !noise_max_option->empty())
	{
		range = tools::generate_range({{noise_min, noise_max}}, noise_step);
	}
}

void Noise::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_name();

	if (!range.empty())
	{
		std::stringstream range_str;
		range_str << *range.begin() << " -> " << *range.rbegin() << " dB";
		headers[p].push_back(std::make_pair("Noise range", range_str.str()));
	}

	headers[p].push_back(std::make_pair("Noise type (E)", type));

	if (!pdf_path.empty())
		headers[p].push_back(std::make_pair("PDF path", pdf_path));
}

template <typename R>
tools::Noise<R>* Noise::parameters
::build(R noise_val, R bit_rate, int bps, int upf) const
{
	if (type == "EBN0" || type == "ESN0")
	{
		R esn0, ebn0;
		if (type == "EBN0")
		{
			ebn0 = noise_val;
			esn0 = tools::ebn0_to_esn0(ebn0, bit_rate, bps);
		}
		else // (type == "ESN0")
		{
			esn0 = noise_val;
			ebn0 = tools::esn0_to_ebn0(esn0, bit_rate, bps);
		}

		auto sigma = tools::esn0_to_sigma(esn0, upf);

		return new tools::Sigma<R>(sigma, ebn0, esn0);
	}

	if (type == "ROP") return new tools::Received_optical_power<R>(noise_val);
	if (type == "EP" ) return new tools::Event_probability     <R>(noise_val);

	std::stringstream message;
	message << "Unknown noise type ('noise_type' = " << type << ").";
	throw tools::cannot_allocate(__FILE__, __LINE__, __func__, message.str());
}

template <typename R>
tools::Noise<R>* Noise
::build(const aff3ct::factory::Noise::parameters& params, R noise_val, R bit_rate, int bps, int upf)
{
	return params.template build<R>(noise_val, bit_rate, bps, upf);
}

// ==================================================================================== explicit template instantiation
template aff3ct::tools::Noise<float>* aff3ct::factory::Noise::parameters::build<float>(float, float, int, int) const;
template aff3ct::tools::Noise<float>* aff3ct::factory::Noise::build<float>(const aff3ct::factory::Noise::parameters&, float, float, int, int);

template aff3ct::tools::Noise<double>* aff3ct::factory::Noise::parameters::build<double>(double, double, int, int) const;
template aff3ct::tools::Noise<double>* aff3ct::factory::Noise::build<double>(const aff3ct::factory::Noise::parameters&, double, double, int, int);
// ==================================================================================== explicit template instantiation
