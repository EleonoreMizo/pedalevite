/*****************************************************************************

        FrequencyShifter.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_FrequencyShifter_HEADER_INCLUDED)
#define mfx_pi_FrequencyShifter_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/AllocAlign.h"
#include "fstb/SingleObj.h"
#include "mfx/dsp/osc/OscSinCosStable4Simd.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/pi/ParamDescSet.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/pi/PhaseHalfPi.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{



class FrequencyShifter
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Param
	{
		Param_FREQ = 0,

		Param_NBR_ELT
	};

	               FrequencyShifter ();
	virtual        ~FrequencyShifter () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	virtual State  do_get_state () const;
	virtual int    do_init ();
	virtual int    do_restore ();
	virtual void   do_get_nbr_io (int &nbr_i, int &nbr_o) const;
	virtual bool   do_prefer_stereo () const;
	virtual int    do_get_nbr_param (piapi::ParamCateg categ) const;
	virtual const piapi::ParamDescInterface &
	               do_get_param_info (piapi::ParamCateg categ, int index) const;
	virtual double do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const;
	virtual int    do_reset (double sample_freq, int max_buf_len, int &latency);
	virtual void   do_process_block (ProcInfo &proc);



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
	               _max_freq = 5000;
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
	typedef std::array <
		Channel,
		piapi::PluginInterface::_max_nbr_chn
	>	ChannelArray;

	class Aligned
	{
	public:
		ChannelArray   _chn_arr;
		dsp::osc::OscSinCosStable4Simd
		               _osc;
	};

	void           update_step ();

	State          _state;

	ParamDescSet   _desc_set;
	ParamStateSet  _state_set;
	double         _sample_freq;        // Hz, > 0. <= 0: not initialized

	fstb::util::NotificationFlag
	               _param_change_flag;

	fstb::SingleObj <Aligned, fstb::AllocAlign <Aligned, 16> >
	               _ali;
	float          _inv_fs;
	float          _freq;
	float          _step_angle;         // Radians

	BufArray       _buf_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               FrequencyShifter (const FrequencyShifter &other)  = delete;
	FrequencyShifter &
	               operator = (const FrequencyShifter &other)        = delete;
	bool           operator == (const FrequencyShifter &other) const = delete;
	bool           operator != (const FrequencyShifter &other) const = delete;

}; // class FrequencyShifter



}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/FrequencyShifter.hpp"



#endif   // mfx_pi_FrequencyShifter_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
