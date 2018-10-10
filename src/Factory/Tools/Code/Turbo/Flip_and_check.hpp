#ifndef FACTORY_FLIP_AND_CHECK_HPP
#define FACTORY_FLIP_AND_CHECK_HPP

#include <string>

#include "Tools/Code/Turbo/Post_processing_SISO/Flip_and_check/Flip_and_check.hpp"

#include "../../../Factory.hpp"

namespace aff3ct
{
namespace factory
{
extern const std::string Flip_and_check_name;
extern const std::string Flip_and_check_prefix;
struct Flip_and_check : public Factory
{
	class parameters : public Factory::parameters
	{
	public:
		// ------------------------------------------------------------------------------------------------- PARAMETERS
		// required parameters
		unsigned size                = 0;

		// optional parameters
		bool     enable              = false;
		unsigned q                   = 10;
		unsigned n_ite               = 6;
		unsigned ite_min             = 3;
		unsigned ite_max             = 10;
		unsigned ite_step            = 1;
		unsigned start_crc_check_ite = 2;
		unsigned n_frames            = 1;

		// ---------------------------------------------------------------------------------------------------- METHODS
		explicit parameters(const std::string &p = Flip_and_check_prefix);
		virtual ~parameters() = default;
		virtual Flip_and_check::parameters* clone() const;

		// parameters construction
		virtual void register_arguments(CLI::App &app);
		virtual void callback_arguments();
		virtual void get_headers(std::map<std::string,header_list>& headers, const bool full = true) const;

		// builder
		template <typename B = int, typename Q = float>
		tools::Flip_and_check<B,Q>* build(module::CRC<B> &crc) const;

	protected:
		parameters(const std::string &n, const std::string &p);

	private:
		CLI::Option* ite_M_option;
	};

	template <typename B = int, typename Q = float>
	static tools::Flip_and_check<B,Q>* build(const parameters& params, module::CRC<B> &crc);
};
}
}

#endif /* FACTORY_FLIP_AND_CHECK_HPP */
