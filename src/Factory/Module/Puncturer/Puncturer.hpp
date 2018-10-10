#ifndef FACTORY_PONCTURER_HPP_
#define FACTORY_PONCTURER_HPP_

#include <string>
#include <set>

#include "Module/Puncturer/Puncturer.hpp"

#include "../../Factory.hpp"

namespace aff3ct
{
namespace factory
{
extern const std::string Puncturer_name;
extern const std::string Puncturer_prefix;
struct Puncturer : Factory
{
	class parameters : public Factory::parameters
	{
	public:
		// ------------------------------------------------------------------------------------------------- PARAMETERS
		// required parameters
		unsigned    K        = 0;
		unsigned    N        = 0;

		// optional parameters
		std::string type     = "NO";
		unsigned    N_cw     = 0;
		unsigned    n_frames = 1;

		std::set<std::string> type_set = {"NO"};
		// ---------------------------------------------------------------------------------------------------- METHODS
		explicit parameters(const std::string &p = Puncturer_prefix);
		virtual ~parameters() = default;
		virtual Puncturer::parameters* clone() const;

		// parameters construction
		virtual void register_arguments(CLI::App &app);
		virtual void callback_arguments();
		virtual void get_headers(std::map<std::string,header_list>& headers, const bool full = true) const;

		// builder
		template <typename B, typename Q>
		module::Puncturer<B,Q>* build() const;

	protected:
		parameters(const std::string &n, const std::string &p);
	};

	template <typename B, typename Q>
	static module::Puncturer<B,Q>* build(const parameters &params);
};
}
}

#endif /* FACTORY_PONCTURER_HPP_ */
