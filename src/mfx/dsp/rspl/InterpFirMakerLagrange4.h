/*****************************************************************************

        InterpFirMakerLagrange4.h
        Author: Laurent de Soras, 2018

Template parameter:

- DT: main data type (floating point)

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_rspl_InterpFirMakerLagrange4_HEADER_INCLUDED)
#define mfx_dsp_rspl_InterpFirMakerLagrange4_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace rspl
{



template <typename DT>
class InterpFirMakerLagrange4
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef DT DataType;

	inline static void
	               make_interpolator (DataType fir_ptr [], float pos);
	inline static int
	               get_length ();
	inline static int
	               get_delay ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const InterpFirMakerLagrange4 <DT> &other) const = delete;
	bool           operator != (const InterpFirMakerLagrange4 <DT> &other) const = delete;

}; // class InterpFirMakerLagrange4



}  // namespace rspl
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/rspl/InterpFirMakerLagrange4.hpp"



#endif   // mfx_dsp_rspl_InterpFirMakerLagrange4_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
