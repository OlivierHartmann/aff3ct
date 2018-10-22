#ifndef FACTORY_DECODER_TURBO_HPP
#define FACTORY_DECODER_TURBO_HPP

#include <string>
#include <type_traits>

#include "Module/Decoder/Turbo/Decoder_turbo.hpp"
#include "Module/Decoder/Decoder_SISO.hpp"
#include "Module/Interleaver/Interleaver.hpp"
#include "Module/Encoder/Encoder.hpp"

#include "Factory/Module/Interleaver/Interleaver.hpp"
#include "Factory/Tools/Code/Turbo/Flip_and_check.hpp"
#include "Factory/Tools/Code/Turbo/Scaling_factor.hpp"
#include "Factory/Module/Decoder/RSC/Decoder_RSC.hpp"

#include "Tools/auto_cloned_unique_ptr.hpp"

#include "../Decoder.hpp"

namespace aff3ct
{
namespace factory
{
extern const std::string Decoder_turbo_name;
extern const std::string Decoder_turbo_prefix;
struct Decoder_turbo : public Decoder
{
	template <class D1 = Decoder_RSC, class D2 = D1>
	class parameters : public Decoder::parameters
	{
	public:
		// ------------------------------------------------------------------------------------------------- PARAMETERS
		// optional parameters
		bool      self_corrected = false;
		bool      enable_json    = false;
		unsigned  n_ite          = 6;

		// depending parameters
		tools::auto_cloned_unique_ptr<typename D1   ::parameters> sub1;
		tools::auto_cloned_unique_ptr<typename D2   ::parameters> sub2;
		tools::auto_cloned_unique_ptr<Interleaver   ::parameters> itl;
		tools::auto_cloned_unique_ptr<Scaling_factor::parameters> sf;
		tools::auto_cloned_unique_ptr<Flip_and_check::parameters> fnc;

		// ---------------------------------------------------------------------------------------------------- METHODS
		explicit parameters(const std::string &p = Decoder_turbo_prefix);
		parameters(const std::string &n, const std::string &p);
		virtual ~parameters() = default;
		Decoder_turbo::parameters<D1,D2>* clone() const;

		virtual std::vector<std::string> get_names      () const;
		virtual std::vector<std::string> get_short_names() const;
		virtual std::vector<std::string> get_prefixes   () const;

		// parameters construction
		void register_arguments(CLI::App &app);
		void callback_arguments();
		void get_headers(std::map<std::string,header_list>& headers, const bool full = true) const;

		// builder
		template <typename B = int, typename Q = float>
		module::Decoder_turbo<B,Q>* build(const module::Interleaver<Q>  &itl,
		                                        module::Decoder_SISO<Q> &siso_n,
		                                        module::Decoder_SISO<Q> &siso_i,
		                                        const std::unique_ptr<module::Encoder<B>>& encoder = nullptr) const;

		template <typename B = int, typename Q = float>
		module::Decoder_SIHO<B,Q>* build(const std::unique_ptr<module::Encoder<B>>& encoder = nullptr) const;
	};

	template <typename B = int, typename Q = float, class D1 = Decoder_RSC, class D2 = D1>
	static module::Decoder_turbo<B,Q>* build(const parameters<D1,D2>       &params,
	                                         const module::Interleaver<Q>  &itl,
	                                               module::Decoder_SISO<Q> &siso_n,
	                                               module::Decoder_SISO<Q> &siso_i,
	                                               const std::unique_ptr<module::Encoder<B>>& encoder = nullptr);

	template <typename B = int, typename Q = float, class D1 = Decoder_RSC, class D2 = D1>
	static module::Decoder_SIHO<B,Q>* build(const parameters<D1,D2> &params, const std::unique_ptr<module::Encoder<B>>& encoder = nullptr);
};
}
}

#include "Decoder_turbo.hxx"

#endif /* FACTORY_DECODER_TURBO_HPP */
