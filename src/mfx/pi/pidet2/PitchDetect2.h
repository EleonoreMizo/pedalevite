/*****************************************************************************

        PitchDetect2.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_pidet2_PitchDetect2_HEADER_INCLUDED)
#define mfx_pi_pidet2_PitchDetect2_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/AllocAlign.h"
#include "fstb/SingleObj.h"
#include "mfx/dsp/ana/FreqYin.h"
#include "mfx/dsp/ana/ValSmooth.h"
#include "mfx/dsp/iir/Downsampler8xSimd.h"
#include "mfx/pi/pidet2/PitchDetect2Desc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace pidet2
{



class PitchDetect2 final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               PitchDetect2 ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum OutType
	{
		OutType_PITCH = 0,
		OutType_FREQ,

		OutType_NBR_ELT
	};

	static const int  _nc_84       = 2;
	static const int  _nc_42       = 3;
	static const int  _nc_21       = 8;

	static const int  _sub_spl_max = 8;

	class AlignedData
	{
	public:
		dsp::iir::Downsampler8xSimd <_nc_84, _nc_42, _nc_21>
							_dspl;
	};

	typedef fstb::SingleObj <
		AlignedData,
		fstb::AllocAlign <AlignedData, 16>
	> AlignedDataXPtr;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;
	typedef std::array <float, _sub_spl_max>  BufPrevSpl;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);

	State          _state = State_CREATED;

	PitchDetect2Desc
	               _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc { _state_set };
	float          _sample_freq = 0;    // Hz, > 0. <= 0: not initialized
	float          _inv_fs = 0;         // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;

	BufPrevSpl     _buf_prev_spl;       // Buffer containing the last samples of the previous frame that were not downsampled
	int            _nbr_spl_in_buf = 0; // Number of samples contained in the buffer
	const int      _sub_spl = _sub_spl_max; // Downsampling ratio. Currently 8 only.
	AlignedDataXPtr
	               _data_xptr;
	dsp::ana::FreqYin <dsp::ana::ValSmooth <float, 0> >
	               _analyser;
	float          _freq = 0;           // Hz. 0 = not found
	float          _last_valid_output = 0;
	OutType        _output_type = OutType_PITCH;
	BufAlign       _buffer;

	static const std::array <double, _nc_84>
	               _dspl_coef_84;
	static const std::array <double, _nc_42>
	               _dspl_coef_42;
	static const std::array <double, _nc_21>
	               _dspl_coef_21;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PitchDetect2 (const PitchDetect2 &other)      = delete;
	PitchDetect2 & operator = (const PitchDetect2 &other)        = delete;
	bool           operator == (const PitchDetect2 &other) const = delete;
	bool           operator != (const PitchDetect2 &other) const = delete;

}; // class PitchDetect2



}  // namespace pidet2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/pidet2/PitchDetect2.hpp"



#endif   // mfx_pi_pidet2_PitchDetect2_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
