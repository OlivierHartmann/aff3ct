#ifndef CLI11_TOOLS_HPP
#define CLI11_TOOLS_HPP

#include <string>
#include <vector>
#include <set>


#include "Type_chrono.hpp"
#include "Matlab_vector.hpp"

#include <CLI/CLI.hpp>

#include "Type_chrono_details.hpp"
#include "Matlab_vector_details.hpp"

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
 * \param name is the option name
 * \param prefix is the prefix to add to long names only
 * \return true if the option has been found
 */
bool has_option(CLI::App& app, const std::string& name, const std::string& prefix, bool no_argflag);
bool has_option(CLI::App* app, const std::string& name, const std::string& prefix, bool no_argflag);


/**
 * \brief Return the option that is in the application
 * \param app is the application in which the option should be
 * \param name is the option name
 * \param prefix is the prefix to add to long names only
 * \return pointer to the option
 */
CLI::Option* get_option(CLI::App& app, const std::string& name, const std::string& prefix, bool no_argflag);
CLI::Option* get_option(CLI::App* app, const std::string& name, const std::string& prefix, bool no_argflag);


/**
 * \brief Remove the option in the application
 * \param app is the application in which the option should be
 * \param name is the option name
 * \param prefix is the prefix to add to long names only
 */
void remove_option(CLI::App& app, const std::string& name, const std::string& prefix, bool no_argflag);
void remove_option(CLI::App* app, const std::string& name, const std::string& prefix, bool no_argflag);

/**
 * \brief Add the option in the application if is not present already
 * \param app is the application in which the option should be
 * \param prefix is the prefix to add to long names only
 * \param no_argflag when true, keep the option name without the flags but only the long tags
 * \param name is the option name (one or a list of tags)
 * \param variable is a reference to the variable in which the value will be stocked
 * \param description is the description displayed in the help
 * \param defaulted when true the default value is displayed in the help
 * \return pointer to the option
 */
template <typename T>
CLI::Option* add_option(CLI::App& app, const std::string& prefix, bool no_argflag, const std::string& name,
                        T& variable, const std::string& description, bool defaulted);
template <typename T>
CLI::Option* add_option(CLI::App& app, const std::string& prefix, bool no_argflag, const std::string& name,
                        T& variable, const std::string& description);

CLI::Option* add_option(CLI::App& app, const std::string& prefix, bool no_argflag, const std::string& name,
                        std::vector<std::vector<bool>>& variable, const std::string& description, bool defaulted);

CLI::Option* add_option(CLI::App& app, const std::string& prefix, bool no_argflag, const std::string& name,
                        std::vector<std::vector<bool>>& variable, const std::string& description);

/**
 * \brief Add the flag in the application if is not present already
 * \param app is the application in which the option should be
 * \param prefix is the prefix to add to long names only
 * \param no_argflag when true, keep the option name without the flags but only the long tags
 * \param name is the flag name (one or a list of tags)
 * \param variable is a reference to the variable in which the value will be stocked
 * \param description is the description displayed in the help
 * \param defaulted when true the default value is displayed in the help
 * \return pointer to the option
 */
template <typename T>
CLI::Option* add_flag(CLI::App& app, const std::string& prefix, bool no_argflag, const std::string& name,
                      T& variable, const std::string& description);

/**
 * \brief Add the set in the application if is not present already
 * \param app is the application in which the option should be
 * \param prefix is the prefix to add to long names only
 * \param no_argflag when true, keep the option name without the flags but only the long tags
 * \param name is the set name (one or a list of tags)
 * \param variable is a reference to the variable in which the value will be stocked
 * \param description is the description displayed in the help
 * \param defaulted when true the default value is displayed in the help
 * \return pointer to the option
 */
template <typename T>
CLI::Option* add_set(CLI::App& app, const std::string& prefix, bool no_argflag, const std::string& name,
                     T& variable, const std::set<T> &options, const std::string& description, bool defaulted);
template <typename T>
CLI::Option* add_set(CLI::App& app, const std::string& prefix, bool no_argflag, const std::string& name,
                     T& variable, const std::set<T> &options, const std::string& description);

template <typename T>
CLI::Option* add_set(CLI::App& app, const std::string& prefix, bool no_argflag, const std::string& name,
                     T& variable, const std::set<T> &&options, const std::string& description, bool defaulted);
template <typename T>
CLI::Option* add_set(CLI::App& app, const std::string& prefix, bool no_argflag, const std::string& name,
                     T& variable, const std::set<T> &&options, const std::string& description);

/**
 * \brief From a tag list, add the given prefix to long tags only and remove eventually short flags
 * \param name is the option name
 * \param prefix is the prefix to add
 * \param no_argflag when true, keep the option name without the flags but only the long tags
 *        except if there only short flags
 * \return a new name list with prefix added
 */
std::string add_prefix_to_name(const std::string& name, const std::string& prefix, bool no_argflag);
}

#include "CLI11_tools.hxx"

#endif // CLI11_TOOLS_HPP
