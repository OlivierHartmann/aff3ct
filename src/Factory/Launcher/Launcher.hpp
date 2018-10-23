#ifndef FACTORY_LAUNCHER_HPP_
#define FACTORY_LAUNCHER_HPP_

#include <string>
#include "Tools/auto_cloned_unique_ptr.hpp"

#include "../Factory.hpp"
#include "Launcher_global.hpp"

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
		tools::auto_cloned_unique_ptr<Launcher_global::parameters> glb;

		// required parameters
		std::string cde_type;

		// optional parameters
		std::string sim_type = "BFER";
		unsigned    sim_prec = 32;

		// ---------------------------------------------------------------------------------------------------- METHODS
		explicit parameters(const std::string &p = Launcher_prefix);
		parameters(const std::string &n, const std::string &p);
		virtual ~parameters() = default;
		virtual Launcher::parameters* clone() const;

		virtual std::vector<std::string> get_names      () const;
		virtual std::vector<std::string> get_short_names() const;
		virtual std::vector<std::string> get_prefixes   () const;

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

#endif /* FACTORY_LAUNCHER_HPP_ */
