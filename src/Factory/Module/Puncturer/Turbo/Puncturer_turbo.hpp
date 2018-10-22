#ifndef FACTORY_PUNCTURER_TURBO_HPP
#define FACTORY_PUNCTURER_TURBO_HPP

#include <string>
#include <vector>

#include "Module/Puncturer/Puncturer.hpp"

#include "../Puncturer.hpp"

namespace aff3ct
{
namespace factory
{
extern const std::string Puncturer_turbo_name;
extern const std::string Puncturer_turbo_prefix;
struct Puncturer_turbo : public Puncturer
{
	class parameters : public Puncturer::parameters
	{
	public:
		// ------------------------------------------------------------------------------------------------- PARAMETERS
		// optional parameters
		std::vector<std::vector<bool>> pattern      = {{1,1,1},{1,1,1},{1,1,1}};
		bool                           not_buffered = false;
		unsigned                       tail_length  = 0;

		// ---------------------------------------------------------------------------------------------------- METHODS
		explicit parameters(const std::string &p = Puncturer_turbo_prefix);
		virtual ~parameters() = default;
		Puncturer_turbo::parameters* clone() const;

		// parameters construction
		void register_arguments(CLI::App &app);
		void callback_arguments();
		void get_headers(std::map<std::string,header_list>& headers, const bool full = true) const;

		// builder
		template <typename B = int, typename Q = float>
		module::Puncturer<B,Q>* build() const;

	private:
		std::string str_pattern = "{111,111,111}";
	};

	template <typename B = int, typename Q = float>
	static module::Puncturer<B,Q>* build(const parameters &params);
};
}
}

#endif /* FACTORY_PUNCTURER_TURBO_HPP */
