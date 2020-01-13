/*****************************************************************************

        BandParam.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_peq_BandParam_HEADER_INCLUDED)
#define mfx_pi_peq_BandParam_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/peq/PEqType.h"



namespace mfx
{
namespace pi
{
namespace peq
{



class BandParam
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_gain (float gain);
	float          get_gain () const;
	void           set_freq (float freq);
	float          get_freq () const;
	void           set_q (float q);
	float          get_q () const;
	void           set_type (PEqType type);
	PEqType        get_type () const;
	void           set_bypass (bool bypass_flag);
	bool           is_bypass () const;

	void           create_filter (float bz [3], float az [3], float fs, float inv_fs) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	float          _gain        =    1;
	float          _freq        = 1000;
	float          _q           =    1;
	PEqType        _type        = PEqType_PEAK;
	bool           _bypass_flag = false;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const BandParam &other) const = delete;
	bool           operator != (const BandParam &other) const = delete;

}; // class BandParam



}  // namespace peq
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/peq/BandParam.hpp"



#endif   // mfx_pi_peq_BandParam_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
