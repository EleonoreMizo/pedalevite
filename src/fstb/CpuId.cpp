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

#if defined (_MSC_VER)
	#include <intrin.h>
#endif

#include <cassert>



namespace fstb
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



CpuId::CpuId ()
{
#if (fstb_ARCHI == fstb_ARCHI_X86)

	unsigned int   eax;
	unsigned int   ebx;
	unsigned int   ecx;
	unsigned int   edx;

	// Basic features
	call_cpuid (0x00000001, eax, ebx, ecx, edx);

	_mmx_flag     = ((edx & (1L << 23)) != 0);
	_sse_flag     = ((edx & (1L << 25)) != 0);
	_sse2_flag    = ((edx & (1L << 26)) != 0);
	_sse3_flag    = ((ecx & (1L <<  0)) != 0);
	_ssse3_flag   = ((ecx & (1L <<  9)) != 0);
	_cx16_flag    = ((ecx & (1L << 13)) != 0);
	_fma3_flag    = ((ecx & (1L << 16)) != 0);
	_sse41_flag   = ((ecx & (1L << 19)) != 0);
	_sse42_flag   = ((ecx & (1L << 20)) != 0);
	_avx_flag     = ((ecx & (1L << 28)) != 0);
	_f16c_flag    = ((ecx & (1L << 29)) != 0);

	call_cpuid (0x00000007, eax, ebx, ecx, edx);
	_avx2_flag    = ((ebx & (1L <<  5)) != 0);
	_avx512f_flag = ((ebx & (1L << 16)) != 0);

	// Extended features
	call_cpuid (0x80000000, eax, ebx, ecx, edx);
	if (eax >= 0x80000001)
	{
		call_cpuid (0x80000001, eax, ebx, ecx, edx);
		_isse_flag    = ((edx & (1L << 22)) != 0) || _sse_flag;
		_sse4a_flag   = ((ecx & (1L <<  6)) != 0);
		_fma4_flag    = ((ecx & (1L << 16)) != 0);
	}

#endif
}



#if (fstb_ARCHI == fstb_ARCHI_X86)

void	CpuId::call_cpuid (unsigned int fnc_nbr, unsigned int &v_eax, unsigned int &v_ebx, unsigned int &v_ecx, unsigned int &v_edx)
{
#if defined (__GNUC__)
	
	long           r_eax;
	long           r_ebx;
	long           r_ecx;
	long           r_edx;

	#if defined (__x86_64__)

	__asm__ (
	   "push %%rbx      \n\t" /* save %rbx */
		"cpuid           \n\t"
		"mov %%rbx, %1   \n\t" /* save what cpuid just put in %rbx */
		"pop %%rbx       \n\t" /* restore the old %rbx */
	  : "=a"(r_eax), "=r"(r_ebx), "=c"(r_ecx), "=d"(r_edx)
	  : "a"(fnc_nbr)
	  : "cc");

	#else

	__asm__ (
		"pushl %%ebx      \n\t" /* save %ebx */
		"cpuid            \n\t"
		"movl %%ebx, %1   \n\t" /* save what cpuid just put in %ebx */
		"popl %%ebx       \n\t" /* restore the old %ebx */
	  : "=a"(r_eax), "=r"(r_ebx), "=c"(r_ecx), "=d"(r_edx)
	  : "a"(fnc_nbr)
	  : "cc");

	#endif

	v_eax = r_eax;
	v_ebx = r_ebx;
	v_ecx = r_ecx;
	v_edx = r_edx;

#elif (_MSC_VER)

	int            cpu_info [4];
	__cpuid (cpu_info, fnc_nbr);
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
