/*****************************************************************************

        SplDataRetrievalInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_SplDataRetrievalInterface_HEADER_INCLUDED)
#define	mfx_dsp_SplDataRetrievalInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cstdint>



namespace mfx
{
namespace dsp
{



class SplDataRetrievalInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	virtual			~SplDataRetrievalInterface () {}

	void				get_data (float *chn_data_ptr_arr [], int64_t pos, long len, bool invert_flag);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void	do_get_data (float *chn_data_ptr_arr [], int64_t pos, long len, bool invert_flag) = 0;



};	// class SplDataRetrievalInterface



}	// namespace dsp
}	// namespace mfx



//#include	"mfx/dsp/SplDataRetrievalInterface.hpp"



#endif	// mfx_dsp_SplDataRetrievalInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
