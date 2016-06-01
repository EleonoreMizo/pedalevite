/*****************************************************************************

        StageProcFpu.h
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



#if ! defined (hiir_StageProc_HEADER_INCLUDED)
#define	hiir_StageProc_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"hiir/def.h"



namespace hiir
{



template <int REMAINING>
class StageProcFpu
{

	// Template parameter check, not used
	typedef	int	ChkTpl1 [(REMAINING >= 0) ? 1 : -1];

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static hiir_FORCEINLINE void
						process_sample_pos (const int nbr_coefs, float &spl_0, float &spl_1, const float coef [], float x [], float y []);
	static hiir_FORCEINLINE void
						process_sample_neg (const int nbr_coefs, float &spl_0, float &spl_1, const float coef [], float x [], float y []);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						StageProcFpu ();
						StageProcFpu (const StageProcFpu &other);
	StageProcFpu &	operator = (const StageProcFpu &other);
	bool				operator == (const StageProcFpu &other);
	bool				operator != (const StageProcFpu &other);

};	// class StageProcFpu



}	// namespace hiir



#include	"hiir/StageProcFpu.hpp"



#endif	// hiir_StageProc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
