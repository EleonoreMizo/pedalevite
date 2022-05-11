/*****************************************************************************

        ToolsSimd.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/ArrayAlign.h"
#include "fstb/CpuId.h"
#include "fstb/ToolsSimd.h"

#if (fstb_ARCHI == fstb_ARCHI_X86)
#include <immintrin.h>
#endif // fstb_ARCHI

#include <cassert>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ToolsSimd::disable_denorm () noexcept
{
#if (fstb_ARCHI == fstb_ARCHI_X86)

	// Gets the MXCSR_MASK to know which features are supported
	uint32_t       mxcsr_mask = 0;
	fstb::CpuId    cpuid;
	if (cpuid._fxsr_flag)
	{
		ArrayAlign <uint8_t, 512, 16> fxsave_img;
		_fxsave (fxsave_img.data ());
		mxcsr_mask = *reinterpret_cast <const uint32_t *> (&fxsave_img [28]);
	}
	if (mxcsr_mask == 0)
	{
		mxcsr_mask = 0x0000FFBF;
	}

	// Sets denormal behaviour for SSE operations
	auto           reg_mxcsr = _mm_getcsr ();
	reg_mxcsr |= 1 << 15;   // FTZ - Flush To Zero
	reg_mxcsr |= 1 <<  6;   // DAZ - Denormals Are Zero
	// DAZ is not supported on all CPUs, we have to mask it
	reg_mxcsr &= mxcsr_mask;
	_mm_setcsr (reg_mxcsr);

	// On MacOS (not tested):
	// #include <cfenv>
	// #pragma STDC FENV_ACCESS ON
	// fesetenv (_FE_DFL_DISABLE_SSE_DENORMS_ENV);

#elif (fstb_ARCHI == fstb_ARCHI_ARM)
# if (fstb_WORD_SIZE == 64 && fstb_COMPILER == fstb_COMPILER_GCC) // Aarch64

	// https://developer.arm.com/documentation/ddi0595/2021-12/AArch64-Registers/FPCR--Floating-point-Control-Register
	uint64_t       fpcr_old = 0;
	asm volatile ("mrs %0, fpcr " : "=r" (fpcr_old));

	constexpr auto fz = uint64_t (1ULL << 24); // Flush denormal to zero

	const auto     fpcr_new = fpcr_old | fz;
	asm volatile ("msr fpcr, %0 " : : "r" (fpcr_new));

# endif
#endif
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/


#define fstb_ToolsSimd_INV4(x, y) \
	4.0f / (x +  y), 4.0f / (x +  1), 4.0f / (x +  2), 4.0f / (x +  3), \
	4.0f / (x +  4), 4.0f / (x +  5), 4.0f / (x +  6), 4.0f / (x +  7), \
	4.0f / (x +  8), 4.0f / (x +  9), 4.0f / (x + 10), 4.0f / (x + 11), \
	4.0f / (x + 12), 4.0f / (x + 13), 4.0f / (x + 14), 4.0f / (x + 15)

const float	ToolsSimd::_inv_table_4 [_inv_table_4_len] =
{
	fstb_ToolsSimd_INV4 (0x000, 1), fstb_ToolsSimd_INV4 (0x010, 0),
	fstb_ToolsSimd_INV4 (0x020, 0), fstb_ToolsSimd_INV4 (0x030, 0),
	fstb_ToolsSimd_INV4 (0x040, 0), fstb_ToolsSimd_INV4 (0x050, 0),
	fstb_ToolsSimd_INV4 (0x060, 0), fstb_ToolsSimd_INV4 (0x070, 0),
	fstb_ToolsSimd_INV4 (0x080, 0), fstb_ToolsSimd_INV4 (0x090, 0),
	fstb_ToolsSimd_INV4 (0x0A0, 0), fstb_ToolsSimd_INV4 (0x0B0, 0),
	fstb_ToolsSimd_INV4 (0x0C0, 0), fstb_ToolsSimd_INV4 (0x0D0, 0),
	fstb_ToolsSimd_INV4 (0x0E0, 0), fstb_ToolsSimd_INV4 (0x0F0, 0),
	fstb_ToolsSimd_INV4 (0x100, 0), fstb_ToolsSimd_INV4 (0x110, 0),
	fstb_ToolsSimd_INV4 (0x120, 0), fstb_ToolsSimd_INV4 (0x130, 0),
	fstb_ToolsSimd_INV4 (0x140, 0), fstb_ToolsSimd_INV4 (0x150, 0),
	fstb_ToolsSimd_INV4 (0x160, 0), fstb_ToolsSimd_INV4 (0x170, 0),
	fstb_ToolsSimd_INV4 (0x180, 0), fstb_ToolsSimd_INV4 (0x190, 0),
	fstb_ToolsSimd_INV4 (0x1A0, 0), fstb_ToolsSimd_INV4 (0x1B0, 0),
	fstb_ToolsSimd_INV4 (0x1C0, 0), fstb_ToolsSimd_INV4 (0x1D0, 0),
	fstb_ToolsSimd_INV4 (0x1E0, 0), fstb_ToolsSimd_INV4 (0x1F0, 0),
	fstb_ToolsSimd_INV4 (0x200, 0), fstb_ToolsSimd_INV4 (0x210, 0),
	fstb_ToolsSimd_INV4 (0x220, 0), fstb_ToolsSimd_INV4 (0x230, 0),
	fstb_ToolsSimd_INV4 (0x240, 0), fstb_ToolsSimd_INV4 (0x250, 0),
	fstb_ToolsSimd_INV4 (0x260, 0), fstb_ToolsSimd_INV4 (0x270, 0),
	fstb_ToolsSimd_INV4 (0x280, 0), fstb_ToolsSimd_INV4 (0x290, 0),
	fstb_ToolsSimd_INV4 (0x2A0, 0), fstb_ToolsSimd_INV4 (0x2B0, 0),
	fstb_ToolsSimd_INV4 (0x2C0, 0), fstb_ToolsSimd_INV4 (0x2D0, 0),
	fstb_ToolsSimd_INV4 (0x2E0, 0), fstb_ToolsSimd_INV4 (0x2F0, 0),
	fstb_ToolsSimd_INV4 (0x300, 0), fstb_ToolsSimd_INV4 (0x310, 0),
	fstb_ToolsSimd_INV4 (0x320, 0), fstb_ToolsSimd_INV4 (0x330, 0),
	fstb_ToolsSimd_INV4 (0x340, 0), fstb_ToolsSimd_INV4 (0x350, 0),
	fstb_ToolsSimd_INV4 (0x360, 0), fstb_ToolsSimd_INV4 (0x370, 0),
	fstb_ToolsSimd_INV4 (0x380, 0), fstb_ToolsSimd_INV4 (0x390, 0),
	fstb_ToolsSimd_INV4 (0x3A0, 0), fstb_ToolsSimd_INV4 (0x3B0, 0),
	fstb_ToolsSimd_INV4 (0x3C0, 0), fstb_ToolsSimd_INV4 (0x3D0, 0),
	fstb_ToolsSimd_INV4 (0x3E0, 0), fstb_ToolsSimd_INV4 (0x3F0, 0),
	4.0f / 0x400
};

#undef fstb_ToolsSimd_INV4



}  // namespace fstb



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
