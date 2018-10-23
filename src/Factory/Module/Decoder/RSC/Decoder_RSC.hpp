#ifndef FACTORY_DECODER_RSC_HPP
#define FACTORY_DECODER_RSC_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "Tools/Math/max.h"

#include "Module/Decoder/Decoder_SISO_SIHO.hpp"
#include "Module/Encoder/Encoder.hpp"

#include "../Decoder.hpp"

namespace aff3ct
{
namespace factory
{
extern const std::string Decoder_RSC_name;
extern const std::string Decoder_RSC_prefix;
struct Decoder_RSC : public Decoder
{
	class parameters : public Decoder::parameters
	{
	public:
		// ------------------------------------------------------------------------------------------------- PARAMETERS
		// optional parameters
		std::string      max           = "MAX";
		std::string      simd_strategy = "";
		std::string      standard      = "LTE";
		bool             not_buffered  = false;
		std::vector<int> poly          = {013, 015};

		void delete_poly_option(CLI::App *);

		// ---------------------------------------------------------------------------------------------------- METHODS
		explicit parameters(const std::string &p = Decoder_RSC_prefix);
		parameters(const std::string &n, const std::string &p);
		virtual ~parameters() = default;

		Decoder_RSC::parameters* clone() const;

		// parameters construction
		void register_arguments(CLI::App &app);
		void callback_arguments();
		void get_headers(std::map<std::string,header_list>& headers, const bool full = true) const;

		// builder
		template <typename B = int, typename Q = float>
		module::Decoder_SIHO<B,Q>* build(const std::vector<std::vector<int>> &trellis,
		                                       std::ostream                  &stream  = std::cout,
		                                 const int                            n_ite   = 1,
		                                 const std::unique_ptr<module::Encoder<B>>& encoder = nullptr) const;

		template <typename B = int, typename Q = float>
		module::Decoder_SISO_SIHO<B,Q>* build_siso(const std::vector<std::vector<int>> &trellis,
		                                                 std::ostream                  &stream  = std::cout,
		                                           const int                            n_ite   = 1,
		                                           const std::unique_ptr<module::Encoder<B>>& encoder = nullptr) const;

	private:
		std::string  poly_str    = "{013, 015}";
		CLI::Option *poly_option = nullptr;

		template <typename B = int, typename Q = float, typename QD = Q, tools::proto_max<Q> MAX1, tools::proto_max<QD> MAX2>
		module::Decoder_SISO_SIHO<B,Q>* _build_siso_seq(const std::vector<std::vector<int>> &trellis,
		                                                      std::ostream                  &stream  = std::cout,
		                                                const int                            n_ite   = 1,
		                                                const std::unique_ptr<module::Encoder<B>>& encoder = nullptr) const;

		template <typename B = int, typename Q = float, typename QD = Q, tools::proto_max_i<Q> MAX>
		module::Decoder_SISO_SIHO<B,Q>* _build_siso_simd(const std::vector<std::vector<int>> &trellis,
		                                                 const std::unique_ptr<module::Encoder<B>>& encoder = nullptr) const;
	};

	template <typename B = int, typename Q = float>
	static module::Decoder_SIHO<B,Q>* build(const parameters                    &params,
	                                        const std::vector<std::vector<int>> &trellis,
	                                              std::ostream                  &stream  = std::cout,
	                                        const int                            n_ite   = 1,
	                                        const std::unique_ptr<module::Encoder<B>>& encoder = nullptr);

	template <typename B = int, typename Q = float>
	static module::Decoder_SISO_SIHO<B,Q>* build_siso(const parameters                    &params,
	                                                  const std::vector<std::vector<int>> &trellis,
	                                                        std::ostream                  &stream  = std::cout,
	                                                  const int                            n_ite   = 1,
	                                                  const std::unique_ptr<module::Encoder<B>>& encoder = nullptr);
};
}
}

#endif /* FACTORY_DECODER_RSC_HPP */
