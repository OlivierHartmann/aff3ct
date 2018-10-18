#include "Tools/Exception/exception.hpp"

#include "Module/Decoder/RA/Decoder_RA.hpp"

#include "Decoder_RA.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Decoder_RA_name   = "Decoder RA";
const std::string aff3ct::factory::Decoder_RA_prefix = "dec";

Decoder_RA::parameters
::parameters(const std::string &prefix)
: Decoder::parameters(Decoder_RA_name, prefix),
  itl(new Interleaver::parameters("itl"))
{
	type   = "RA";
	implem = "STD";

	type_set  .insert({"RA"});
	implem_set.insert({"STD"});
}

Decoder_RA::parameters* Decoder_RA::parameters
::clone() const
{
	return new Decoder_RA::parameters(*this);
}

std::vector<std::string> Decoder_RA::parameters
::get_names() const
{
	auto n = Decoder::parameters::get_names();
	if (itl != nullptr) { auto nn = itl->get_names(); for (auto &x : nn) n.push_back(x); }
	return n;
}
std::vector<std::string> Decoder_RA::parameters
::get_short_names() const
{
	auto sn = Decoder::parameters::get_short_names();
	if (itl != nullptr) { auto nn = itl->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	return sn;
}

std::vector<std::string> Decoder_RA::parameters
::get_prefixes() const
{
	auto p = Decoder::parameters::get_prefixes();
	if (itl != nullptr) { auto nn = itl->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	return p;
}

void Decoder_RA::parameters
::register_arguments(CLI::App &app)
{
	auto p = get_prefix();

	Decoder::parameters::register_arguments(app);

	if (itl != nullptr)
	{
		itl->register_arguments(app);

		CLI::remove_option(app, "--size", itl->get_prefix());
		CLI::remove_option(app, "--fra" , itl->get_prefix());
	}

	CLI::add_option(app, p,
		"-i,--ite",
		n_ite,
		"Maximal number of iterations in the decoder.",
		true)
		->group("Standard");
}

void Decoder_RA::parameters
::callback_arguments()
{
	Decoder::parameters::callback_arguments();

	if (itl != nullptr)
	{
		itl->core->size     = N_cw;
		itl->core->n_frames = n_frames;

		itl->callback_arguments();
	}
}

void Decoder_RA::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	auto p = get_short_name();

	Decoder::parameters::get_headers(headers, full);

	if (itl != nullptr)
		itl->get_headers(headers, full);

	if (type != "ML" && type != "CHASE")
		headers[p].push_back(std::make_pair("Num. of iterations (i)", std::to_string(n_ite)));
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_RA::parameters
::build(const module::Interleaver<Q> &itl, const std::unique_ptr<module::Encoder<B>>& encoder) const
{
	try
	{
		return Decoder::parameters::build<B,Q>(encoder);
	}
	catch (tools::cannot_allocate const&)
	{
		if (type == "RA")
		{
			if (implem == "STD" ) return new module::Decoder_RA<B,Q>(K, N_cw, itl, n_ite, n_frames);
		}
	}

	throw tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template <typename B, typename Q>
module::Decoder_SIHO<B,Q>* Decoder_RA
::build(const parameters &params, const module::Interleaver<Q> &itl, const std::unique_ptr<module::Encoder<B>>& encoder)
{
	return params.template build<B,Q>(itl, encoder);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_RA::parameters::build<B_8 ,Q_8 >(const aff3ct::module::Interleaver<Q_8 >&, const std::unique_ptr<module::Encoder<B_8 >>&) const;
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_RA::parameters::build<B_16,Q_16>(const aff3ct::module::Interleaver<Q_16>&, const std::unique_ptr<module::Encoder<B_16>>&) const;
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_RA::parameters::build<B_32,Q_32>(const aff3ct::module::Interleaver<Q_32>&, const std::unique_ptr<module::Encoder<B_32>>&) const;
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_RA::parameters::build<B_64,Q_64>(const aff3ct::module::Interleaver<Q_64>&, const std::unique_ptr<module::Encoder<B_64>>&) const;
template aff3ct::module::Decoder_SIHO<B_8 ,Q_8 >* aff3ct::factory::Decoder_RA::build<B_8 ,Q_8 >(const aff3ct::factory::Decoder_RA::parameters&, const aff3ct::module::Interleaver<Q_8 >&, const std::unique_ptr<module::Encoder<B_8 >>&);
template aff3ct::module::Decoder_SIHO<B_16,Q_16>* aff3ct::factory::Decoder_RA::build<B_16,Q_16>(const aff3ct::factory::Decoder_RA::parameters&, const aff3ct::module::Interleaver<Q_16>&, const std::unique_ptr<module::Encoder<B_16>>&);
template aff3ct::module::Decoder_SIHO<B_32,Q_32>* aff3ct::factory::Decoder_RA::build<B_32,Q_32>(const aff3ct::factory::Decoder_RA::parameters&, const aff3ct::module::Interleaver<Q_32>&, const std::unique_ptr<module::Encoder<B_32>>&);
template aff3ct::module::Decoder_SIHO<B_64,Q_64>* aff3ct::factory::Decoder_RA::build<B_64,Q_64>(const aff3ct::factory::Decoder_RA::parameters&, const aff3ct::module::Interleaver<Q_64>&, const std::unique_ptr<module::Encoder<B_64>>&);
#else
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_RA::parameters::build<B,Q>(const aff3ct::module::Interleaver<Q>&, const std::unique_ptr<module::Encoder<B>>& ) const;
template aff3ct::module::Decoder_SIHO<B,Q>* aff3ct::factory::Decoder_RA::build<B,Q>(const aff3ct::factory::Decoder_RA::parameters&, const aff3ct::module::Interleaver<Q>&, const std::unique_ptr<module::Encoder<B>>& );
#endif
// ==================================================================================== explicit template instantiation
