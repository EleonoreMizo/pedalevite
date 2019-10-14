/*****************************************************************************

        FreqShiftCore.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_freqsh_FreqShiftCore_HEADER_INCLUDED)
#define mfx_pi_freqsh_FreqShiftCore_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/AllocAlign.h"
#include "fstb/SingleObj.h"
#include "mfx/dsp/osc/OscSinCosStableSimd.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/pi/freqsh/PhaseHalfPi.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace freqsh
{



class FreqShiftCore
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _max_nbr_chn = 2;

	               FreqShiftCore ();
	virtual        ~FreqShiftCore () = default;

	void           reset (double sample_freq, int max_buf_len, double &latency);
	void           set_freq (float f);
	bool           is_active () const;
	void           process_block (float * const dst_ptr_arr [], const float * const src_ptr_arr [], int nbr_spl, int nbr_chn);
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum Buf
	{
		Buf_COS = 0,
		Buf_SIN,
		Buf_AAF,
		Buf_PHC,
		Buf_PHS,

		Buf_NBR_ELT
	};

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;
	typedef std::array <BufAlign, Buf_NBR_ELT> BufArray;

	static const int
	               _nbr_coef = 8;

	class Channel
	{
	public:
		dsp::iir::Biquad
		               _aa;
		PhaseHalfPi <_nbr_coef>
		               _ssb;
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	class Aligned
	{
	public:
		ChannelArray   _chn_arr;
		dsp::osc::OscSinCosStableSimd
		               _osc;
	};

	void           update_step ();

	double         _sample_freq;        // Hz, > 0. <= 0: not initialized
	fstb::SingleObj <Aligned, fstb::AllocAlign <Aligned, 16> >
	               _ali;
	float          _inv_fs;
	float          _freq;
	float          _step_angle;         // Radians

	BufArray       _buf_arr;
	std::array <double, _nbr_coef>
	               _coef_list;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               FreqShiftCore (const FreqShiftCore &other)     = delete;
	FreqShiftCore &
	               operator = (const FreqShiftCore &other)        = delete;
	bool           operator == (const FreqShiftCore &other) const = delete;
	bool           operator != (const FreqShiftCore &other) const = delete;

}; // class FreqShiftCore



}  // namespace freqsh
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/freqsh/FreqShiftCore.hpp"



#endif   // mfx_pi_freqsh_FreqShiftCore_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
