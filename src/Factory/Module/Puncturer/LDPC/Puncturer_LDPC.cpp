#include <sstream>

#include "Tools/general_utils.h"
#include "Tools/Exception/exception.hpp"

#include "Module/Puncturer/NO/Puncturer_NO.hpp"
#include "Module/Puncturer/LDPC/Puncturer_LDPC.hpp"

#include "Puncturer_LDPC.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Puncturer_LDPC_name   = "Puncturer LDPC";
const std::string aff3ct::factory::Puncturer_LDPC_prefix = "pct";

Puncturer_LDPC::parameters
::parameters(const std::string &prefix)
: Puncturer::parameters(Puncturer_LDPC_name, prefix)
{
	type = "LDPC";

	type_set.insert("LDPC");
}

Puncturer_LDPC::parameters* Puncturer_LDPC::parameters
::clone() const
{
	return new Puncturer_LDPC::parameters(*this);
}

void Puncturer_LDPC::parameters
::register_arguments(CLI::App &app)
{
	Puncturer::parameters::register_arguments(app);

	auto sub = CLI::make_subcommand(app, get_prefix(), get_name() + " parameters");

	sub->add_option(
		"--cw-size",
		N_cw,
		"The codeword size.")
		->required()
		->check(CLI::StrictlyPositiveRange(0u))
		->group("Standard");

	sub->add_option(
		"--pattern",
		pattern,
		"Puncturing pattern for the LDPC encoder/decoder (size = N_Code/Z) (ex: \"{1,1,1,0}\").")
		->check(CLI::Boolean())
		->group("Standard");
}

void Puncturer_LDPC::parameters
::callback_arguments()
{
	auto save_N_cw = N_cw;
	Puncturer::parameters::callback_arguments();
	if (save_N_cw != 0)
		N_cw = save_N_cw;

	if (N == N_cw)
		type = "NO";
}

void Puncturer_LDPC::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	Puncturer::parameters::get_headers(headers, full);

	auto p = get_prefix();

	if (type != "NO")
	{
		std::stringstream pat;
		for (auto p : pattern)
			pat << p;

		headers[p].push_back(std::make_pair(std::string("Pattern"), std::string("{" + pat.str() + "}")));
	}
}

template <typename B, typename Q>
module::Puncturer<B,Q>* Puncturer_LDPC::parameters
::build() const
{
	if (type == "LDPC") return new module::Puncturer_LDPC<B,Q>(K, N, N_cw, pattern, n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Puncturer<B,Q>* Puncturer_LDPC
::build(const parameters &params)
{
	return params.template build<B,Q>();
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Puncturer<B_8 ,Q_8 >* aff3ct::factory::Puncturer_LDPC::parameters::build<B_8 ,Q_8 >() const;
template aff3ct::module::Puncturer<B_16,Q_16>* aff3ct::factory::Puncturer_LDPC::parameters::build<B_16,Q_16>() const;
template aff3ct::module::Puncturer<B_32,Q_32>* aff3ct::factory::Puncturer_LDPC::parameters::build<B_32,Q_32>() const;
template aff3ct::module::Puncturer<B_64,Q_64>* aff3ct::factory::Puncturer_LDPC::parameters::build<B_64,Q_64>() const;
template aff3ct::module::Puncturer<B_8 ,Q_8 >* aff3ct::factory::Puncturer_LDPC::build<B_8 ,Q_8 >(const aff3ct::factory::Puncturer_LDPC::parameters&);
template aff3ct::module::Puncturer<B_16,Q_16>* aff3ct::factory::Puncturer_LDPC::build<B_16,Q_16>(const aff3ct::factory::Puncturer_LDPC::parameters&);
template aff3ct::module::Puncturer<B_32,Q_32>* aff3ct::factory::Puncturer_LDPC::build<B_32,Q_32>(const aff3ct::factory::Puncturer_LDPC::parameters&);
template aff3ct::module::Puncturer<B_64,Q_64>* aff3ct::factory::Puncturer_LDPC::build<B_64,Q_64>(const aff3ct::factory::Puncturer_LDPC::parameters&);
#else
template aff3ct::module::Puncturer<B,Q>* aff3ct::factory::Puncturer_LDPC::parameters::build<B,Q>() const;
template aff3ct::module::Puncturer<B,Q>* aff3ct::factory::Puncturer_LDPC::build<B,Q>(const aff3ct::factory::Puncturer_LDPC::parameters&);
#endif
// ==================================================================================== explicit template instantiation
