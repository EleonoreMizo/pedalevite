// INLINE CODE ////////////////////

// Code automatically generated.

#include "fstb/ToolsSimd.h"
#include "fstb/Vs32.h"

#include <algorithm>
#include <iterator>
#include <tuple>

namespace mfx {
namespace pi {
namespace nzbl {

fstb::Vf32	SplitterSimd::Filter3::process_sample (fstb::Vf32 x) noexcept
{
	// Stage 0
	auto x1z = _z1 [0];
	auto x2z = _z2 [0];
	fstb::Vf32 mask;
	fstb::Vf32 x_save;
	{
		const auto gain = fstb::Vf32 (0.125000f);
		x *= gain;
	}
	{
		const auto b0  = _ap2 [0] [0];
		const auto b1  = _ap2 [0] [1];
		const auto y1z = _z1 [1];
		const auto y2z = _z2 [1];
		const auto y   = b0 * (x - y2z) + b1 * (x1z - y1z) + x2z;
		_z1 [0] = x;
		_z2 [0] = x1z;
		x   = y;
		x1z = y1z;
		x2z = y2z;
	}
	_z2 [1] = x1z;
	x_save = x;
	{
		const auto b0  = _ap1 [0];
		const auto y1z = _z1 [2];
		const auto y   = b0 * (x - y1z) + x1z;
		_z1 [1] = x;
		x   = y;
		x1z = y1z;
	}
	_z1 [2] = x;
	mask = fstb::ToolsSimd::cast_f32 (fstb::Vs32 (-1, -1, 0, 0));
	x    = fstb::ToolsSimd::select (mask, x_save, x);
	x = fstb::ToolsSimd::butterfly_f32_w64 (x);

	// Stage 1
	x1z = _z1 [3];
	x2z = _z2 [2];
	{
		const auto b0  = _ap2 [1] [0];
		const auto b1  = _ap2 [1] [1];
		const auto y1z = _z1 [4];
		const auto y2z = _z2 [3];
		const auto y   = b0 * (x - y2z) + b1 * (x1z - y1z) + x2z;
		_z1 [3] = x;
		_z2 [2] = x1z;
		x   = y;
		x1z = y1z;
		x2z = y2z;
	}
	{
		const auto b0  = _ap2 [2] [0];
		const auto b1  = _ap2 [2] [1];
		const auto y1z = _z1 [5];
		const auto y2z = _z2 [4];
		const auto y   = b0 * (x - y2z) + b1 * (x1z - y1z) + x2z;
		_z1 [4] = x;
		_z2 [3] = x1z;
		x   = y;
		x1z = y1z;
		x2z = y2z;
	}
	{
		const auto b0  = _ap2 [3] [0];
		const auto b1  = _ap2 [3] [1];
		const auto y1z = _z1 [6];
		const auto y2z = _z2 [5];
		const auto y   = b0 * (x - y2z) + b1 * (x1z - y1z) + x2z;
		_z1 [5] = x;
		_z2 [4] = x1z;
		x   = y;
		x1z = y1z;
		x2z = y2z;
	}
	_z2 [5] = x1z;
	_z1 [6] = x;
	x = fstb::ToolsSimd::deinterleave_f32_lo (x, x);

	// Stage 2
	x1z = _z1 [7];
	x2z = _z2 [6];
	{
		const auto b0  = _ap2 [4] [0];
		const auto b1  = _ap2 [4] [1];
		const auto y1z = _z1 [8];
		const auto y2z = _z2 [7];
		const auto y   = b0 * (x - y2z) + b1 * (x1z - y1z) + x2z;
		_z1 [7] = x;
		_z2 [6] = x1z;
		x   = y;
		x1z = y1z;
		x2z = y2z;
	}
	_z2 [7] = x1z;
	x_save = x;
	{
		const auto b0  = _ap1 [1];
		const auto y1z = _z1 [9];
		const auto y   = b0 * (x - y1z) + x1z;
		_z1 [8] = x;
		x   = y;
		x1z = y1z;
	}
	_z1 [9] = x;
	mask = fstb::ToolsSimd::cast_f32 (fstb::Vs32 (-1, -1, 0, 0));
	x    = fstb::ToolsSimd::select (mask, x_save, x);
	x = fstb::ToolsSimd::butterfly_f32_w64 (x);

	// Stage 3
	x1z = _z1 [10];
	x2z = _z2 [8];
	{
		const auto b0  = _ap2 [5] [0];
		const auto b1  = _ap2 [5] [1];
		const auto y1z = _z1 [11];
		const auto y2z = _z2 [9];
		const auto y   = b0 * (x - y2z) + b1 * (x1z - y1z) + x2z;
		_z1 [10] = x;
		_z2 [8] = x1z;
		x   = y;
		x1z = y1z;
		x2z = y2z;
	}
	_z2 [9] = x1z;
	_z1 [11] = x;

	return x;
}

fstb::Vf32	SplitterSimd::Filter0::process_sample (fstb::Vf32 x) noexcept
{
	auto x1z = _z1 [0];
	auto x2z = _z2 [0];
	fstb::Vf32 mask;
	fstb::Vf32 x_save;
	{
		const auto b0  = _ap2 [0] [0];
		const auto b1  = _ap2 [0] [1];
		const auto y1z = _z1 [1];
		const auto y2z = _z2 [1];
		const auto y   = b0 * (x - y2z) + b1 * (x1z - y1z) + x2z;
		_z1 [0] = x;
		_z2 [0] = x1z;
		x   = y;
		x1z = y1z;
		x2z = y2z;
	}
	_z2 [1] = x1z;
	x_save = x;
	{
		const auto b0  = _ap1 [0];
		const auto y1z = _z1 [2];
		const auto y   = b0 * (x - y1z) + x1z;
		_z1 [1] = x;
		x   = y;
		x1z = y1z;
	}
	_z1 [2] = x;
	mask = fstb::ToolsSimd::cast_f32 (fstb::Vs32 (-1, -1, 0, 0));
	x    = fstb::ToolsSimd::select (mask, x_save, x);
	x = fstb::ToolsSimd::butterfly_f32_w64 (x);

	return x;
}

void	SplitterSimd::set_band_ptr (float * const band_ptr_arr [8]) noexcept
{
	std::copy (band_ptr_arr, band_ptr_arr + 8, _out_ptr_arr);
}

void	SplitterSimd::offset_band_ptr (ptrdiff_t offset) noexcept
{
	std::for_each (
		std::begin (_out_ptr_arr), std::end (_out_ptr_arr),
		[offset] (float * &ptr) { ptr += offset; }
	);
}

void	SplitterSimd::process_sample (float x) noexcept
{
	const auto v0i = fstb::Vf32 (x);
	const auto v0o = _filter_0.process_sample (v0i);
	const auto v1i = fstb::ToolsSimd::deinterleave_f32_lo (v0o, v0o);
	const auto v1o = _filter_1.process_sample (v1i);
	const auto v2i = fstb::ToolsSimd::deinterleave_f32_hi (v0o, v0o);
	const auto v2o = _filter_2.process_sample (v2i);
	std::tie (
		*(_out_ptr_arr [0]),
		*(_out_ptr_arr [2]),
		*(_out_ptr_arr [1]),
		*(_out_ptr_arr [3])
	) = v1o.explode ();
	std::tie (
		*(_out_ptr_arr [4]),
		*(_out_ptr_arr [6]),
		*(_out_ptr_arr [5]),
		*(_out_ptr_arr [7])
	) = v2o.explode ();
}

} // namespace nzbl
} // namespace pi
} // namespace mfx



