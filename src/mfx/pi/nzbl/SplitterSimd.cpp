// COMPILATION UNIT ///////////////

// Code automatically generated.

#include "mfx/pi/nzbl/SplitterSimd.h"

namespace mfx {
namespace pi {
namespace nzbl {

void	SplitterSimd::Filter3::set_apf_1p (int idx, int lane, float b0) noexcept
{
	(reinterpret_cast <float *> (&_ap1 [idx])) [lane] = b0;
}

void	SplitterSimd::Filter3::set_apf_2p (int idx, int lane, float b0, float b1) noexcept
{
	(reinterpret_cast <float *> (&_ap2 [idx] [0])) [lane] = b0;
	(reinterpret_cast <float *> (&_ap2 [idx] [1])) [lane] = b1;
}

void	SplitterSimd::Filter3::clear_buffers () noexcept
{
	std::fill (std::begin (_z1), std::end (_z1), fstb::Vf32::zero ());
	std::fill (std::begin (_z2), std::end (_z2), fstb::Vf32::zero ());
}

void	SplitterSimd::Filter0::set_apf_1p (int idx, int lane, float b0) noexcept
{
	(reinterpret_cast <float *> (&_ap1 [idx])) [lane] = b0;
}

void	SplitterSimd::Filter0::set_apf_2p (int idx, int lane, float b0, float b1) noexcept
{
	(reinterpret_cast <float *> (&_ap2 [idx] [0])) [lane] = b0;
	(reinterpret_cast <float *> (&_ap2 [idx] [1])) [lane] = b1;
}

void	SplitterSimd::Filter0::clear_buffers () noexcept
{
	std::fill (std::begin (_z1), std::end (_z1), fstb::Vf32::zero ());
	std::fill (std::begin (_z2), std::end (_z2), fstb::Vf32::zero ());
}

void	SplitterSimd::set_splitter_coef (int split_idx, const float a0_arr [2], const float a1_arr [3]) noexcept
{
	switch (split_idx)
	{
	case 0:
		_filter_1.set_apf_2p (0, 0, a0_arr [0], a0_arr [1]);
		_filter_1.set_apf_2p (0, 2, a1_arr [0], a1_arr [1]);
		_filter_1.set_apf_1p (0, 2, a1_arr [2]);
		_filter_0.set_apf_2p (5, 2, a0_arr [0], a0_arr [1]);
		_filter_0.set_apf_2p (1, 2, a0_arr [0], a0_arr [1]);
		break;
	case 1:
		_filter_0.set_apf_2p (4, 0, a0_arr [0], a0_arr [1]);
		_filter_0.set_apf_2p (4, 2, a1_arr [0], a1_arr [1]);
		_filter_0.set_apf_1p (1, 2, a1_arr [2]);
		_filter_0.set_apf_2p (3, 2, a0_arr [0], a0_arr [1]);
		break;
	case 2:
		_filter_1.set_apf_2p (0, 1, a0_arr [0], a0_arr [1]);
		_filter_1.set_apf_2p (0, 3, a1_arr [0], a1_arr [1]);
		_filter_1.set_apf_1p (0, 3, a1_arr [2]);
		_filter_0.set_apf_2p (5, 0, a0_arr [0], a0_arr [1]);
		_filter_0.set_apf_2p (2, 2, a0_arr [0], a0_arr [1]);
		break;
	case 3:
		_filter_0.set_apf_2p (0, 0, a0_arr [0], a0_arr [1]);
		_filter_0.set_apf_2p (0, 2, a1_arr [0], a1_arr [1]);
		_filter_0.set_apf_1p (0, 2, a1_arr [2]);
		break;
	case 4:
		_filter_2.set_apf_2p (0, 0, a0_arr [0], a0_arr [1]);
		_filter_2.set_apf_2p (0, 2, a1_arr [0], a1_arr [1]);
		_filter_2.set_apf_1p (0, 2, a1_arr [2]);
		_filter_0.set_apf_2p (1, 0, a0_arr [0], a0_arr [1]);
		_filter_0.set_apf_2p (5, 3, a0_arr [0], a0_arr [1]);
		break;
	case 5:
		_filter_0.set_apf_2p (4, 1, a0_arr [0], a0_arr [1]);
		_filter_0.set_apf_2p (4, 3, a1_arr [0], a1_arr [1]);
		_filter_0.set_apf_1p (1, 3, a1_arr [2]);
		_filter_0.set_apf_2p (3, 0, a0_arr [0], a0_arr [1]);
		break;
	case 6:
		_filter_2.set_apf_2p (0, 1, a0_arr [0], a0_arr [1]);
		_filter_2.set_apf_2p (0, 3, a1_arr [0], a1_arr [1]);
		_filter_2.set_apf_1p (0, 3, a1_arr [2]);
		_filter_0.set_apf_2p (2, 0, a0_arr [0], a0_arr [1]);
		_filter_0.set_apf_2p (5, 1, a0_arr [0], a0_arr [1]);
		break;
	}
}

void	SplitterSimd::clear_buffers () noexcept
{
	_filter_0.clear_buffers ();
	_filter_1.clear_buffers ();
	_filter_2.clear_buffers ();
}

void	SplitterSimd::process_block (const float *src_ptr, int nbr_spl) noexcept
{
	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		process_sample (src_ptr [pos]);
		offset_band_ptr (1);
	}
	offset_band_ptr (-nbr_spl);
}

} // namespace nzbl
} // namespace pi
} // namespace mfx



