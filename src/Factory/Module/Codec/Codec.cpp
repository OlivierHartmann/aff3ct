#include "Codec.hpp"

using namespace aff3ct::factory;

const std::string aff3ct::factory::Codec::name   = "Codec";
const std::string aff3ct::factory::Codec::prefix = "cdc";

Codec::parameters
::parameters(const std::string prefix)
: Factory::parameters(Codec::name, Codec::name, prefix)
{
}

Codec::parameters
::parameters(const std::string name, const std::string prefix)
: Factory::parameters(name, Codec::name, prefix)
{
}

Codec::parameters
::~parameters()
{
}

Codec::parameters* Codec::parameters
::clone() const
{
	return new Codec::parameters(*this);
}

void Codec::parameters
::enable_puncturer()
{
	throw tools::runtime_error(__FILE__, __LINE__, __func__, "This codec does not support to be punctured.");
}

std::vector<std::string> Codec::parameters
::get_names() const
{
	auto n = Factory::parameters::get_names();
	if (enc != nullptr) { auto nn = enc->get_names(); for (auto &x : nn) n.push_back(x); }
	if (dec != nullptr) { auto nn = dec->get_names(); for (auto &x : nn) n.push_back(x); }
	if (itl != nullptr) { auto nn = itl->get_names(); for (auto &x : nn) n.push_back(x); }
	return n;
}
std::vector<std::string> Codec::parameters
::get_short_names() const
{
	auto sn = Factory::parameters::get_short_names();
	if (enc != nullptr) { auto nn = enc->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	if (dec != nullptr) { auto nn = dec->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	if (itl != nullptr) { auto nn = itl->get_short_names(); for (auto &x : nn) sn.push_back(x); }
	return sn;
}

std::vector<std::string> Codec::parameters
::get_prefixes() const
{
	auto p = Factory::parameters::get_prefixes();
	if (enc != nullptr) { auto nn = enc->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	if (dec != nullptr) { auto nn = dec->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	if (itl != nullptr) { auto nn = itl->get_prefixes(); for (auto &x : nn) p.push_back(x); }
	return p;
}

void Codec::parameters
::get_description(arg_map &req_args, arg_map &opt_args) const
{
}

void Codec::parameters
::store(const arg_val_map &vals)
{
}

void Codec::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
}

void Codec::parameters
::set_enc(Encoder::parameters *enc)
{
	this->enc = enc;
}

void Codec::parameters
::set_dec(Decoder::parameters *dec)
{
	this->dec = dec;
}

void Codec::parameters
::set_itl(Interleaver::parameters *itl)
{
	this->itl = itl;
}
