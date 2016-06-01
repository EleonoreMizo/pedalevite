/*****************************************************************************

        StageProcSse.h
        Copyright (c) 2005 Laurent de Soras

Template parameters:
	- CUR: index of the coefficient to process, >= 0

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_StageProcSse_HEADER_INCLUDED)
#define	hiir_StageProcSse_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"hiir/def.h"

#include <xmmintrin.h>



namespace hiir
{



class StageDataSse;

template <int CUR>
class StageProcSse
{

	// Template parameter check, not used
	typedef	int	ChkTpl1 [(CUR >= 0) ? 1 : -1];

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static hiir_FORCEINLINE void
						process_sample_pos (StageDataSse *stage_ptr, __m128 &y, __m128 &mem);
	static hiir_FORCEINLINE void
						process_sample_neg (StageDataSse *stage_ptr, __m128 &y, __m128 &mem);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum {         PREV = CUR - 1 };



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						StageProcSse ();
						StageProcSse (const StageProcSse &other);
	StageProcSse &	operator = (const StageProcSse &other);
	bool				operator == (const StageProcSse &other);
	bool				operator != (const StageProcSse &other);

};	// class StageProcSse



}	// namespace hiir



#include	"hiir/StageProcSse.hpp"



#endif	// hiir_StageProcSse_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
