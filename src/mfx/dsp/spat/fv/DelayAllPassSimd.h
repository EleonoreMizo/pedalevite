/*****************************************************************************

        DelayAllPassSimd.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_spat_fv_DelayAllPassSimd_HEADER_INCLUDED)
#define mfx_dsp_spat_fv_DelayAllPassSimd_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/spat/fv/DelayLineSimple.h"



namespace mfx
{
namespace dsp
{
namespace spat
{
namespace fv
{



class DelayAllPassSimd
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_delay (int len);
	void           set_feedback (float coef) noexcept;
	void           clear_buffers () noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	DelayLineSimple
	               _delay_line;
	float          _fdbk        = 0; // in ]-1 ; 1[



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const DelayAllPassSimd &other) const = delete;
	bool           operator != (const DelayAllPassSimd &other) const = delete;

}; // class DelayAllPassSimd



}  // namespace fv
}  // namespace spat
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/spat/fv/DelayAllPassSimd.hpp"



#endif   // mfx_dsp_spat_fv_DelayAllPassSimd_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
