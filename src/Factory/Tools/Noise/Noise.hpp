#ifndef FACTORY_NOISE_HPP
#define FACTORY_NOISE_HPP

#include <string>
#include <vector>

#include "Tools/Noise/Noise.hpp"

#include "../../Factory.hpp"

namespace aff3ct
{
namespace factory
{
extern const std::string Noise_name;
extern const std::string Noise_prefix;
struct Noise : public Factory
{
	class parameters : public Factory::parameters
	{
	public:
		// ------------------------------------------------------------------------------------------------- PARAMETERS
		// required parameters
		CLI::Matlab_vector<float> range;

		// optional parameters
		std::string type     = "EBN0";
		std::string pdf_path = "";

		// ---------------------------------------------------------------------------------------------------- METHODS
		explicit parameters(const std::string &n = Noise_name, const std::string &p = Noise_prefix);
		virtual ~parameters() = default;
		Noise::parameters* clone() const;

		// parameters construction
		void register_arguments(CLI::App &app);
		void callback_arguments();
		void get_headers(std::map<std::string,header_list>& headers, const bool full = true) const;

		// builder
		template <typename R = float>
		tools::Noise<R>* build(R noise_val, R bit_rate = 1., int bps = 1, int upf = 1) const;

	private:
		float noise_min;
		float noise_max;
		float noise_step = 0.1f;
		CLI::Option* noise_min_option;
		CLI::Option* noise_max_option;
		CLI::Option* noise_step_option;
		CLI::Option* pdf_path_option;
	};

	template <typename R = float>
	static tools::Noise<R>* build(const aff3ct::factory::Noise::parameters& params, R noise_val, R bit_rate = 1., int bps = 1, int upf = 1);
};
}
}

#endif /* FACTORY_NOISE_HPP */
