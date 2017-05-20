/*****************************************************************************

        FreeverbCore.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_fv_FreeverbCore_HEADER_INCLUDED)
#define mfx_pi_fv_FreeverbCore_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/AllocAlign.h"
#include "mfx/pi/fv/DelayAllPassSimd.h"
#include "mfx/pi/fv/DelayComb.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace fv
{



class FreeverbCore
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _max_nbr_chn = 2;

	               FreeverbCore ();
	virtual        ~FreeverbCore () = default;

	void           reset (double sample_freq, int max_buf_len);
	void           set_reflectivity (float fdbk);
	void           set_damp (float damp);
	void           process_block (float * const dst_ptr_arr [], const float * const src_ptr_arr [], int nbr_spl, int nbr_chn);
	void           clear_buffers ();

	static const float
	               _scalewet;
	static const float
	               _scalein;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _nbr_comb = 8;
	static const int  _nbr_ap   = 4;

	typedef std::array <DelayComb, _nbr_comb> CombArray;
	typedef std::array <DelayAllPassSimd, _nbr_ap> AllPassArray;

	class Channel
	{
	public:
		CombArray      _comb_arr;
		AllPassArray   _ap_arr;
	};

	typedef std::array <Channel, _max_nbr_chn> ChnArray;

	double         _sample_freq;        // Hz, > 0. <= 0: not initialized
	ChnArray       _chn_arr;
	std::vector <float, fstb::AllocAlign <float, 16> >
	               _buf;

	static const int
	               _stereospread;
	static const std::array <int, _nbr_comb>
	               _comb_len_arr;
	static const std::array <int, _nbr_ap>
	               _ap_len_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               FreeverbCore (const FreeverbCore &other)      = delete;
	FreeverbCore & operator = (const FreeverbCore &other)        = delete;
	bool           operator == (const FreeverbCore &other) const = delete;
	bool           operator != (const FreeverbCore &other) const = delete;

}; // class FreeverbCore



}  // namespace fv
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/fv/FreeverbCore.hpp"



#endif   // mfx_pi_fv_FreeverbCore_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
