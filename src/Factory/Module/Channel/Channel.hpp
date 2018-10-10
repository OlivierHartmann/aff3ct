#ifndef FACTORY_CHANNEL_HPP
#define FACTORY_CHANNEL_HPP

#include <string>
#include <set>
#include <memory>

#include "Module/Channel/Channel.hpp"
#include "Tools/Math/Distribution/Distributions.hpp"

#include "../../Factory.hpp"

namespace aff3ct
{
namespace factory
{
extern const std::string Channel_name;
extern const std::string Channel_prefix;
struct Channel : public Factory
{
	class parameters : public Factory::parameters
	{
	public:
		// ------------------------------------------------------------------------------------------------- PARAMETERS
		// required parameters
		unsigned    N            = 0;

		// optional parameters
		std::string type         = "AWGN";
		std::string implem       = "STD";
		std::string path         = "";
		std::string block_fading = "NO";
		bool        add_users    = false;
		bool        complex      = false;
		unsigned    n_frames     = 1;
		unsigned    seed         = 0;
		unsigned    gain_occur   = 1;
		float       noise        = -1.f;

		std::set<std::string> type_set   = {"NO", "USER", "USER_ADD", "AWGN", "RAYLEIGH", "RAYLEIGH_USER", "BEC", "BSC", "OPTICAL"};
		std::set<std::string> implem_set = {"STD", "FAST"};

		// ---------------------------------------------------------------------------------------------------- METHODS
		explicit parameters(const std::string &p = Channel_prefix);
		virtual ~parameters() = default;
		Channel::parameters* clone() const;

		// parameters construction
		void register_arguments(CLI::App &app);
		void callback_arguments();
		void get_headers(std::map<std::string,header_list>& headers, const bool full = true) const;

		// builder
		template <typename R = float>
		module::Channel<R>* build_gaussian() const;
		template <typename R = float>
		module::Channel<R>* build_event() const;
		template <typename R = float>
		module::Channel<R>* build_userpdf(const tools::Distributions<R>& dist) const;
		template <typename R = float>
		module::Channel<R>* build() const;
		template <typename R = float>
		module::Channel<R>* build(const tools::Distributions<R>& dist) const;
	};

	template <typename R = float>
	static module::Channel<R>* build(const parameters &params);
	template <typename R = float>
	static module::Channel<R>* build(const parameters &params, const tools::Distributions<R>& dist);
};
}
}

#endif /* FACTORY_CHANNEL_HPP */
