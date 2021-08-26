/*****************************************************************************

        CpuId.cpp
        Author: Laurent de Soras, 2012

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

#include "fstb/CpuId.h"

#if fstb_ARCHI == fstb_ARCHI_X86
	#if defined (__GNUC__)
		#include <cpuid.h>
	#elif defined (_MSC_VER)
		#include <intrin.h>
	#endif
#endif

#include <cassert>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



CpuId::CpuId ()
{
#if fstb_ARCHI == fstb_ARCHI_X86

	unsigned int   eax;
	unsigned int   ebx;
	unsigned int   ecx;
	unsigned int   edx;

	// Highest function available
	call_cpuid (0x00000000, 0, eax, ebx, ecx, edx);
	const unsigned int   hf_basic = eax;

	// Processor Info and Feature Bits
	call_cpuid (0x00000001, 0, eax, ebx, ecx, edx);

	_mmx_flag     = ((edx & (1L << 23)) != 0);
	_fxsr_flag    = ((edx & (1L << 24)) != 0);
	_sse_flag     = ((edx & (1L << 25)) != 0);
	_sse2_flag    = ((edx & (1L << 26)) != 0);
	_sse3_flag    = ((ecx & (1L <<  0)) != 0);
	_ssse3_flag   = ((ecx & (1L <<  9)) != 0);
	_cx16_flag    = ((ecx & (1L << 13)) != 0);
	_fma3_flag    = ((ecx & (1L << 16)) != 0);
	_sse41_flag   = ((ecx & (1L << 19)) != 0);
	_sse42_flag   = ((ecx & (1L << 20)) != 0);
	_abm_flag     = ((ecx & (1L << 23)) != 0);
	_avx_flag     = ((ecx & (1L << 28)) != 0);
	_f16c_flag    = ((ecx & (1L << 29)) != 0);

	if (hf_basic >= 0x00000007)
	{
		// Extended Features
		call_cpuid (0x00000007, 0, eax, ebx, ecx, edx);
		_bmi1_flag    = ((ebx & (1L <<  3)) != 0);
		_avx2_flag    = ((ebx & (1L <<  5)) != 0);
		_bmi2_flag    = ((ebx & (1L <<  8)) != 0);
		_avx512f_flag = ((ebx & (1L << 16)) != 0);
	}

	// Extended Processor Info and Feature Bits
	call_cpuid (0x80000000, 0, eax, ebx, ecx, edx);
	const unsigned int   hf_ext = eax;
	if (hf_ext >= 0x80000001)
	{
		call_cpuid (0x80000001, 0, eax, ebx, ecx, edx);
		_isse_flag    = ((edx & (1L << 22)) != 0) || _sse_flag;
		_sse4a_flag   = ((ecx & (1L <<  6)) != 0);
		_fma4_flag    = ((ecx & (1L << 16)) != 0);
		_3dnow_flag   = ((ecx & (1L << 31)) != 0);
	}

#endif
}



#if fstb_ARCHI == fstb_ARCHI_X86

void	CpuId::call_cpuid (unsigned int fnc_nbr, unsigned int subfnc_nbr, unsigned int &v_eax, unsigned int &v_ebx, unsigned int &v_ecx, unsigned int &v_edx)
{
#if defined (__GNUC__)
	
	#if defined (__x86_64__)
	__cpuid_count (fnc_nbr, subfnc_nbr, v_eax, v_ebx, v_ecx, v_edx);
	#else
	fstb::unused (subfnc_nbr);
	__cpuid (fnc_nbr, v_eax, v_ebx, v_ecx, v_edx);
	#endif

#elif defined (_MSC_VER)

	int            cpu_info [4];
	__cpuidex (cpu_info, fnc_nbr, subfnc_nbr);
	v_eax = cpu_info [0];
	v_ebx = cpu_info [1];
	v_ecx = cpu_info [2];
	v_edx = cpu_info [3];

#else

	#pragma error "Unsupported compiler"

#endif
}

#endif



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace fstb



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
