/*****************************************************************************

        TestAllClassesFnc.hpp
        Author: Laurent de Soras, 2005

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (hiir_test_TestAllClassesFnc_CURRENT_CODEHEADER)
	#error Recursive inclusion of TestAllClassesFnc code header.
#endif
#define hiir_test_TestAllClassesFnc_CURRENT_CODEHEADER

#if ! defined (hiir_test_TestAllClassesFnc_CODEHEADER_INCLUDED)
#define hiir_test_TestAllClassesFnc_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/test/AlignedObject.h"
#include "hiir/test/conf.h"
#include "hiir/test/CpuId.h"
#include "hiir/test/SweepingSine.h"
#include "hiir/test/TestDownsampler2x.h"
#include "hiir/test/TestHalfBand.h"
#include "hiir/test/TestPhaseHalfPi.h"
#include "hiir/test/TestUpsampler2x.h"
#include "hiir/Downsampler2xF64Fpu.h"
#include "hiir/Downsampler2xFpu.h"
#include "hiir/HalfBandF64Fpu.h"
#include "hiir/HalfBandFpu.h"
#include "hiir/fnc.h"
#include "hiir/PhaseHalfPiF64Fpu.h"
#include "hiir/PhaseHalfPiFpu.h"
#include "hiir/PolyphaseIir2Designer.h"
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

#include <vector>

#include <cstdio>


namespace hiir
{
namespace test
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int NC>
int	TestAllClassesFnc <NC>::perform_test (double transition_bw)
{
	assert (transition_bw > 0);
	assert (transition_bw < 0.5);

	int            ret_val = 0;

	CpuId          cpu;

	// hiir::PolyphaseIir2Designer
	double         coef_arr [NBR_COEFS];
	printf ("Calculating %d filter coefficients... ", NBR_COEFS);
	fflush (stdout);
	PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
		&coef_arr [0],
		NBR_COEFS,
		transition_bw
	);
	printf ("Done.\n");
	const double   stopband_at =
		PolyphaseIir2Designer::compute_atten_from_order_tbw (
			NBR_COEFS,
			transition_bw
		);
	printf ("Transition bandwidth: %f\n", transition_bw);
	printf ("Stopband attenuation: %f dB\n", stopband_at);
	printf ("\n");

	const float    fs  = 44100;
	const long     len = round_int (fs * 20.0f);
	SweepingSine   ss (fs, 20, 22000, len);

	if (ret_val == 0)
	{
		// hiir::HalfBandFpu
		typedef HalfBandFpu <NBR_COEFS> TestedType;
		TestedType     dspl;
		ret_val = TestHalfBand <TestedType>::perform_test (
			dspl, coef_arr, ss, "fpu", transition_bw, stopband_at
		);
	}

	if (ret_val == 0)
	{
		// hiir::HalfBandF64Fpu
		typedef HalfBandF64Fpu <NBR_COEFS> TestedType;
		TestedType     dspl;
		ret_val = TestHalfBand <TestedType>::perform_test (
			dspl, coef_arr, ss, "fpu", transition_bw, stopband_at
		);
	}

	if (ret_val == 0)
	{
		// hiir::Downsampler2xFpu
		typedef Downsampler2xFpu <NBR_COEFS> TestedType;
		TestedType     dspl;
		ret_val = TestDownsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "fpu", transition_bw, stopband_at
		);
	}

	if (ret_val == 0)
	{
		// hiir::Downsampler2xF64Fpu
		typedef Downsampler2xF64Fpu <NBR_COEFS> TestedType;
		TestedType     dspl;
		ret_val = TestDownsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "fpu", transition_bw, stopband_at
		);
	}

	if (ret_val == 0)
	{
		// hiir::Upsampler2xFpu
		typedef Upsampler2xFpu <NBR_COEFS> TestedType;
		TestedType     dspl;
		ret_val = TestUpsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "fpu", transition_bw, stopband_at
		);
	}

	if (ret_val == 0)
	{
		// hiir::Upsampler2xF64Fpu
		typedef Upsampler2xF64Fpu <NBR_COEFS> TestedType;
		TestedType     dspl;
		ret_val = TestUpsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "fpu", transition_bw, stopband_at
		);
	}

	if (ret_val == 0)
	{
		// hiir::PhaseHalfPiFpu
		typedef PhaseHalfPiFpu <NBR_COEFS> TestedType;
		TestedType     dspl;
		ret_val = TestPhaseHalfPi <TestedType>::perform_test (
			dspl, coef_arr, ss, "fpu", transition_bw
		);
	}

	if (ret_val == 0)
	{
		// hiir::PhaseHalfPiF64Fpu
		typedef PhaseHalfPiF64Fpu <NBR_COEFS> TestedType;
		TestedType     dspl;
		ret_val = TestPhaseHalfPi <TestedType>::perform_test (
			dspl, coef_arr, ss, "fpu", transition_bw
		);
	}

#if defined (hiir_test_3DNOW)
	if (ret_val == 0 && cpu._3dnow_flag)
	{
		// hiir::Downsampler2x3dnow
		typedef Downsampler2x3dnow <NBR_COEFS> TestedType;
		TestedType     dspl;
		ret_val = TestDownsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "3dnow", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._3dnow_flag)
	{
		// hiir::Upsampler2x3dnow
		typedef Upsampler2x3dnow <NBR_COEFS> TestedType;
		TestedType     dspl;
		ret_val = TestUpsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "3dnow", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._3dnow_flag)
	{
		// hiir::PhaseHalfPi3dnow
		typedef PhaseHalfPi3dnow <NBR_COEFS> TestedType;
		TestedType     dspl;
		ret_val = TestPhaseHalfPi <TestedType>::perform_test (
			dspl, coef_arr, ss, "3dnow", transition_bw
		);
	}
#endif

#if defined (hiir_test_SSE)
	if (ret_val == 0 && cpu._sse_flag)
	{
		// hiir::HalfBandSse
		typedef HalfBandSse <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestHalfBand <TestedType>::perform_test (
			dspl, coef_arr, ss, "sse", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._sse_flag)
	{
		// hiir::HalfBand4Sse
		typedef HalfBand4Sse <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestHalfBand <TestedType>::perform_test (
			dspl, coef_arr, ss, "sse", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._sse_flag)
	{
		// hiir::Downsampler2xSseOld
		typedef Downsampler2xSseOld <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestDownsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "sseO", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._sse_flag)
	{
		// hiir::Downsampler2xSse
		typedef Downsampler2xSse <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestDownsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "sse", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._sse_flag)
	{
		// hiir::Downsampler2x4Sse
		typedef Downsampler2x4Sse <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestDownsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "sse", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._sse_flag)
	{
		// hiir::Upsampler2xSseOld
		typedef Upsampler2xSseOld <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestUpsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "sse", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._sse_flag)
	{
		// hiir::Upsampler2xSse
		typedef Upsampler2xSse <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestUpsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "sse", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._sse_flag)
	{
		// hiir::Upsampler2x4Sse
		typedef Upsampler2x4Sse <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestUpsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "sse", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._sse_flag)
	{
		// hiir::PhaseHalfPiSse
		typedef PhaseHalfPiSse <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestPhaseHalfPi <TestedType>::perform_test (
			dspl, coef_arr, ss, "sse", transition_bw
		);
	}

	if (ret_val == 0 && cpu._sse_flag)
	{
		// hiir::PhaseHalfPi4Sse
		typedef PhaseHalfPi4Sse <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestPhaseHalfPi <TestedType>::perform_test (
			dspl, coef_arr, ss, "sse", transition_bw
		);
	}
#endif

#if defined (hiir_test_SSE2)
	if (ret_val == 0 && cpu._sse2_flag)
	{
		// hiir::HalfBandF64Sse2
		typedef HalfBandF64Sse2 <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestHalfBand <TestedType>::perform_test (
			dspl, coef_arr, ss, "sse2", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._sse2_flag)
	{
		// hiir::HalfBand2F64Sse2
		typedef HalfBand2F64Sse2 <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestHalfBand <TestedType>::perform_test (
			dspl, coef_arr, ss, "sse2", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._sse2_flag)
	{
		// hiir::Downsampler2xF64Sse2
		typedef Downsampler2xF64Sse2 <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestDownsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "sse2", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._sse2_flag)
	{
		// hiir::Downsampler2x2F64Sse2
		typedef Downsampler2x2F64Sse2 <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestDownsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "sse2", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._sse2_flag)
	{
		// hiir::Upsampler2x2F64Sse2
		typedef Upsampler2x2F64Sse2 <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestUpsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "sse2", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._sse2_flag)
	{
		// hiir::Upsampler2xF64Sse2
		typedef Upsampler2xF64Sse2 <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestUpsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "sse2", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._sse2_flag)
	{
		// hiir::PhaseHalfPi2F64Sse2
		typedef PhaseHalfPi2F64Sse2 <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestPhaseHalfPi <TestedType>::perform_test (
			dspl, coef_arr, ss, "sse2", transition_bw
		);
	}

	if (ret_val == 0 && cpu._sse2_flag)
	{
		// hiir::PhaseHalfPiF64Sse2
		typedef PhaseHalfPiF64Sse2 <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestPhaseHalfPi <TestedType>::perform_test (
			dspl, coef_arr, ss, "sse2", transition_bw
		);
	}
#endif

#if defined (hiir_test_AVX)
	if (ret_val == 0 && cpu._avx_flag)
	{
		// hiir::HalfBand8Avx
		typedef HalfBand8Avx <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestHalfBand <TestedType>::perform_test (
			dspl, coef_arr, ss, "avx", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._avx_flag)
	{
		// hiir::HalfBand4F64Avx
		typedef HalfBand4F64Avx <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestHalfBand <TestedType>::perform_test (
			dspl, coef_arr, ss, "avx", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._avx_flag)
	{
		// hiir::Downsampler2x4F64Avx
		typedef Downsampler2x4F64Avx <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestDownsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "avx", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._avx_flag)
	{
		// hiir::Downsampler2x8Avx
		typedef Downsampler2x8Avx <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestDownsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "avx", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._avx_flag)
	{
		// hiir::PhaseHalfPi8Avx
		typedef PhaseHalfPi8Avx <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestPhaseHalfPi <TestedType>::perform_test (
			dspl, coef_arr, ss, "avx", transition_bw
		);
	}

	if (ret_val == 0 && cpu._avx_flag)
	{
		// hiir::PhaseHalfPi4F64Avx
		typedef PhaseHalfPi4F64Avx <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestPhaseHalfPi <TestedType>::perform_test (
			dspl, coef_arr, ss, "avx", transition_bw
		);
	}

	if (ret_val == 0 && cpu._avx_flag)
	{
		// hiir::Upsampler2x4F64Avx
		typedef Upsampler2x4F64Avx <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestUpsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "avx", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._avx_flag)
	{
		// hiir::Upsampler2x8Avx
		typedef Upsampler2x8Avx <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestUpsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "avx", transition_bw, stopband_at
		);
	}
#endif

#if defined (hiir_test_AVX512)
	if (ret_val == 0 && cpu._avx512f_flag)
	{
		// hiir::HalfBand16Avx512
		typedef HalfBand16Avx512 <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestHalfBand <TestedType>::perform_test (
			dspl, coef_arr, ss, "avx512", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._avx512f_flag)
	{
		// hiir::HalfBand8F64Avx512
		typedef HalfBand8F64Avx512 <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestHalfBand <TestedType>::perform_test (
			dspl, coef_arr, ss, "avx512", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._avx512f_flag)
	{
		// hiir::Downsampler2x16Avx512
		typedef Downsampler2x16Avx512 <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestDownsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "avx512", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._avx512f_flag)
	{
		// hiir::Downsampler2x8F64Avx512
		typedef Downsampler2x8F64Avx512 <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestDownsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "avx512", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._avx512f_flag)
	{
		// hiir::PhaseHalfPi16Avx512
		typedef PhaseHalfPi16Avx512 <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestPhaseHalfPi <TestedType>::perform_test (
			dspl, coef_arr, ss, "avx512", transition_bw
		);
	}

	if (ret_val == 0 && cpu._avx512f_flag)
	{
		// hiir::PhaseHalfPi8F64Avx512
		typedef PhaseHalfPi8F64Avx512 <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestPhaseHalfPi <TestedType>::perform_test (
			dspl, coef_arr, ss, "avx512", transition_bw
		);
	}

	if (ret_val == 0 && cpu._avx512f_flag)
	{
		// hiir::Upsampler2x16Avx512
		typedef Upsampler2x16Avx512 <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestUpsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "avx512", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._avx512f_flag)
	{
		// hiir::Upsampler2x8F64Avx512
		typedef Upsampler2x8F64Avx512 <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestUpsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "avx512", transition_bw, stopband_at
		);
	}
#endif

#if defined (hiir_test_NEON)
	if (ret_val == 0 && cpu._neon_flag)
	{
		// hiir::HalfBandNeon
		typedef HalfBandNeon <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestHalfBand <TestedType>::perform_test (
			dspl, coef_arr, ss, "neon", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._neon_flag)
	{
		// hiir::HalfBand4Neon
		typedef HalfBand4Neon <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestHalfBand <TestedType>::perform_test (
			dspl, coef_arr, ss, "neon", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._neon_flag)
	{
		// hiir::Downsampler2xNeon
		typedef Downsampler2xNeon <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestDownsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "neon", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._neon_flag)
	{
		// hiir::Downsampler2xNeonOld
		typedef Downsampler2xNeonOld <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestDownsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "neonO", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._neon_flag)
	{
		// hiir::Downsampler2x4Neon
		typedef Downsampler2x4Neon <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestDownsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "neon", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._neon_flag)
	{
		// hiir::Upsampler2xNeon
		typedef Upsampler2xNeon <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestUpsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "neon", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._neon_flag)
	{
		// hiir::Upsampler2xNeonOld
		typedef Upsampler2xNeonOld <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestUpsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "neonO", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._neon_flag)
	{
		// hiir::Upsampler2x4Neon
		typedef Upsampler2x4Neon <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestUpsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "neon", transition_bw, stopband_at
		);
	}

	if (ret_val == 0 && cpu._neon_flag)
	{
		// hiir::PhaseHalfPiNeon
		typedef PhaseHalfPiNeon <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestPhaseHalfPi <TestedType>::perform_test (
			dspl, coef_arr, ss, "neon", transition_bw
		);
	}

	if (ret_val == 0 && cpu._neon_flag)
	{
		// hiir::PhaseHalfPi4Neon
		typedef PhaseHalfPi4Neon <NBR_COEFS> TestedType;
		AlignedObject <TestedType> container;
		TestedType &   dspl = container.use ();
		ret_val = TestPhaseHalfPi <TestedType>::perform_test (
			dspl, coef_arr, ss, "neon", transition_bw
		);
	}
#endif

	return (ret_val);
}



template <int NC>
void	TestAllClassesFnc <NC>::perform_test_r (int &ret_val, double transition_bw)
{
	assert (transition_bw > 0);
	assert (transition_bw < 0.5);

	if (ret_val == 0)
	{
		ret_val = perform_test (transition_bw);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace test
}  // namespace hiir



#endif   // hiir_test_TestAllClassesFnc_CODEHEADER_INCLUDED

#undef hiir_test_TestAllClassesFnc_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
