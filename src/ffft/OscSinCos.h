/*****************************************************************************

        OscSinCos.h
        By Laurent de Soras

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (ffft_OscSinCos_HEADER_INCLUDED)
#define	ffft_OscSinCos_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"ffft/def.h"



namespace ffft
{



template <class T>
class OscSinCos
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef	T	DataType;

	ffft_FORCEINLINE void
						set_step (double angle_rad) noexcept;

	ffft_FORCEINLINE DataType
						get_cos () const noexcept;
	ffft_FORCEINLINE DataType
						get_sin () const noexcept;
	ffft_FORCEINLINE void
						step () noexcept;
	ffft_FORCEINLINE void
						clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	DataType       _pos_cos  { 1 };  // Current phase expressed with sin and cos. [-1 ; 1]
	DataType       _pos_sin  { 0 };  // -
	DataType       _step_cos { 1 };  // Phase increment per step, [-1 ; 1]
	DataType       _step_sin { 0 };  // -



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const OscSinCos &other) const = delete;
	bool           operator != (const OscSinCos &other) const = delete;

};	// class OscSinCos



}	// namespace ffft



#include	"ffft/OscSinCos.hpp"



#endif	// ffft_OscSinCos_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
