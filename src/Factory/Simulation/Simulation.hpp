#ifndef FACTORY_SIMULATION_HPP
#define FACTORY_SIMULATION_HPP

#ifdef ENABLE_MPI
#include <mpi.h>
#endif
#include <chrono>
#include <string>
#include <sstream>
#include <vector>

#include "Factory/Launcher/Launcher.hpp"
#include "Factory/Tools/Noise/Noise.hpp"

namespace aff3ct
{
namespace factory
{
extern const std::string Simulation_name;
extern const std::string Simulation_prefix;
struct Simulation : Launcher
{
	class parameters : public Launcher::parameters
	{
	public:
		// ------------------------------------------------------------------------------------------------- PARAMETERS
		// required arg
		tools::auto_cloned_unique_ptr<Noise::parameters> noise;

		// optional parameters
		std::chrono::milliseconds mpi_comm_freq   = std::chrono::milliseconds(1000);
		int                       mpi_rank        = 0;
		int                       mpi_size        = 1;

		std::chrono::seconds      stop_time       = std::chrono::seconds(0);
		std::string               meta            = "";
		unsigned                  max_frame       = 0;
		bool                      debug           = false;
		bool                      debug_hex       = false;
		bool                      statistics      = false;
		bool                      crit_nostop     = false;
		unsigned                  n_threads       = 0;
		unsigned                  local_seed      = 0;
		unsigned                  global_seed     = 0;
		unsigned                  debug_limit     = 0;
		unsigned                  debug_precision = 2;


		// ---------------------------------------------------------------------------------------------------- METHODS
		virtual ~parameters() = default;
		virtual Simulation::parameters* clone() const;

		virtual std::vector<std::string> get_names      () const;
		virtual std::vector<std::string> get_short_names() const;
		virtual std::vector<std::string> get_prefixes   () const;

		// parameters construction
		virtual void register_arguments(CLI::App &app);
		virtual void callback_arguments();
		virtual void get_headers(std::map<std::string,header_list>& headers, const bool full = true) const;

	protected:
		parameters(const std::string &n = Simulation_name, const std::string &p = Simulation_prefix);
	};
};
}
}

#endif /* FACTORY_SIMULATION_HPP */
