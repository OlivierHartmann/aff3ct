#include "Tools/general_utils.h"
#include "Tools/Exception/exception.hpp"

#include "Module/Puncturer/NO/Puncturer_NO.hpp"
#include "Module/Puncturer/Turbo/Puncturer_turbo.hpp"

#include "Puncturer_turbo.hpp"

#include "Tools/types.h"
#ifdef MULTI_PREC
using PT = aff3ct::module::Puncturer_turbo<B_32,Q_32>;
#else
using PT = aff3ct::module::Puncturer_turbo<B,Q>;
#endif

using namespace aff3ct;
using namespace aff3ct::factory;


const std::string aff3ct::factory::Puncturer_turbo_name   = "Puncturer Turbo";
const std::string aff3ct::factory::Puncturer_turbo_prefix = "pct";


Puncturer_turbo::parameters
::parameters(const std::string &prefix)
: Puncturer::parameters(Puncturer_turbo_name, prefix)
{
	type = "TURBO";

	type_set.insert("TURBO");
}

Puncturer_turbo::parameters* Puncturer_turbo::parameters
::clone() const
{
	return new Puncturer_turbo::parameters(*this);
}

// struct sub_same_length
// {
// 	template <typename T>
// 	static void check(const std::vector<T>& v)
// 	{
// 		for (unsigned i = 0; i < v.size(); i++)
// 			if (v[i].size() != v[0].size())
// 				throw std::runtime_error("all elements do not have the same length");
// 	}
// };


void Puncturer_turbo::parameters
::register_arguments(CLI::App &app)
{
	Puncturer::parameters::register_arguments(app);

	auto sub = CLI::make_subcommand(app, get_prefix(), get_name() + " parameters");

	sub->remove_option(sub->get_option("--fra-size"));

	sub->add_option(
		"--pattern",
		str_pattern,
		"Puncturing pattern for the turbo encoder (ex: \"11,10,01\").",
		true)
		->group("Standard");

	// args.add(
	// 	{p+"-pattern"},
	// 	tools::List2D<bool>(tools::Boolean(),
	// 	                    std::make_tuple(tools::Length(3, 3), tools::Function<sub_same_length>("elements of same length")),
	// 	                    std::make_tuple(tools::Length(1))),
	// 	"puncturing pattern for the turbo encoder (ex: \"11,10,01\").");

	sub->add_option(
		"--tail-length",
		tail_length,
		"Total number of tail bits at the end of the frame.",
		true)
		->group("Standard");

	sub->add_flag(
		"--no-buff",
		no_buffered,
		"Does not suppose a buffered encoding")
		->group("Standard");
}

void Puncturer_turbo::parameters
::callback_arguments()
{
	Puncturer::parameters::callback_arguments();

	N_cw = 3 * K + tail_length;
	N    = PT::compute_N(K, tail_length, pattern);

	if (N == N_cw)
		type = "NO";
}

void Puncturer_turbo::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	Puncturer::parameters::get_headers(headers, full);

	auto p = get_prefix();

	if (type != "NO")
	{
		headers[p].push_back(std::make_pair(std::string("Pattern"), std::string("{" + PT::display_pattern(pattern) + "}")));
		if (full) headers[p].push_back(std::make_pair(std::string("Tail length"), std::to_string(tail_length)));
		if (full) headers[p].push_back(std::make_pair(std::string("Buffered"), no_buffered ? "off" : "on"));
	}
}

template <typename B, typename Q>
module::Puncturer<B,Q>* Puncturer_turbo::parameters
::build() const
{
	if (type == "TURBO") return new module::Puncturer_turbo<B,Q>(K, N, tail_length, pattern, !no_buffered, n_frames);

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Puncturer<B,Q>* Puncturer_turbo
::build(const parameters &params)
{
	return params.template build<B,Q>();
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Puncturer<B_8 ,Q_8 >* aff3ct::factory::Puncturer_turbo::parameters::build<B_8 ,Q_8 >() const;
template aff3ct::module::Puncturer<B_16,Q_16>* aff3ct::factory::Puncturer_turbo::parameters::build<B_16,Q_16>() const;
template aff3ct::module::Puncturer<B_32,Q_32>* aff3ct::factory::Puncturer_turbo::parameters::build<B_32,Q_32>() const;
template aff3ct::module::Puncturer<B_64,Q_64>* aff3ct::factory::Puncturer_turbo::parameters::build<B_64,Q_64>() const;
template aff3ct::module::Puncturer<B_8 ,Q_8 >* aff3ct::factory::Puncturer_turbo::build<B_8 ,Q_8 >(const aff3ct::factory::Puncturer_turbo::parameters&);
template aff3ct::module::Puncturer<B_16,Q_16>* aff3ct::factory::Puncturer_turbo::build<B_16,Q_16>(const aff3ct::factory::Puncturer_turbo::parameters&);
template aff3ct::module::Puncturer<B_32,Q_32>* aff3ct::factory::Puncturer_turbo::build<B_32,Q_32>(const aff3ct::factory::Puncturer_turbo::parameters&);
template aff3ct::module::Puncturer<B_64,Q_64>* aff3ct::factory::Puncturer_turbo::build<B_64,Q_64>(const aff3ct::factory::Puncturer_turbo::parameters&);
#else
template aff3ct::module::Puncturer<B,Q>* aff3ct::factory::Puncturer_turbo::parameters::build<B,Q>() const;
template aff3ct::module::Puncturer<B,Q>* aff3ct::factory::Puncturer_turbo::build<B,Q>(const aff3ct::factory::Puncturer_turbo::parameters&);
#endif
// ==================================================================================== explicit template instantiation
