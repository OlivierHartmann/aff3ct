#ifndef LAUNCHER_BCH_HPP_
#define LAUNCHER_BCH_HPP_

#include "Factory/Module/Codec/BCH/Codec_BCH.hpp"

namespace aff3ct
{
namespace launcher
{
template <class L, typename B = int, typename R = float, typename Q = R>
class BCH : public L
{
protected:
	factory::Codec_BCH::parameters *params_cdc;

public:
	BCH(const int argc, const char **argv, std::ostream &stream = std::cout);
	virtual ~BCH() = default;

protected:
	virtual void register_arguments(CLI::App &app);
	virtual void callback_arguments();
};
}
}

#endif /* LAUNCHER_BCH_HPP_ */
