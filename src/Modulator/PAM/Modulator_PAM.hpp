#ifndef MODULATOR_PAM_HPP_
#define MODULATOR_PAM_HPP_

#include <vector>
#include "../../Tools/MIPP/mipp.h"
#include "../../Tools/Math/max.h"

#include "../Modulator.hpp"

template <typename B, typename R, typename Q, proto_max<Q> MAX>
class Modulator_PAM : public Modulator<B,R,Q>
{
private:
	const int bits_per_symbol;
	const int nbr_symbols;
	const R sigma;
	const R sqrt_es;
	const bool disable_sig2;
	mipp::vector<R> constellation;

public:
	Modulator_PAM(const int N, const int bits_per_symbol = 1, const R sigma = 1.0, const bool disable_sig2 = false, 
	              const int n_frames = 1, const std::string name = "Modulator_PAM");
	virtual ~Modulator_PAM();

	virtual void   modulate(const mipp::vector<B>& X_N1, mipp::vector<R>& X_N2);
	virtual void demodulate(const mipp::vector<Q>& Y_N1, mipp::vector<Q>& Y_N2);

	int get_buffer_size_after_modulation(const int N);

private:
	inline R bits_to_symbol(const B* bits) const;
};

#include "Modulator_PAM.hxx"

#endif // MODULATOR_PAM_HPP_