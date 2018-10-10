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
  noise(new factory::Noise::parameters(prefix))
{
}

Simulation::parameters* Simulation::parameters
::clone() const
{
	return new Simulation::parameters(*this);
}

void Simulation::parameters
::register_arguments(CLI::App &app)
{
	Launcher::parameters::register_arguments(app);

	noise->register_arguments(app);

	auto sub = CLI::make_subcommand(app, get_prefix(), get_name() + " parameters");

	sub->add_option(
		"--meta",
		meta,
		"Print the output with metadata, takes the simulation title.")
		->group("Standard");

	sub->add_option(
		"--stop-time",
		stop_time,
		"Time in sec after what the current simulatated noise stops (0 is infinite).")
		->group("Advanced");

	sub->add_option(
		"--max-frame",
		max_frame,
		"Maximum number of frames to play after what the current simulatated noise stops (0 is infinite).")
		->group("Advanced");

	sub->add_flag(
		"--crit-nostop",
		crit_nostop,
		"The stop criteria arguments -stop-time or -max-frame kill the current simulatated noise point"
		" but not the simulation.")
		->group("Advanced");



	sub->add_flag(
		"--debug",
		debug,
		"Enable debug mode: print array values after each step.")
		->group("Standard");

	sub->add_flag(
		"--debug-hex",
		debug_hex,
		"Enable debug mode: prints values in the hexadecimal format.")
		->group("Standard");

	sub->add_option(
		"-d,--debug-limit",
		debug_limit,
		"Enable debug mode: set the max number of elements to display in the debug mode (0 is all).")
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	sub->add_option(
		"--debug-prec",
		debug_precision,
		"Set the precision of real elements when displayed in debug mode.",
		true)
		->check(CLI::Range(10))
		->group("Standard");


	sub->add_flag(
		"--stats",
		statistics,
		"Display statistics module by module.")
		->group("Standard");



	sub->add_option(
		"-t,--threads",
		n_threads,
		"Enable multi-threaded mode and specify the number of threads (0 means the maximum supported by the core).",
		true)
		->group("Standard");

	sub->add_option(
		"-S,--seed",
		global_seed,
		"Seed used in the simulation to initialize the pseudo random generators in general.",
		true)
		->group("Standard");

#ifdef ENABLE_MPI
	sub->add_option(
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
	MPI_Comm_size(MPI_COMM_WORLD, &this->mpi_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &this->mpi_rank);

	int max_n_threads_global;
	int max_n_threads_local = this->n_threads;

	MPI_Allreduce(&max_n_threads_local, &max_n_threads_global, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

	if (max_n_threads_global <= 0)
	{
		std::stringstream message;
		message << "'max_n_threads_global' has to be greater than 0 ('max_n_threads_global' = "
		        << max_n_threads_global << ").";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	// ensure that all the MPI processes have a different seed (crucial for the Monte-Carlo method)
	this->local_seed = this->global_seed + max_n_threads_global * this->mpi_rank;
#else
	this->local_seed = this->global_seed;
#endif
}

void Simulation::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	Launcher::parameters::get_headers(headers);

	noise->get_headers(headers, full);

	auto p = this->get_prefix();

	headers[p].push_back(std::make_pair("Seed", std::to_string(this->global_seed)));
	headers[p].push_back(std::make_pair("Statistics", this->statistics ? "on" : "off"));
	headers[p].push_back(std::make_pair("Debug mode", this->debug ? "on" : "off"));
	if (this->debug)
	{
		headers[p].push_back(std::make_pair("Debug precision", std::to_string(this->debug_precision)));
		if (this->debug_limit)
			headers[p].push_back(std::make_pair("Debug limit", std::to_string(this->debug_limit)));
	}

#ifdef ENABLE_MPI
	headers[p].push_back(std::make_pair("MPI comm. freq. (ms)", std::to_string(this->mpi_comm_freq.count())));
	headers[p].push_back(std::make_pair("MPI size",             std::to_string(this->mpi_size             )));
#endif

	std::string threads = "unused";
	if (this->n_threads)
		threads = std::to_string(this->n_threads) + " thread(s)";

	headers[p].push_back(std::make_pair("Multi-threading (t)", threads));
}
