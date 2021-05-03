/*****************************************************************************

        DenormStop.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_DenormStop_HEADER_INCLUDED)
#define mfx_dsp_DenormStop_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cstdint>



namespace mfx
{
namespace dsp
{



class DenormStop
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static float   process_sample (float x) noexcept;
	static void    process_block (float buf_ptr [], int nbr_spl) noexcept;
	static void    process_block_2chn (float buf_ptr [], int nbr_frames) noexcept;
	static void    process_block_4chn (float buf_ptr [], int nbr_frames) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static inline float
	               gen_new_rnd_val () noexcept;

	static uint32_t
	               _rnd_val;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	virtual        ~DenormStop ()                              = delete;
	               DenormStop ()                               = delete;
	               DenormStop (const DenormStop &other)        = delete;
	DenormStop &   operator = (const DenormStop &other)        = delete;
	bool           operator == (const DenormStop &other) const = delete;
	bool           operator != (const DenormStop &other) const = delete;

}; // class DenormStop



}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/DenormStop.hpp"



#endif   // mfx_dsp_DenormStop_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
