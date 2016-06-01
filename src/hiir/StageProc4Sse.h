/*****************************************************************************

        StageProc4Sse.h
        Copyright (c) 2015 Laurent de Soras

Template parameters:
	- REMAINING: Number of remaining coefficients to process, >= 0

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_StageProc_HEADER_INCLUDED)
#define	hiir_StageProc_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "hiir/def.h"

#include <xmmintrin.h>



namespace hiir
{



class StageDataSse;

template <int REMAINING>
class StageProc4Sse
{

	// Template parameter check, not used
	typedef	int	ChkTpl1 [(REMAINING >= 0) ? 1 : -1];

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static hiir_FORCEINLINE void
	               process_sample_pos (const int nbr_coefs, __m128 &spl_0, __m128 &spl_1, StageDataSse *stage_arr);
	static hiir_FORCEINLINE void
	               process_sample_neg (const int nbr_coefs, __m128 &spl_0, __m128 &spl_1, StageDataSse *stage_arr);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               StageProc4Sse ();
	               StageProc4Sse (const StageProc4Sse &other);
	StageProc4Sse& operator = (const StageProc4Sse &other);
	bool           operator == (const StageProc4Sse &other);
	bool           operator != (const StageProc4Sse &other);

};	// class StageProc4Sse



}	// namespace hiir



#include "hiir/StageProc4Sse.hpp"



#endif	// hiir_StageProc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
