#ifndef DECODER_HD_HPP_
#define DECODER_HD_HPP_

#include "../Decoder_SISO_SIHO.hpp"

namespace aff3ct
{
namespace module
{
template <typename B = int, typename R = float>
class Decoder_HD : public Decoder_SISO_SIHO<B,R>
{
public:
	Decoder_HD(const int K, const int n_frames = 1);
	virtual ~Decoder_HD() = default;

protected:
	void _decode_siso   (const R *sys, const R *par, R *ext, const int frame_id);
	void _decode_siso   (const R *Y_K1, R *Y_K2, const int frame_id);
	void _decode_siho   (const R *Y_K,  B *V_K,  const int frame_id);
	void _decode_siho_cw(const R *Y_K,  B *V_K,  const int frame_id);
};
}
}

#endif /* DECODER_HD_HPP_ */