#ifndef FACTORY_ENCODER_REPETITION_HPP
#define FACTORY_ENCODER_REPETITION_HPP

#include <string>

#include "Module/Encoder/Repetition/Encoder_repetition_sys.hpp"

#include "../Encoder.hpp"

namespace aff3ct
{
namespace factory
{
extern const std::string Encoder_repetition_name;
extern const std::string Encoder_repetition_prefix;
struct Encoder_repetition : public Encoder
{
	class parameters : public Encoder::parameters
	{
	public:
		// ------------------------------------------------------------------------------------------------- PARAMETERS
		// optional
		bool not_buffered = false;

		// ---------------------------------------------------------------------------------------------------- METHODS
		explicit parameters(const std::string &p = Encoder_repetition_prefix);
		virtual ~parameters() = default;
		Encoder_repetition::parameters* clone() const;

		// parameters construction
		void register_arguments(CLI::App &app);
		void callback_arguments();
		void get_headers(std::map<std::string,header_list>& headers, const bool full = true) const;

		// builder
		template <typename B = int>
		module::Encoder_repetition_sys<B>* build() const;
	};

	template <typename B = int>
	static module::Encoder_repetition_sys<B>* build(const parameters &params);
};
}
}

#endif /* FACTORY_ENCODER_REPETITION_HPP */
