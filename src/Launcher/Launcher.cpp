#include <rang.hpp>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <regex>
#include <date.h>

#include "Tools/Display/Terminal/Terminal.hpp"

#ifdef ENABLE_MPI
#include <mpi.h>
#endif

#include "Tools/general_utils.h"
#include "Tools/system_functions.h"
#include "Tools/Display/rang_format/rang_format.h"
#include "Tools/Exception/exception.hpp"

#include "Factory/Module/Source/Source.hpp"
#include "Factory/Module/CRC/CRC.hpp"
#include "Factory/Module/Encoder/Encoder.hpp"
#include "Factory/Module/Puncturer/Puncturer.hpp"
#include "Factory/Module/Interleaver/Interleaver.hpp"
#include "Factory/Module/Modem/Modem.hpp"
#include "Factory/Module/Channel/Channel.hpp"
#include "Factory/Module/Quantizer/Quantizer.hpp"
#include "Factory/Module/Decoder/Decoder.hpp"
#include "Factory/Module/Monitor/Monitor.hpp"
#include "Factory/Tools/Display/Terminal/Terminal.hpp"

#include "Launcher.hpp"

using namespace aff3ct;
using namespace aff3ct::launcher;

Launcher
::Launcher(const int argc, const char **argv, std::ostream &stream)
: simu(nullptr), argc(argc), argv((char**)argv), params_simu(nullptr), ah(argc, argv), stream(stream)
{
	cmd_line += std::string(argv[0]) + std::string(" ");
	for (auto i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
			cmd_line += std::string(argv[i]);
		else
			cmd_line += std::string("\"") + std::string(argv[i]) + std::string("\"");

		cmd_line += std::string(" ");
	}
}

void Launcher
::set_params(factory::Simulation::parameters *params)
{
	params_simu = params;
}

void Launcher
::register_arguments(CLI::App &app)
{
}

void Launcher
::callback_arguments()
{
}

int Launcher
::print_help(bool help, bool advanced)
{
	if (advanced)
	{
		auto app = factory::Factory::make_argument_handler();

		this->register_arguments(*app);

		app->exit(CLI::CallForAllHelp());
		return EXIT_FAILURE;
	}
	else if (help)
	{
		auto app = factory::Factory::make_argument_handler();

		this->register_arguments(*app);

		app->exit(CLI::CallForHelp());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int Launcher
::generate_arguments()
{
	auto app = factory::Factory::make_argument_handler();

	this->register_arguments(*app);

	try
	{
		app->parse(argc, argv);
	}
	catch (const CLI::ParseError &e)
	{
		std::cerr << "PARSING ERROR" << std::endl;
		if (params_simu->mpi_rank == 0)
		{
			auto app2 = factory::Factory::make_argument_handler();
			// add the help flags to shorcut required arguments or links if help is asked
			app2->set_help_flag("-h,--help", "Print the help.");
			app2->set_help_all_flag("-H,--Help", "Print this help with the advanced arguments.");

			this->register_arguments(*app2);

			try
			{
				app2->parse(argc, argv);
			}
			catch (const CLI::CallForAllHelp &e)
			{
				print_help(false, true);
			}
			catch (const CLI::CallForHelp &e)
			{
				print_help(true, false);
			}
			catch (const CLI::ParseError &e)
			{
			}
			std::cerr << rang::tag::error << e.what() << std::endl;
			// return EXIT_FAILURE;
		}
		return EXIT_FAILURE;
	}

	if (print_help(params_simu->help, params_simu->advanced_help) == EXIT_FAILURE)
		return EXIT_FAILURE;

	this->callback_arguments();

	for (auto& name : app->remaining(true))
		std::cerr << rang::tag::warning << name << std::endl;

	return EXIT_SUCCESS;
}

void Launcher
::print_header()
{
	// display configuration and simulation parameters
	stream << rang::tag::comment << rang::style::bold << "----------------------------------------------------" << std::endl;
	stream << rang::tag::comment << rang::style::bold << "---- A FAST FORWARD ERROR CORRECTION TOOLBOX >> ----" << std::endl;
	stream << rang::tag::comment << rang::style::bold << "----------------------------------------------------" << std::endl;
	stream << rang::tag::comment << rang::style::bold << rang::style::underline << "Parameters :"<< rang::style::reset << std::endl;
	factory::Header::print_parameters({params_simu}, params_simu->full_legend, this->stream);
	this->stream << rang::tag::comment << std::endl;
}

std::string remove_argument(const std::string& cmd, std::string arg)
{
#if !defined(__clang__) && !defined(__llvm__) && defined(__GNUC__) && defined(__cplusplus) && __GNUC__ < 5
	if (arg.front() != ' ')
		arg = " " + arg;
	auto pos_arg = cmd.find(arg);

	if (pos_arg == std::string::npos)
		return cmd;

	auto pos_start = cmd.find("\"", pos_arg + arg.size());
	auto pos_end   = cmd.find("\"", pos_start + 1);

	return cmd.substr(0, pos_arg) + cmd.substr(pos_end + 1);
#else
	return std::regex_replace(cmd, std::regex("( " + arg + " \"[^\"]*\")"), "");
#endif
}

std::string remove_argument(std::string cmd, const std::vector<std::string>& args)
{
	for (auto& a : args)
		cmd = remove_argument(cmd, a);
	return cmd;
}

int Launcher
::launch()
{
	if (params_simu == nullptr)
		tools::runtime_error(__FILE__, __LINE__, __func__, "'params_simu' is a nullptr.");

	int exit_code = EXIT_SUCCESS;

	std::srand((unsigned)params_simu->global_seed);

	if (this->generate_arguments() == EXIT_FAILURE)
		return EXIT_FAILURE;

	// write the metadata: command and the curve name for PyBER
	if (params_simu->mpi_rank == 0)
		if (!params_simu->meta.empty())
		{
			stream << "[metadata]" << std::endl;
			stream << "command=" << remove_argument(cmd_line, {"--sim-meta", "-t", "--ter-freq"}) << std::endl;
			stream << "title=" << params_simu->meta << std::endl;
			stream << std::endl << "[trace]" << std::endl;
		}

	if (!params_simu->hide_legend)
		if (params_simu->mpi_rank == 0)
			this->print_header();

	try
	{
		simu.reset(this->build_simu());
	}
	catch(const std::exception& e)
	{
		rang::format_on_each_line(std::cerr, std::string(e.what()) + "\n", rang::tag::error);
		exit_code = EXIT_FAILURE;
	}

	if (simu != nullptr)
	{
		// launch the simulation
		if (!params_simu->hide_legend)
			if (params_simu->mpi_rank == 0)
				stream << rang::tag::comment << "The simulation is running..." << std::endl;

		try
		{
			simu->launch();
			if (simu->is_error())
				exit_code = EXIT_FAILURE;
		}
		catch(const std::exception& e)
		{
			rang::format_on_each_line(std::cerr, std::string(e.what()) + "\n", rang::tag::error);
			exit_code = EXIT_FAILURE;
		}
	}

	if (!params_simu->hide_legend)
		if (params_simu->mpi_rank == 0)
			stream << rang::tag::comment << "End of the simulation." << std::endl;

	return exit_code;
}
