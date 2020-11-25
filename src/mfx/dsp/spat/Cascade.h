/*****************************************************************************

        Cascade.h
        Author: Laurent de Soras, 2020

Template parameters:

- D1: first processor. Requires:
	D1::D1 ();
	D1::~D1 ();
	typename D1::DataType;
	D1::DataType D1::process_sample (D1::DataType);
	void D1::clear_buffers ();

- D2: same as D1, but processed with the output of D1

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_spat_Cascade_HEADER_INCLUDED)
#define mfx_dsp_spat_Cascade_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"



namespace mfx
{
namespace dsp
{
namespace spat
{



template <class D1, class D2>
class Cascade
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef typename D1::DataType DataType;

	D1 &           use_proc_1 ();
	D2 &           use_proc_2 ();

	fstb_FORCEINLINE DataType
	               process_sample (DataType x);
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	D1             _d1;
	D2             _d2;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Cascade &other) const = delete;
	bool           operator != (const Cascade &other) const = delete;

}; // class Cascade



}  // namespace spat
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/spat/Cascade.hpp"



#endif   // mfx_dsp_spat_Cascade_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
