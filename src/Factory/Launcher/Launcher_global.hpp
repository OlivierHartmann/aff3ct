#ifndef FACTORY_LAUNCHER_GLOBAL_HPP_
#define FACTORY_LAUNCHER_GLOBAL_HPP_

#include <string>

#include "../Factory.hpp"

namespace aff3ct
{
namespace factory
{
extern const std::string Launcher_global_name;
extern const std::string Launcher_global_prefix;
struct Launcher_global : public Factory
{
	class parameters : public Factory::parameters
	{
	public:
		// ------------------------------------------------------------------------------------------------- PARAMETERS
		// optional parameters
		bool        display_version  = false;
		bool        hide_legend      = false;
		bool        full_legend      = false;
		bool        disable_bt       = false;
		bool        enable_full_bt   = false;
		bool        disable_colors   = false;
		bool        help             = false;
		bool        advanced_help    = false;

		// ---------------------------------------------------------------------------------------------------- METHODS
		explicit parameters(const std::string &p = Launcher_global_prefix, const std::string &n = Launcher_global_name);
		virtual ~parameters() = default;
		virtual Launcher_global::parameters* clone() const;

		// parameters construction
		virtual void register_arguments(CLI::App &app);
		virtual void callback_arguments();
		virtual void get_headers(std::map<std::string,header_list>& headers, const bool full = true) const;

	protected:
		parameters(const std::string &n, const std::string &sn, const std::string &p);
	};
};
}
}

#endif /* FACTORY_LAUNCHER_GLOBAL_HPP_ */
