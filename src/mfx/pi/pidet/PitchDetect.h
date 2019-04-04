/*****************************************************************************

        PitchDetect.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_pidet_PitchDetect_HEADER_INCLUDED)
#define mfx_pi_pidet_PitchDetect_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/ana/FreqAmGha.h"
#include "mfx/pi/pidet/PitchDetectDesc.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace pidet
{



class PitchDetect
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               PitchDetect ();
	virtual        ~PitchDetect () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	virtual State  do_get_state () const;
	virtual double do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const;
	virtual int    do_reset (double sample_freq, int max_buf_len, int &latency);
	virtual void   do_clean_quick ();
	virtual void   do_process_block (ProcInfo &proc);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum OutType
	{
		OutType_PITCH = 0,
		OutType_FREQ,

		OutType_NBR_ELT
	};

	static const int  _sub_spl_max = 16;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;
	typedef std::array <float, _sub_spl_max>  BufPrevSpl;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);

	State          _state;

	PitchDetectDesc
	               _desc;
	ParamStateSet  _state_set;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;

	BufPrevSpl     _buf_prev_spl;       // Buffer containing the last samples of the previous frame that were not downsampled
	int            _nbr_spl_in_buf;     // Number of samples contained in the buffer
	int            _sub_spl;            // Downsampling ratio. Currently 8 or 16 only
	dsp::ana::FreqAmGha
	               _analyser;
	float          _freq;               // Hz. 0 = not found
	float          _last_valid_output;
	OutType        _output_type;
	BufAlign       _buffer;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PitchDetect (const PitchDetect &other)       = delete;
	PitchDetect &  operator = (const PitchDetect &other)        = delete;
	bool           operator == (const PitchDetect &other) const = delete;
	bool           operator != (const PitchDetect &other) const = delete;

}; // class PitchDetect



}  // namespace pidet
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/pidet/PitchDetect.hpp"



#endif   // mfx_pi_pidet_PitchDetect_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
