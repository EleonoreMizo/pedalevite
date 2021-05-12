/*****************************************************************************

        conf.h
        Author: Laurent de Soras, 2005

Depending on your CPU, define/undef symbols in this file.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_test_conf_HEADER_INCLUDED)
#define hiir_test_conf_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/def.h"



namespace hiir
{
namespace test
{



// CPU configuration (check and modify this, depending on your CPU)
#define hiir_test_3DNOW
#define hiir_test_SSE
#define hiir_test_SSE2
#undef hiir_test_AVX
#undef hiir_test_AVX512
#define hiir_test_NEON



// Removes code that isn't available for compilers/architectures
#if hiir_ARCHI == hiir_ARCHI_PPC

	#undef hiir_test_3DNOW
	#undef hiir_test_SSE
	#undef hiir_test_SSE2
	#undef hiir_test_AVX
	#undef hiir_test_AVX512
	#undef hiir_test_NEON

#elif hiir_ARCHI == hiir_ARCHI_ARM

	#undef hiir_test_3DNOW
	#undef hiir_test_SSE
	#undef hiir_test_SSE2
	#undef hiir_test_AVX
	#undef hiir_test_AVX512

#elif hiir_ARCHI == hiir_ARCHI_X86

	#undef hiir_test_NEON

	#if (! defined (_MSC_VER) || defined (_WIN64))
		#undef hiir_test_3DNOW
	#endif

#else

	#undef hiir_test_3DNOW
	#undef hiir_test_SSE
	#undef hiir_test_SSE2
	#undef hiir_test_AVX
	#undef hiir_test_AVX512
	#undef hiir_test_NEON

#endif



// Testing options
#undef  hiir_test_SAVE_RESULTS
#define hiir_test_LONG_FUNC_TESTS
#define hiir_test_LONG_SPEED_TESTS
constexpr int hiir_test_nbr_blocks = 65536;



}  // namespace test
}  // namespace hiir



#endif   // hiir_test_conf_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
