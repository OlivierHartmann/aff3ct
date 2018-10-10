#ifndef FACTORY_DECODER_HPP_
#define FACTORY_DECODER_HPP_

#include <string>
#include <memory>

#include "Module/Decoder/Decoder_SIHO.hpp"
#include "Module/Encoder/Encoder.hpp"

#include "../../Factory.hpp"

namespace aff3ct
{
namespace factory
{
extern const std::string Decoder_name;
extern const std::string Decoder_prefix;
struct Decoder : Factory
{
	class parameters : public Factory::parameters
	{
	public:
		// ------------------------------------------------------------------------------------------------- PARAMETERS
		// required parameters
		unsigned    K              = 0;
		unsigned    N_cw           = 0;

		// optional parameters
		std::string type           = "ML";
		std::string implem         = "STD";
		unsigned    n_frames       = 1;
		unsigned    tail_length    = 0;
		unsigned    flips          = 3;
		bool        hamming        = false;
		bool        not_systematic = false;

		// deduced parameters
		float       R              = -1.f;

		std::set<std::string> type_set   = {"ML",  "CHASE"};
		std::set<std::string> implem_set = {"STD", "NAIVE"};

		// ---------------------------------------------------------------------------------------------------- METHODS
		virtual ~parameters() = default;
		virtual Decoder::parameters* clone() const;

		// parameters construction
		virtual void register_arguments(CLI::App &app);
		virtual void callback_arguments();
		virtual void get_headers(std::map<std::string,header_list>& headers, const bool full = true) const;

	protected:
		parameters(const std::string &n, const std::string &p);

		template <typename B = int, typename Q = float>
		module::Decoder_SIHO<B,Q>* build(const std::unique_ptr<module::Encoder<B>>& encoder = nullptr) const;
	};
};
}
}

#endif /* FACTORY_DECODER_HPP_ */
