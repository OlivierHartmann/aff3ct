/*!
 * \file
 * \brief Collects the command line parameters and launches the simulation.
 *
 * \section LICENSE
 * This file is under MIT license (https://opensource.org/licenses/MIT).
 */
#ifndef LAUNCHER_HPP_
#define LAUNCHER_HPP_

#include <map>
#include <string>
#include <memory>

#include "Tools/types.h"
#include "Factory/Simulation/Simulation.hpp"
#include "Simulation/Simulation.hpp"

namespace aff3ct
{
namespace launcher
{
/*!
 * \class Launcher
 *
 * \brief Collects the command line parameters and launches the simulation.
 *        Describes and provides tools to make a working launcher.
 */
class Launcher
{
private:
	std::unique_ptr<simulation::Simulation> simu; /*!< A generic simulation pointer to allocate a specific simulation. */
	std::string                      cmd_line;
	std::vector<std::string>         cmd_warn;

	int argc;
	char **argv;

	factory::Simulation::parameters *params_simu;  /*!< A structure of parameters to store and pass to the simulation. */

protected:
	std::ostream                    &stream;  /*!< The dedicated stream in which the Launcher writes the parameters. */

public:
	/*!
	 * \brief Constructor.
	 *
	 * Takes the famous "argc" and "argv" arguments from the main function.
	 *
	 * \param argc:   number of arguments.
	 * \param argv:   array of arguments
	 * \param stream: the stream in which the Launcher writes the parameters.
	 */
	Launcher(const int argc, const char **argv, std::ostream &stream = std::cout);

	/*!
	 * \brief Destructor.
	 *
	 * Deallocates the simulation.
	 */
	virtual ~Launcher() = default;

	/*!
	 * \brief Launch the simulation.
	 */
	int launch();

protected:
	/*!
	 * \brief Builds the various arguments required (or/and optional) for the simulation.
	 *
	 * This method can be overloaded to be extended.
	 */
	virtual void register_arguments(CLI::App &app);


	/*!
	 * \brief Post command line parse process on arguments if needed
	 *
	 * This method can be overloaded to be extended.
	 */
	virtual void callback_arguments();

	/*!
	 * \brief Allocates a specific simulation.
	 *
	 * This method have to be overloaded.
	 *
	 * \return a new simulation.
	 */
	virtual simulation::Simulation* build_simu() = 0;

	void print_header();

	void set_params(factory::Simulation::parameters *params_simu);
private:
	int generate_arguments();
	int print_help(bool help, bool advanced);
};
}
}

#endif /* LAUNCHER_HPP_ */
