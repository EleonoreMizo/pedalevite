/*****************************************************************************

        BadRadio.h
        Author: Laurent de Soras, 2020

Based on an effect by Geraint Luff:
https://gist.github.com/geraintluff/4cfb7f71edfc6acece9ff510a4174659

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_badrad_BadRadio_HEADER_INCLUDED)
#define mfx_pi_badrad_BadRadio_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/ctrl/VarBlock.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/pi/badrad/BadRadioDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace badrad
{



class BadRadio final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       BadRadio (piapi::HostInterface &host);
	               ~BadRadio () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) noexcept final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	class Channel
	{
	public:
		dsp::iir::Biquad
		               _noise_lpf;
		dsp::iir::Biquad
		               _out_hpf;
		dsp::iir::Biquad
		               _out_lpf;
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	void           clear_buffers () noexcept;
	void           update_param (bool force_flag = false) noexcept;
	template <typename F>
	void           udpate_filter (float freq, std::array <float, 3> b_s, F use_filter) noexcept;

	piapi::HostInterface &
	               _host;
	State          _state = State_CREATED;

	BadRadioDesc   _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc { _state_set };
	float          _sample_freq = 0;    // Hz, > 0. <= 0: not initialized
	float          _inv_fs      = 0;    // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;

	ChannelArray   _chn_arr;

	dsp::ctrl::VarBlock
	               _gain;
	uint32_t       _noise_cnt   = 0;
	float          _noise_fix   = 1;    // Sampling-rate dependent gain
	float          _noise_lvl   = 0;
	float          _nz_comp     = 1;    // Volume compensation for the noise level
	float          _hiss_lvl    = 0;




/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               BadRadio ()                               = delete;
	               BadRadio (const BadRadio &other)          = delete;
	               BadRadio (BadRadio &&other)               = delete;
	BadRadio &     operator = (const BadRadio &other)        = delete;
	BadRadio &     operator = (BadRadio &&other)             = delete;
	bool           operator == (const BadRadio &other) const = delete;
	bool           operator != (const BadRadio &other) const = delete;

}; // class BadRadio



}  // namespace badrad
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/badrad/BadRadio.hpp"



#endif   // mfx_pi_badrad_BadRadio_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
