/*!
 * \file
 * \brief Class builder.
 *
 * \section LICENSE
 * This file is under MIT license (https://opensource.org/licenses/MIT).
 */
#ifndef FACTORY_HPP
#define FACTORY_HPP

#include <iostream>
#include <vector>
#include <map>

#include "Tools/Arguments/CLI11_tools.hpp"
#include "Tools/Arguments/Argument_tools.hpp"

namespace aff3ct
{
namespace factory
{
using header_list = std::vector<std::pair<std::string,std::string>>;

extern const std::string Factory_name;
extern const std::string Factory_short_name;
extern const std::string Factory_prefix;

/*!
 * \class Factory
 *
 * \brief Class builder.
 *
 * A factory is a structure dedicated to the object creation. The common usage is to call the build method which returns
 * a new allocated object.
 */
struct Factory
{
	class parameters
	{
	public:
		tools::Argument_map_info  args;
		tools::Argument_map_value vals;
		// constructor/destructor
		parameters(const std::string &name       = Factory_name,
		           const std::string &short_name = Factory_short_name,
		           const std::string &prefix     = Factory_prefix);
		virtual ~parameters() = default;
		virtual Factory::parameters* clone() const = 0;

		        std::string              get_name       () const;
		        std::string              get_short_name () const;
		        std::string              get_prefix     () const;

		virtual std::vector<std::string> get_names      () const;
		virtual std::vector<std::string> get_short_names() const;
		virtual std::vector<std::string> get_prefixes   () const;

		virtual void register_arguments(CLI::App &app) = 0;
		virtual void callback_arguments() = 0;

		virtual void get_headers(std::map<std::string,header_list>& headers, const bool full = true) const = 0;

		void no_argflag(bool v) ;
		bool no_argflag() const;

	private:
		const std::string name;
		const std::string short_name;
		const std::string prefix;
		bool no_argflag_ = false;
	};

	static std::unique_ptr<CLI::App> make_argument_handler();

	static void register_arguments(const std::vector<Factory::parameters*> &params, CLI::App &app);
	static void callback_arguments(const std::vector<Factory::parameters*> &params);

	static tools::Argument_map_group create_groups(const std::vector<Factory::parameters*> &params);
};

struct Header
{
	static void print_parameters(bool grp_key, const std::string& grp_name, const header_list& header, int max_n_chars,
	                             std::ostream& stream = std::cout);

	static void print_parameters(const std::vector<Factory::parameters*> &params, const bool full = true,
	                             std::ostream& stream = std::cout);

	static void compute_max_n_chars(const header_list& header, int& max_n_chars);
};
}
}

#endif /* FACTORY_HPP */
