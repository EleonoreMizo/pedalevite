/*****************************************************************************

        TestAllClassesSpeed.hpp
        Author: Laurent de Soras, 2005

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (hiir_test_TestAllClassesSpeed_CURRENT_CODEHEADER)
	#error Recursive inclusion of TestAllClassesSpeed code header.
#endif
#define hiir_test_TestAllClassesSpeed_CURRENT_CODEHEADER

#if ! defined (hiir_test_TestAllClassesSpeed_CODEHEADER_INCLUDED)
#define hiir_test_TestAllClassesSpeed_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/test/AlignedObject.h"
#include "hiir/test/conf.h"
#include "hiir/test/CpuId.h"
#include "hiir/test/SpeedTester.h"
#include "hiir/Downsampler2xF64Fpu.h"
#include "hiir/Downsampler2xFpu.h"
#include "hiir/HalfBandF64Fpu.h"
#include "hiir/HalfBandFpu.h"
#include "hiir/PhaseHalfPiF64Fpu.h"
#include "hiir/PhaseHalfPiFpu.h"
#include "hiir/Upsampler2xF64Fpu.h"
#include "hiir/Upsampler2xFpu.h"

#if defined (hiir_test_3DNOW)
#include "hiir/Downsampler2x3dnow.h"
#include "hiir/PhaseHalfPi3dnow.h"
#include "hiir/Upsampler2x3dnow.h"
#endif

#if defined (hiir_test_SSE)
#include "hiir/Downsampler2x4Sse.h"
#include "hiir/Downsampler2xSse.h"
#include "hiir/Downsampler2xSseOld.h"
#include "hiir/HalfBand4Sse.h"
#include "hiir/HalfBandSse.h"
#include "hiir/PhaseHalfPi4Sse.h"
#include "hiir/PhaseHalfPiSse.h"
#include "hiir/Upsampler2x4Sse.h"
#include "hiir/Upsampler2xSse.h"
#include "hiir/Upsampler2xSseOld.h"
#endif

#if defined (hiir_test_SSE2)
#include "hiir/Downsampler2x2F64Sse2.h"
#include "hiir/Downsampler2xF64Sse2.h"
#include "hiir/HalfBand2F64Sse2.h"
#include "hiir/HalfBandF64Sse2.h"
#include "hiir/PhaseHalfPi2F64Sse2.h"
#include "hiir/PhaseHalfPiF64Sse2.h"
#include "hiir/Upsampler2x2F64Sse2.h"
#include "hiir/Upsampler2xF64Sse2.h"
#endif

#if defined (hiir_test_AVX)
#include "hiir/Downsampler2x4F64Avx.h"
#include "hiir/Downsampler2x8Avx.h"
#include "hiir/HalfBand4F64Avx.h"
#include "hiir/HalfBand8Avx.h"
#include "hiir/PhaseHalfPi4F64Avx.h"
#include "hiir/PhaseHalfPi8Avx.h"
#include "hiir/Upsampler2x4F64Avx.h"
#include "hiir/Upsampler2x8Avx.h"
#endif

#if defined (hiir_test_AVX512)
#include "hiir/Downsampler2x16Avx512.h"
#include "hiir/Downsampler2x8F64Avx512.h"
#include "hiir/HalfBand16Avx512.h"
#include "hiir/HalfBand8F64Avx512.h"
#include "hiir/PhaseHalfPi16Avx512.h"
#include "hiir/PhaseHalfPi8F64Avx512.h"
#include "hiir/Upsampler2x16Avx512.h"
#include "hiir/Upsampler2x8F64Avx512.h"
#endif

#if defined (hiir_test_NEON)
#include "hiir/Downsampler2x4Neon.h"
#include "hiir/Downsampler2xNeon.h"
#include "hiir/Downsampler2xNeonOld.h"
#include "hiir/HalfBand4Neon.h"
#include "hiir/HalfBandNeon.h"
#include "hiir/PhaseHalfPi4Neon.h"
#include "hiir/PhaseHalfPiNeon.h"
#include "hiir/Upsampler2x4Neon.h"
#include "hiir/Upsampler2xNeon.h"
#include "hiir/Upsampler2xNeonOld.h"
#endif

#include <cstdio>



namespace hiir
{
namespace test
{



template <class TO>
class AuxProc11
{
public:
	typedef TO TestedObject;
	static hiir_FORCEINLINE void	process_block (SpeedTesterBase <TO> &tester, TO &tested_obj) noexcept
	{
		tested_obj.process_block (
			&tester._dest_arr [0] [0],
			&tester._src_arr [0] [0],
			tester._block_len / TO::_nbr_chn
		);
	}
};

template <class TO>
class AuxProc12
{
public:
	typedef TO TestedObject;
	static hiir_FORCEINLINE void	process_block (SpeedTesterBase <TO> &tester, TO &tested_obj) noexcept
	{
		tested_obj.process_block (
			&tester._dest_arr [0] [0],
			&tester._dest_arr [1] [0],
			&tester._src_arr [0] [0],
			tester._block_len / TO::_nbr_chn
		);
	}
};

template <class TO>
class AuxProc12Split
{
public:
	typedef TO TestedObject;
	static hiir_FORCEINLINE void	process_block (SpeedTesterBase <TO> &tester, TO &tested_obj) noexcept
	{
		tested_obj.process_block_split (
			&tester._dest_arr [0] [0],
			&tester._dest_arr [1] [0],
			&tester._src_arr [0] [0],
			tester._block_len / TO::_nbr_chn
		);
	}
};



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int NC>
void	TestAllClassesSpeed <NC>::perform_test () noexcept
{
	CpuId          cpu;

	// Half-band
	{
		SpeedTester <AuxProc11 <HalfBandFpu <NBR_COEFS> > > st;
		st.perform_test ("HalfBandFpu", "process_block");
	}
#if defined (hiir_test_SSE)
	if (cpu._sse_flag)
	{
		typedef	SpeedTester <AuxProc11 <HalfBandSse <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("HalfBandSse", "process_block");
	}
	if (cpu._sse_flag)
	{
		typedef	SpeedTester <AuxProc11 <HalfBand4Sse <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("HalfBand4Sse", "process_block");
	}
#endif
#if defined (hiir_test_AVX)
	if (cpu._avx_flag)
	{
		typedef	SpeedTester <AuxProc11 <HalfBand8Avx <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("HalfBand8Avx", "process_block");
	}
#endif
#if defined (hiir_test_AVX512)
	if (cpu._avx512f_flag)
	{
		typedef	SpeedTester <AuxProc11 <HalfBand16Avx512 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("HalfBand16Avx512", "process_block");
	}
#endif
#if defined (hiir_test_NEON)
	if (cpu._neon_flag)
	{
		typedef	SpeedTester <AuxProc11 <HalfBandNeon <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("HalfBandNeon", "process_block");
	}
	if (cpu._neon_flag)
	{
		typedef	SpeedTester <AuxProc11 <HalfBand4Neon <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("HalfBand4Neon", "process_block");
	}
#endif
	{
		SpeedTester <AuxProc11 <HalfBandF64Fpu <NBR_COEFS> > > st;
		st.perform_test ("HalfBandF64Fpu", "process_block");
	}
#if defined (hiir_test_SSE2)
	if (cpu._sse2_flag)
	{
		typedef	SpeedTester <AuxProc11 <HalfBandF64Sse2 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("HalfBandF64Sse2", "process_block");
	}
	if (cpu._sse2_flag)
	{
		typedef	SpeedTester <AuxProc11 <HalfBand2F64Sse2 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("HalfBand2F64Sse2", "process_block");
	}
#endif
#if defined (hiir_test_AVX)
	if (cpu._avx_flag)
	{
		typedef	SpeedTester <AuxProc11 <HalfBand4F64Avx <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("HalfBand4F64Avx", "process_block");
	}
#endif
#if defined (hiir_test_AVX512)
	if (cpu._avx512f_flag)
	{
		typedef	SpeedTester <AuxProc11 <HalfBand8F64Avx512 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("HalfBand8F64Avx512", "process_block");
	}
#endif

	// Half-band, split
	{
		SpeedTester <AuxProc12Split <HalfBandFpu <NBR_COEFS> > > st;
		st.perform_test ("HalfBandFpu", "process_block_split");
	}
#if defined (hiir_test_SSE)
	if (cpu._sse_flag)
	{
		typedef	SpeedTester <AuxProc12Split <HalfBandSse <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("HalfBandSse", "process_block_split");
	}
	if (cpu._sse_flag)
	{
		typedef	SpeedTester <AuxProc12Split <HalfBand4Sse <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("HalfBand4Sse", "process_block_split");
	}
#endif
#if defined (hiir_test_AVX)
	if (cpu._avx_flag)
	{
		typedef	SpeedTester <AuxProc12Split <HalfBand8Avx <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("HalfBand8Avx", "process_block_split");
	}
#endif
#if defined (hiir_test_AVX512)
	if (cpu._avx512f_flag)
	{
		typedef	SpeedTester <AuxProc12Split <HalfBand16Avx512 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("HalfBand16Avx512", "process_block_split");
	}
#endif
#if defined (hiir_test_NEON)
	if (cpu._neon_flag)
	{
		typedef	SpeedTester <AuxProc12Split <HalfBandNeon <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("HalfBandNeon", "process_block_split");
	}
	if (cpu._neon_flag)
	{
		typedef	SpeedTester <AuxProc12Split <HalfBand4Neon <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("HalfBand4Neon", "process_block_split");
	}
#endif
	{
		SpeedTester <AuxProc11 <HalfBandF64Fpu <NBR_COEFS> > > st;
		st.perform_test ("HalfBandF64Fpu", "process_block_split");
	}
#if defined (hiir_test_SSE2)
	if (cpu._sse2_flag)
	{
		typedef	SpeedTester <AuxProc12Split <HalfBandF64Sse2 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("HalfBandF64Sse2", "process_block_split");
	}
	if (cpu._sse2_flag)
	{
		typedef	SpeedTester <AuxProc12Split <HalfBand2F64Sse2 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("HalfBand2F64Sse2", "process_block_split");
	}
#endif
#if defined (hiir_test_AVX)
	if (cpu._avx_flag)
	{
		typedef	SpeedTester <AuxProc12Split <HalfBand4F64Avx <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("HalfBand4F64Avx", "process_block_split");
	}
#endif
#if defined (hiir_test_AVX512)
	if (cpu._avx512f_flag)
	{
		typedef	SpeedTester <AuxProc12Split <HalfBand8F64Avx512 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("HalfBand8F64Avx512", "process_block_split");
	}
#endif

	// Downsampler
	{
		SpeedTester <AuxProc11 <Downsampler2xFpu <NBR_COEFS> > > st;
		st.perform_test ("Downsampler2xFpu", "process_block");
	}
#if defined (hiir_test_3DNOW)
	if (cpu._3dnow_flag)
	{
		SpeedTester <AuxProc11 <Downsampler2x3dnow <NBR_COEFS> > > st;
		st.perform_test ("Downsampler2x3dnow", "process_block");
	}
#endif
#if defined (hiir_test_SSE)
	if (cpu._sse_flag)
	{
		typedef	SpeedTester <AuxProc11 <Downsampler2xSseOld <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2xSseOld", "process_block");
	}
	if (cpu._sse_flag)
	{
		typedef	SpeedTester <AuxProc11 <Downsampler2xSse <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2xSse", "process_block");
	}
	if (cpu._sse_flag)
	{
		typedef	SpeedTester <AuxProc11 <Downsampler2x4Sse <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2x4Sse", "process_block");
	}
#endif
#if defined (hiir_test_AVX)
	if (cpu._avx_flag)
	{
		typedef	SpeedTester <AuxProc11 <Downsampler2x8Avx <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2x8Avx", "process_block");
	}
#endif
#if defined (hiir_test_AVX512)
	if (cpu._avx512f_flag)
	{
		typedef	SpeedTester <AuxProc11 <Downsampler2x16Avx512 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2x16Avx512", "process_block");
	}
#endif
#if defined (hiir_test_NEON)
	if (cpu._neon_flag)
	{
		typedef	SpeedTester <AuxProc11 <Downsampler2xNeonOld <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2xNeonOld", "process_block");
	}
	if (cpu._neon_flag)
	{
		typedef	SpeedTester <AuxProc11 <Downsampler2xNeon <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2xNeon", "process_block");
	}
	if (cpu._neon_flag)
	{
		typedef	SpeedTester <AuxProc11 <Downsampler2x4Neon <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2x4Neon", "process_block");
	}
#endif
	{
		SpeedTester <AuxProc11 <Downsampler2xF64Fpu <NBR_COEFS> > > st;
		st.perform_test ("Downsampler2xF64Fpu", "process_block");
	}
#if defined (hiir_test_SSE2)
	if (cpu._sse2_flag)
	{
		typedef	SpeedTester <AuxProc11 <Downsampler2xF64Sse2 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2xF64Sse2", "process_block");
	}
	if (cpu._sse2_flag)
	{
		typedef	SpeedTester <AuxProc11 <Downsampler2x2F64Sse2 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2x2F64Sse2", "process_block");
	}
#endif
#if defined (hiir_test_AVX)
	if (cpu._avx_flag)
	{
		typedef	SpeedTester <AuxProc11 <Downsampler2x4F64Avx <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2x4F64Avx", "process_block");
	}
#endif
#if defined (hiir_test_AVX512)
	if (cpu._avx512f_flag)
	{
		typedef	SpeedTester <AuxProc11 <Downsampler2x8F64Avx512 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2x8F64Avx512", "process_block");
	}
#endif

	// Downsampler, split
	{
		SpeedTester <AuxProc12Split <Downsampler2xFpu <NBR_COEFS> > > st;
		st.perform_test ("Downsampler2xFpu", "process_block_split");
	}
#if defined (hiir_test_3DNOW)
	if (cpu._3dnow_flag)
	{
		SpeedTester <AuxProc12Split <Downsampler2x3dnow <NBR_COEFS> > > st;
		st.perform_test ("Downsampler2x3dnow", "process_block_split");
	}
#endif
#if defined (hiir_test_SSE)
	if (cpu._sse_flag)
	{
		typedef	SpeedTester <AuxProc12Split <Downsampler2xSseOld <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2xSseOld", "process_block_split");
	}
	if (cpu._sse_flag)
	{
		typedef	SpeedTester <AuxProc12Split <Downsampler2xSse <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2xSse", "process_block_split");
	}
	if (cpu._sse_flag)
	{
		typedef	SpeedTester <AuxProc12Split <Downsampler2x4Sse <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2x4Sse", "process_block_split");
	}
#endif
#if defined (hiir_test_AVX)
	if (cpu._avx_flag)
	{
		typedef	SpeedTester <AuxProc12Split <Downsampler2x8Avx <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2x8Avx", "process_block_split");
	}
#endif
#if defined (hiir_test_AVX512)
	if (cpu._avx512f_flag)
	{
		typedef	SpeedTester <AuxProc12Split <Downsampler2x16Avx512 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2x16Avx512", "process_block_split");
	}
#endif
#if defined (hiir_test_NEON)
	if (cpu._neon_flag)
	{
		typedef	SpeedTester <AuxProc12Split <Downsampler2xNeonOld <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2xNeonOld", "process_block_split");
	}
	if (cpu._neon_flag)
	{
		typedef	SpeedTester <AuxProc12Split <Downsampler2xNeon <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2xNeon", "process_block_split");
	}
	if (cpu._neon_flag)
	{
		typedef	SpeedTester <AuxProc12Split <Downsampler2x4Neon <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2x4Neon", "process_block_split");
	}
#endif
	{
		SpeedTester <AuxProc12Split <Downsampler2xF64Fpu <NBR_COEFS> > > st;
		st.perform_test ("Downsampler2xF64Fpu", "process_block_split");
	}
#if defined (hiir_test_SSE2)
	if (cpu._sse2_flag)
	{
		typedef	SpeedTester <AuxProc12Split <Downsampler2xF64Sse2 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2xF64Sse2", "process_block_split");
	}
	if (cpu._sse2_flag)
	{
		typedef	SpeedTester <AuxProc12Split <Downsampler2x2F64Sse2 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2x2F64Sse2", "process_block_split");
	}
#endif
#if defined (hiir_test_AVX)
	if (cpu._avx_flag)
	{
		typedef	SpeedTester <AuxProc12Split <Downsampler2x4F64Avx <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2x4F64Avx", "process_block_split");
	}
#endif
#if defined (hiir_test_AVX512)
	if (cpu._avx512f_flag)
	{
		typedef	SpeedTester <AuxProc12Split <Downsampler2x8F64Avx512 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Downsampler2x8F64Avx512", "process_block_split");
	}
#endif

	// Upsampler
	{
		SpeedTester <AuxProc11 <Upsampler2xFpu <NBR_COEFS> > > st;
		st.perform_test ("Upsampler2xFpu", "process_block");
	}
#if defined (hiir_test_3DNOW)
	if (cpu._3dnow_flag)
	{
		SpeedTester <AuxProc11 <Upsampler2x3dnow <NBR_COEFS> > > st;
		st.perform_test ("Upsampler2x3dnow", "process_block");
	}
#endif
#if defined (hiir_test_SSE)
	if (cpu._sse_flag)
	{
		typedef	SpeedTester <AuxProc11 <Upsampler2xSseOld <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Upsampler2xSseOld", "process_block");
	}
	if (cpu._sse_flag)
	{
		typedef	SpeedTester <AuxProc11 <Upsampler2xSse <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Upsampler2xSse", "process_block");
	}
	if (cpu._sse_flag)
	{
		typedef	SpeedTester <AuxProc11 <Upsampler2x4Sse <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Upsampler2x4Sse", "process_block");
	}
#endif
#if defined (hiir_test_AVX)
	if (cpu._avx_flag)
	{
		typedef	SpeedTester <AuxProc11 <Upsampler2x8Avx <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Upsampler2x8Avx", "process_block");
	}
#endif
#if defined (hiir_test_AVX512)
	if (cpu._avx512f_flag)
	{
		typedef	SpeedTester <AuxProc11 <Upsampler2x16Avx512 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Upsampler2x16Avx512", "process_block");
	}
#endif
#if defined (hiir_test_NEON)
	if (cpu._neon_flag)
	{
		typedef	SpeedTester <AuxProc11 <Upsampler2xNeonOld <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Upsampler2xNeonOld", "process_block");
	}
	if (cpu._neon_flag)
	{
		typedef	SpeedTester <AuxProc11 <Upsampler2xNeon <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Upsampler2xNeon", "process_block");
	}
	if (cpu._neon_flag)
	{
		typedef	SpeedTester <AuxProc11 <Upsampler2x4Neon <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Upsampler2x4Neon", "process_block");
	}
#endif
	{
		SpeedTester <AuxProc11 <Upsampler2xF64Fpu <NBR_COEFS> > > st;
		st.perform_test ("Upsampler2xF64Fpu", "process_block");
	}
#if defined (hiir_test_SSE2)
	if (cpu._sse2_flag)
	{
		typedef	SpeedTester <AuxProc11 <Upsampler2xF64Sse2 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Upsampler2xF64Sse2", "process_block");
	}
	if (cpu._sse2_flag)
	{
		typedef	SpeedTester <AuxProc11 <Upsampler2x2F64Sse2 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Upsampler2x2F64Sse2", "process_block");
	}
#endif
#if defined (hiir_test_AVX)
	if (cpu._avx_flag)
	{
		typedef	SpeedTester <AuxProc11 <Upsampler2x4F64Avx <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Upsampler2x4F64Avx", "process_block");
	}
#endif
#if defined (hiir_test_AVX512)
	if (cpu._avx512f_flag)
	{
		typedef	SpeedTester <AuxProc11 <Upsampler2x8F64Avx512 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("Upsampler2x8F64Avx512", "process_block");
	}
#endif

	// PhaseHalfPi
	{
		SpeedTester <AuxProc12 <PhaseHalfPiFpu <NBR_COEFS> > > st;
		st.perform_test ("PhaseHalfPiFpu", "process_block");
	}
#if defined (hiir_test_3DNOW)
	if (cpu._3dnow_flag)
	{
		SpeedTester <AuxProc12 <PhaseHalfPi3dnow <NBR_COEFS> > > st;
		st.perform_test ("PhaseHalfPi3dnow", "process_block");
	}
#endif
#if defined (hiir_test_SSE)
	if (cpu._sse_flag)
	{
		typedef	SpeedTester <AuxProc12 <PhaseHalfPiSse <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("PhaseHalfPiSse", "process_block");
	}
	if (cpu._sse_flag)
	{
		typedef	SpeedTester <AuxProc12 <PhaseHalfPi4Sse <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("PhaseHalfPi4Sse", "process_block");
	}
#endif
#if defined (hiir_test_AVX)
	if (cpu._avx_flag)
	{
		typedef	SpeedTester <AuxProc12 <PhaseHalfPi8Avx <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("PhaseHalfPi8Avx", "process_block");
	}
#endif
#if defined (hiir_test_AVX512)
	if (cpu._avx512f_flag)
	{
		typedef	SpeedTester <AuxProc12 <PhaseHalfPi16Avx512 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("PhaseHalfPi16Avx512", "process_block");
	}
#endif
#if defined (hiir_test_NEON)
	if (cpu._neon_flag)
	{
		typedef	SpeedTester <AuxProc12 <PhaseHalfPiNeon <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("PhaseHalfPiNeon", "process_block");
	}
	if (cpu._neon_flag)
	{
		typedef	SpeedTester <AuxProc12 <PhaseHalfPi4Neon <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("PhaseHalfPi4Neon", "process_block");
	}
#endif
	{
		SpeedTester <AuxProc12 <PhaseHalfPiF64Fpu <NBR_COEFS> > > st;
		st.perform_test ("PhaseHalfPiF64Fpu", "process_block");
	}
#if defined (hiir_test_SSE2)
	if (cpu._sse2_flag)
	{
		typedef	SpeedTester <AuxProc12 <PhaseHalfPiF64Sse2 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("PhaseHalfPiF64Sse2", "process_block");
	}
	if (cpu._sse2_flag)
	{
		typedef	SpeedTester <AuxProc12 <PhaseHalfPi2F64Sse2 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("PhaseHalfPi2F64Sse2", "process_block");
	}
#endif
#if defined (hiir_test_AVX)
	if (cpu._avx_flag)
	{
		typedef	SpeedTester <AuxProc12 <PhaseHalfPi4F64Avx <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("PhaseHalfPi4F64Avx", "process_block");
	}
#endif
#if defined (hiir_test_AVX512)
	if (cpu._avx512f_flag)
	{
		typedef	SpeedTester <AuxProc12 <PhaseHalfPi8F64Avx512 <NBR_COEFS> > > TestType;
		AlignedObject <TestType>   container;
		TestType &     st = container.use ();
		st.perform_test ("PhaseHalfPi8F64Avx512", "process_block");
	}
#endif

	printf ("\n");
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace test
}  // namespace hiir



#endif   // hiir_test_TestAllClassesSpeed_CODEHEADER_INCLUDED

#undef hiir_test_TestAllClassesSpeed_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
