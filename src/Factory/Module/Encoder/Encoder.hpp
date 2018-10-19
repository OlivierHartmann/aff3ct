#ifndef FACTORY_ENCODER_HPP
#define FACTORY_ENCODER_HPP

#include <string>
#include <set>

#include "Module/Encoder/Encoder.hpp"

#include "../../Factory.hpp"

namespace aff3ct
{
namespace factory
{
extern const std::string Encoder_name;
extern const std::string Encoder_prefix;
struct Encoder : public Factory
{
	class parameters : public Factory::parameters
	{
	public:
		// ------------------------------------------------------------------------------------------------- PARAMETERS
		// required parameters
		unsigned    K           = 0;
		unsigned    N_cw        = 0;

		// optional parameters
		std::string type        = "NO";
		std::string path        = "";
		unsigned    n_frames    = 1;
		unsigned    seed        = 0;
		unsigned    tail_length = 0;
		unsigned    start_idx   = 0;
		bool        not_systematic = false;

		// deduced parameters
		float       R           = -1.f;

		std::set<std::string> type_set = {"NO", "USER", "AZCW", "COSET"};

		// ---------------------------------------------------------------------------------------------------- METHODS
		explicit parameters(const std::string &p = Encoder_prefix);
		virtual ~parameters() = default;
		virtual Encoder::parameters* clone() const;

		// parameters construction
		virtual void register_arguments(CLI::App &app);
		virtual void callback_arguments();
		virtual void get_headers(std::map<std::string,header_list>& headers, const bool full = true) const;

		// builder
		template <typename B = int>
		module::Encoder<B>* build() const;

		bool type_option_set_by_user() const;

	protected:
		parameters(const std::string &n, const std::string &p);

		CLI::Option* type_option = nullptr;
	};

	template <typename B = int>
	static module::Encoder<B>* build(const parameters &params);
};
}
}

#endif /* FACTORY_ENCODER_HPP */
