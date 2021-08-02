// DECLARATIONS ///////////////////

// Code automatically generated.

#pragma once

#include "fstb/ToolsSimd.h"

namespace mfx {
namespace pi {
namespace nzbl {

class SplitterSimd
{
public:

	static constexpr int _nbr_bands = 8;
	static constexpr int _nbr_2p_0  = 1;
	static constexpr int _nbr_1p_0  = 0;
	static constexpr int _nbr_2p_1  = 1;
	static constexpr int _nbr_1p_1  = 1;
	inline void    set_band_ptr (float * const band_ptr_arr [8]) noexcept;
	inline constexpr int
	               get_nbr_bands () const noexcept { return _nbr_bands; }
	inline void    offset_band_ptr (ptrdiff_t offset) noexcept;
	void           set_splitter_coef (int split_idx, const float a0_arr [2], const float a1_arr [3]) noexcept;
	void           clear_buffers () noexcept;
	void           process_block (const float *src_ptr, int nbr_spl) noexcept;
	inline void    process_sample (float x) noexcept;

private:

	class Filter3
	{
	public:
		inline fstb::ToolsSimd::VectF32 process_sample (fstb::ToolsSimd::VectF32 x) noexcept;
		void set_apf_1p (int idx, int lane, float b0) noexcept;
		void set_apf_2p (int idx, int lane, float b0, float b1) noexcept;
		void clear_buffers () noexcept;
	private:
		fstb::ToolsSimd::VectF32 _ap1 [2] {};
		fstb::ToolsSimd::VectF32 _ap2 [6] [2] {};
		fstb::ToolsSimd::VectF32 _z1 [12] {};
		fstb::ToolsSimd::VectF32 _z2 [10] {};
	};

	class Filter0
	{
	public:
		inline fstb::ToolsSimd::VectF32 process_sample (fstb::ToolsSimd::VectF32 x) noexcept;
		void set_apf_1p (int idx, int lane, float b0) noexcept;
		void set_apf_2p (int idx, int lane, float b0, float b1) noexcept;
		void clear_buffers () noexcept;
	private:
		fstb::ToolsSimd::VectF32 _ap1 [1] {};
		fstb::ToolsSimd::VectF32 _ap2 [1] [2] {};
		fstb::ToolsSimd::VectF32 _z1 [3] {};
		fstb::ToolsSimd::VectF32 _z2 [2] {};
	};

	using Filter4 = Filter0;

	Filter3 _filter_0;
	Filter0 _filter_1;
	Filter4 _filter_2;
	float * _out_ptr_arr [8] {};
};

} // namespace nzbl
} // namespace pi
} // namespace mfx

#include "mfx/pi/nzbl/SplitterSimd.hpp"



