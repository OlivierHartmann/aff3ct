#include <fstream>
#include <algorithm>
#include <rang.hpp>

#include "Tools/Exception/exception.hpp"

#include "Simulation.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Simulation_name   = "Simulation";
const std::string aff3ct::factory::Simulation_prefix = "sim";

Simulation::parameters
::parameters(const std::string &name, const std::string &prefix)
: Launcher::parameters(name, Simulation_name, prefix),
  noise(new factory::Noise::parameters(name, prefix))
{
}

Simulation::parameters* Simulation::parameters
::clone() const
{
	return new Simulation::parameters(*this);
}

std::vector<std::string> Simulation::parameters
::get_names() const
{
	auto n = Launcher::parameters::get_names();
	if (noise != nullptr) { auto nn = noise->get_names(); for (auto &x : nn) n.push_back(x); }
	return n;
}

std::vector<std::string> Simulation::parameters
::get_short_names() const
{
	auto sn = Launcher::parameters::get_short_names();
	if (noise != nullptr) { auto nn = noise->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	return sn;
}

std::vector<std::string> Simulation::parameters
::get_prefixes() const
{
	auto p = Launcher::parameters::get_prefixes();
	if (noise != nullptr) { auto nn = noise->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	return p;
}

void Simulation::parameters
::register_arguments(CLI::App &app)
{
	auto p   = get_prefix();
	auto naf = no_argflag();

	Launcher::parameters::register_arguments(app);

	noise->register_arguments(app);

	CLI::add_option(app, p, naf,
		"--meta",
		meta,
		"Print the output with metadata, takes the simulation title.")
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--stop-time",
		stop_time,
		"Time in sec after what the current simulatated noise stops (0 is infinite).")
		->group("Advanced");

	CLI::add_option(app, p, naf,
		"--max-frame",
		max_frame,
		"Maximum number of frames to play after what the current simulatated noise stops (0 is infinite).")
		->group("Advanced");

	CLI::add_flag(app, p, naf,
		"--crit-nostop",
		crit_nostop,
		"The stop criteria arguments -stop-time or -max-frame kill the current simulatated noise point"
		" but not the simulation.")
		->group("Advanced");



	CLI::add_flag(app, p, naf,
		"--debug",
		debug,
		"Enable debug mode: print array values after each step.")
		->group("Standard");

	CLI::add_flag(app, p, naf,
		"--debug-hex",
		debug_hex,
		"Enable debug mode: prints values in the hexadecimal format.")
		->group("Standard");

	CLI::add_option(app, p, naf,
		"-d,--debug-limit",
		debug_limit,
		"Enable debug mode: set the max number of elements to display in the debug mode (0 is all).")
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	CLI::add_option(app, p, naf,
		"--debug-prec",
		debug_precision,
		"Set the precision of real elements when displayed in debug mode.",
		true)
		->check(CLI::Range(10))
		->group("Standard");


	CLI::add_flag(app, p, naf,
		"--stats",
		statistics,
		"Display statistics module by module.")
		->group("Standard");



	CLI::add_option(app, p, naf,
		"-t,--threads",
		n_threads,
		"Enable multi-threaded mode and specify the number of threads (0 means the maximum supported by the core).",
		true)
		->group("Standard");

	CLI::add_option(app, p, naf,
		"-S,--seed",
		global_seed,
		"Seed used in the simulation to initialize the pseudo random generators in general.",
		true)
		->group("Standard");

#ifdef ENABLE_MPI
	CLI::add_option(app, p, naf,
		"--mpi-comm",
		mpi_comm_freq,
		"MPI communication frequency between the nodes (in millisec).")
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Advanced");
#endif
}

void Simulation::parameters
::callback_arguments()
{
	using namespace std::chrono;

	Launcher::parameters::callback_arguments();

	noise->callback_arguments();

	debug = debug || debug_limit || debug_hex;
	if (debug)
		n_threads = 1;


#ifdef ENABLE_MPI
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

	int max_n_threads_global;
	int max_n_threads_local = n_threads;

	MPI_Allreduce(&max_n_threads_local, &max_n_threads_global, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

	if (max_n_threads_global <= 0)
	{
		std::stringstream message;
		message << "'max_n_threads_global' has to be greater than 0 ('max_n_threads_global' = "
		        << max_n_threads_global << ").";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	// ensure that all the MPI processes have a different seed (crucial for the Monte-Carlo method)
	local_seed = global_seed + max_n_threads_global * mpi_rank;
#else
	local_seed = global_seed;
#endif
}

void Simulation::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_name();

	Launcher::parameters::get_headers(headers, full);

	noise->get_headers(headers, full);

	headers[p].push_back(std::make_pair("Seed", std::to_string(global_seed)));
	headers[p].push_back(std::make_pair("Statistics", statistics ? "on" : "off"));
	headers[p].push_back(std::make_pair("Debug mode", debug ? "on" : "off"));
	if (debug)
	{
		headers[p].push_back(std::make_pair("Debug precision", std::to_string(debug_precision)));
		if (debug_limit)
			headers[p].push_back(std::make_pair("Debug limit", std::to_string(debug_limit)));
	}

#ifdef ENABLE_MPI
	headers[p].push_back(std::make_pair("MPI comm. freq. (ms)", std::to_string(mpi_comm_freq.count())));
	headers[p].push_back(std::make_pair("MPI size",             std::to_string(mpi_size             )));
#endif

	std::string threads = "unused";
	if (n_threads)
		threads = std::to_string(n_threads) + " thread(s)";

	headers[p].push_back(std::make_pair("Multi-threading (t)", threads));
}
