#ifndef FACTORY_MONITOR_MI_HPP
#define FACTORY_MONITOR_MI_HPP

#include <string>

#include "Module/Monitor/MI/Monitor_MI.hpp"

#include "../Monitor.hpp"

namespace aff3ct
{
namespace factory
{
extern const std::string Monitor_MI_name;
extern const std::string Monitor_MI_prefix;
struct Monitor_MI : public Monitor
{
	class parameters : public Monitor::parameters
	{
	public:
		// ------------------------------------------------------------------------------------------------- PARAMETERS
		// required parameters
		unsigned    N = 0;

		// optional parameters
		std::string type = "STD";
		unsigned    n_trials = 200;
		unsigned    n_frames = 1;

		// ---------------------------------------------------------------------------------------------------- METHODS
		explicit parameters(const std::string &p = Monitor_MI_prefix);
		virtual ~parameters() = default;
		Monitor_MI::parameters* clone() const;

		// parameters construction
		void register_arguments(CLI::App &app);
		void callback_arguments();
		void get_headers(std::map<std::string,header_list>& headers, const bool full = true) const;

		// builder
		template <typename B = int, typename R = float>
		module::Monitor_MI<B,R>* build() const;
	};

	template <typename B = int, typename R = float>
	static module::Monitor_MI<B,R>* build(const parameters& params);
};
}
}

#endif /* FACTORY_MONITOR_MI_HPP */
