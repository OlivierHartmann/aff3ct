#include "Simulation/BFER/Standard/SystemC/SC_BFER_std.hpp"
#include "Simulation/BFER/Standard/Threads/BFER_std_threads.hpp"

#include "BFER_std.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::BFER_std_name   = "Simulation BFER standard";
const std::string aff3ct::factory::BFER_std_prefix = "sim";

BFER_std::parameters
::parameters(const std::string &prefix)
: BFER::parameters(BFER_std_name, prefix)
{
}

BFER_std::parameters* BFER_std::parameters
::clone() const
{
	return new BFER_std::parameters(*this);
}

void BFER_std::parameters
::register_arguments(CLI::App &app)
{
	BFER::parameters::register_arguments(app);
}

void BFER_std::parameters
::callback_arguments()
{
	BFER::parameters::callback_arguments();
}

void BFER_std::parameters
::get_headers(std::map<std::string,header_list>& headers, const bool full) const
{
	BFER::parameters::get_headers(headers, full);
}

const Codec_SIHO::parameters* BFER_std::parameters
::get_cdc() const
{
	return dynamic_cast<Codec_SIHO::parameters*>(this->cdc.get());
}



template <typename B, typename R, typename Q>
simulation::BFER_std<B,R,Q>* BFER_std::parameters
::build() const
{
#if defined(SYSTEMC)
	return new simulation::SC_BFER_std<B,R,Q>(*this);
#else
	return new simulation::BFER_std_threads<B,R,Q>(*this);
#endif
}

template <typename B, typename R, typename Q>
simulation::BFER_std<B,R,Q>* BFER_std
::build(const parameters &params)
{
	return params.template build<B,R,Q>();
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef MULTI_PREC
template aff3ct::simulation::BFER_std<B_8 ,R_8 ,Q_8 >* aff3ct::factory::BFER_std::build<B_8 ,R_8 ,Q_8 >(const aff3ct::factory::BFER_std::parameters&);
template aff3ct::simulation::BFER_std<B_16,R_16,Q_16>* aff3ct::factory::BFER_std::build<B_16,R_16,Q_16>(const aff3ct::factory::BFER_std::parameters&);
template aff3ct::simulation::BFER_std<B_32,R_32,Q_32>* aff3ct::factory::BFER_std::build<B_32,R_32,Q_32>(const aff3ct::factory::BFER_std::parameters&);
template aff3ct::simulation::BFER_std<B_64,R_64,Q_64>* aff3ct::factory::BFER_std::build<B_64,R_64,Q_64>(const aff3ct::factory::BFER_std::parameters&);
#else
template aff3ct::simulation::BFER_std<B,R,Q>* aff3ct::factory::BFER_std::build<B,R,Q>(const aff3ct::factory::BFER_std::parameters&);
#endif
// ==================================================================================== explicit template instantiation
