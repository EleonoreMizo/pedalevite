/*****************************************************************************

        CpuId.h
        Author: Laurent de Soras, 2012

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_test_CpuId_HEADER_INCLUDED)
#define	hiir_test_CpuId_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/def.h"



namespace hiir
{
namespace test
{



class CpuId
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               CpuId ();
	               CpuId (const CpuId &other)      = default;
	               CpuId (CpuId &&other)           = default;
	               ~CpuId ()                       = default;

	CpuId &        operator = (const CpuId &other) = default;
	CpuId &        operator = (CpuId &&other)      = default;

#if hiir_ARCHI == hiir_ARCHI_X86
	static void		call_cpuid (unsigned int fnc_nbr, unsigned int subfnc_nbr, unsigned int &v_eax, unsigned int &v_ebx, unsigned int &v_ecx, unsigned int &v_edx);
#endif

	// x86
	bool           _mmx_flag     = false;
	bool           _3dnow_flag   = false;
	bool           _isse_flag    = false;
	bool           _sse_flag     = false;
	bool           _sse2_flag    = false;
	bool           _sse3_flag    = false;
	bool           _ssse3_flag   = false;
	bool           _sse41_flag   = false;
	bool           _sse42_flag   = false;
	bool           _sse4a_flag   = false;
	bool           _fma3_flag    = false;
	bool           _fma4_flag    = false;
	bool           _avx_flag     = false;
	bool           _avx2_flag    = false;
	bool           _avx512f_flag = false;
	bool           _f16c_flag    = false;  // Half-precision FP
	bool           _cx16_flag    = false;  // CMPXCHG16B
	bool           _abm_flag     = false;  // POPCNT + LZCNT
	bool           _bmi1_flag    = false;  // Bit Manipulation Instruction Set
	bool           _bmi2_flag    = false;

	// ARM
	bool           _neon_flag    = false;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const CpuId &other) const = delete;
	bool           operator != (const CpuId &other) const = delete;

};	// class CpuId



}	// namespace test
}	// namespace hiir



//#include "hiir/test/CpuId.hpp"



#endif	// hiir_test_CpuId_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
