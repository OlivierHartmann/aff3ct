#ifndef CLI11_TOOLS_HPP
#define CLI11_TOOLS_HPP

#include <sstream>
#include <string>
#include <chrono>
#include <vector>

namespace CLI
{

std::istringstream &operator>>(std::istringstream &in, std::chrono::     seconds &val);
std::istringstream &operator>>(std::istringstream &in, std::chrono::milliseconds &val);

}

#include <CLI/CLI.hpp>
#include "Validators.hpp"

namespace CLI
{

/**
 * \brief Return the subcommand in app if exists else create it.
 * \param app is the application in which the subcommand should be
 * \param command is the subcommand name
 * \param description is the subcommand description in the case it needs to be created
 * \return pointer to the subcommand
 */
CLI::App* make_subcommand(CLI::App& app, const std::string& command, const std::string& description);

/**
 * \brief Check if the subcommand is in the application
 * \param app is the application in which the subcommand should be
 * \param command is the subcommand name
 * \return true if the subcommand has been found
 */
bool has_subcommand(CLI::App& app, const std::string& command);
bool has_subcommand(CLI::App* app, const std::string& command);

/**
 * \brief Check if the option is in the application
 * \param app is the application in which the option should be
 * \param option is the option name
 * \return true if the option has been found
 */
bool has_option(CLI::App& app, const std::string& option);
bool has_option(CLI::App* app, const std::string& option);


/**
 * \brief Remove the option in the application
 * \param app is the application in which the option should be
 * \param option is the option name
 */
void remove_option(CLI::App& app, const std::string& option);
void remove_option(CLI::App* app, const std::string& option);
}

#endif // CLI11_TOOLS_HPP
