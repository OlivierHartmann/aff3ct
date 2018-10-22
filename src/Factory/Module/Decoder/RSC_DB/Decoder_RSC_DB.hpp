#ifndef FACTORY_DECODER_RSC_DB_HPP
#define FACTORY_DECODER_RSC_DB_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "Tools/Math/max.h"

#include "Module/Decoder/RSC_DB/BCJR/Decoder_RSC_DB_BCJR.hpp"
#include "Module/Encoder/Encoder.hpp"

#include "../Decoder.hpp"

namespace aff3ct
{
namespace factory
{
extern const std::string Decoder_RSC_DB_name;
extern const std::string Decoder_RSC_DB_prefix;
struct Decoder_RSC_DB : public Decoder
{
	class parameters : public Decoder::parameters
	{
	public:
		// ------------------------------------------------------------------------------------------------- PARAMETERS
		// optional parameters
		std::string max          = "MAX";
		bool        not_buffered = true;

		// ---------------------------------------------------------------------------------------------------- METHODS
		explicit parameters(const std::string &p = Decoder_RSC_DB_prefix);
		parameters(const std::string &n, const std::string &p);
		virtual ~parameters() = default;
		Decoder_RSC_DB::parameters* clone() const;

		// parameters construction
		void register_arguments(CLI::App &app);
		void callback_arguments();
		void get_headers(std::map<std::string,header_list>& headers, const bool full = true) const;

		// builder
		template <typename B = int, typename Q = float>
		module::Decoder_RSC_DB_BCJR<B,Q>* build_siso(const std::vector<std::vector<int>> &trellis,
		                                             const std::unique_ptr<module::Encoder<B>>& encoder = nullptr) const;

		template <typename B = int, typename Q = float>
		module::Decoder_SIHO<B,Q>* build(const std::vector<std::vector<int>> &trellis,
		                                       const std::unique_ptr<module::Encoder<B>>& encoder = nullptr) const;

	private:
		template <typename B = int, typename Q = float, tools::proto_max<Q> MAX>
		module::Decoder_RSC_DB_BCJR<B,Q>* _build_siso(const std::vector<std::vector<int>> &trellis,
		                                                    const std::unique_ptr<module::Encoder<B>>& encoder = nullptr) const;
	};

	template <typename B = int, typename Q = float>
	static module::Decoder_RSC_DB_BCJR<B,Q>* build_siso(const parameters &params,
	                                                    const std::vector<std::vector<int>> &trellis,
	                                                          const std::unique_ptr<module::Encoder<B>>& encoder = nullptr);

	template <typename B = int, typename Q = float>
	static module::Decoder_SIHO<B,Q>* build(const parameters                    &params,
	                                        const std::vector<std::vector<int>> &trellis,
	                                              const std::unique_ptr<module::Encoder<B>>& encoder = nullptr);
};
}
}

#endif /* FACTORY_DECODER_RSC_DB_HPP */
