/*****************************************************************************

        StageProc3dnow.h
        Copyright (c) 2005 Laurent de Soras

Template parameters:
	- REMAINING: Number of remaining coefficients to process, >= 0

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (hiir_StageProc3dnow_HEADER_INCLUDED)
#define	hiir_StageProc3dnow_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"hiir/def.h"



namespace hiir
{



template <int CUR>
class StageProc3dnow
{

	// Template parameter check, not used
	typedef	int	ChkTpl1 [(CUR >= 0) ? 1 : -1];

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static hiir_FORCEINLINE void
						process_sample_pos ();
	static hiir_FORCEINLINE void
						process_sample_neg ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						StageProc3dnow ();
						StageProc3dnow (const StageProc3dnow &other);
	StageProc3dnow &
						operator = (const StageProc3dnow &other);
	bool				operator == (const StageProc3dnow &other);
	bool				operator != (const StageProc3dnow &other);

};	// class StageProc3dnow



}	// namespace hiir



#include	"hiir/StageProc3dnow.hpp"



#endif	// hiir_StageProc3dnow_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
