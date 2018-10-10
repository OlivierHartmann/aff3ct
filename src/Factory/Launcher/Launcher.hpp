#ifndef FACTORY_SIMULATION_MAIN_H_
#define FACTORY_SIMULATION_MAIN_H_

#include <string>

#include "../Factory.hpp"

namespace aff3ct
{
namespace launcher
{
class Launcher;
}
}

namespace aff3ct
{
namespace factory
{
extern const std::string Launcher_name;
extern const std::string Launcher_prefix;
struct Launcher : public Factory
{
	class parameters : public Factory::parameters
	{
	public:
		// ------------------------------------------------------------------------------------------------- PARAMETERS
		// required parameters
		std::string cde_type;

		// optional parameters
		std::string sim_type         = "BFER";
		unsigned    sim_prec         = 32;
		bool        display_version  = false;
		bool        hide_legend      = false;
		bool        full_legend      = false;
		bool        disable_bt       = false;
		bool        enable_full_bt   = false;
		bool        disable_colors   = false;
		bool        help             = false;
		bool        advanced_help    = false;

		// ---------------------------------------------------------------------------------------------------- METHODS
		explicit parameters(const std::string &p = Launcher_prefix, const std::string &n = Launcher_name);
		virtual ~parameters() = default;
		virtual Launcher::parameters* clone() const;

		// parameters construction
		virtual void register_arguments(CLI::App &app);
		virtual void callback_arguments();
		virtual void get_headers(std::map<std::string,header_list>& headers, const bool full = true) const;

		// builder
		template <typename B = int, typename R = float, typename Q = R>
		launcher::Launcher* build(const int argc, const char **argv) const;

	private:
		template <typename B = int, typename R = float, typename Q = R>
		inline launcher::Launcher* build_exit(const int argc, const char **argv) const;

	protected:
		parameters(const std::string &n, const std::string &sn, const std::string &p);
	};

	template <typename B = int, typename R = float, typename Q = R>
	static launcher::Launcher* build(const parameters &params, const int argc, const char **argv);
};
}
}

#endif /* FACTORY_SIMULATION_MAIN_H_ */
