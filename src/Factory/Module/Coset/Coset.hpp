#ifndef FACTORY_COSET_HPP
#define FACTORY_COSET_HPP

#include <string>

#include "Module/Coset/Coset.hpp"

#include "../../Factory.hpp"

namespace aff3ct
{
namespace factory
{
extern const std::string Coset_name;
extern const std::string Coset_prefix;
struct Coset : public Factory
{
	class parameters : public Factory::parameters
	{
	public:
		// ------------------------------------------------------------------------------------------------- PARAMETERS
		// required parameters
		unsigned    size     = 0;

		// optional parameters
		std::string type     = "STD";
		unsigned    n_frames = 1;

		// ---------------------------------------------------------------------------------------------------- METHODS
		explicit parameters(const std::string &p = Coset_prefix);
		virtual ~parameters() = default;
		Coset::parameters* clone() const;

		// parameters construction
		void register_arguments(CLI::App &app);
		void callback_arguments();
		void get_headers(std::map<std::string,header_list>& headers, const bool full = true) const;

		// builder
		template <typename B1 = int, typename B2 = B1>
		module::Coset<B1,B2>* build_bit() const;

		template <typename B = int, typename R = float>
		module::Coset<B,R>* build_real() const;
	};

	template <typename B1 = int, typename B2 = B1>
	static module::Coset<B1,B2>* build_bit(const parameters &params);

	template <typename B = int, typename R = float>
	static module::Coset<B,R>* build_real(const parameters &params);
};
}
}

#endif /* FACTORY_COSET_HPP */
