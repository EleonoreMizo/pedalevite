/*****************************************************************************

        OnsetDetect.h
        Author: Laurent de Soras, 2018

Inspired by OnsetsDS by Dan Stowell
http://onsetsds.sourceforge.net/

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_osdet_OnsetDetect_HEADER_INCLUDED)
#define mfx_pi_osdet_OnsetDetect_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "ffft/FFTRealFixLen.h"
#include "fstb/util/NotificationFlag.h"
#include "fstb/AllocAlign.h"
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/dyn/EnvFollowerRms.h"
#include "mfx/dsp/iir/OnePole.h"
#include "mfx/dsp/spec/StftHelper.h"
#include "mfx/pi/osdet/OnsetDetectDesc.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <vector>
#include <array>



namespace mfx
{
namespace pi
{
namespace osdet
{



class OnsetDetect
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               OnsetDetect ();
	virtual        ~OnsetDetect () = default;



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

	static const int  _fft_len_l2 = 9;
	static const int  _fft_len    = 1 << _fft_len_l2;
	static const int  _nbr_bins   = _fft_len / 2;   // Number of bins used for the computation
	static const int  _hop_size   = _fft_len / 2;   // Samples
	static const int  _med_span   = 11;             // Number of FFT frames for the median subtration

	static_assert ((_med_span & 1) != 0, "_med_span must be odd.");
	static_assert ((_nbr_bins & 3) == 0, "_nbr_bins must be a multiple of 4");

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;
	typedef dsp::spec::StftHelper <ffft::FFTRealFixLen <_fft_len_l2> > Stft;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);
	float          compute_coef (double t) const;
	void           set_relax_coef (float t, int hop_size);
	void           compute_magnitudes ();
	void           whiten ();
	float          compute_mkl ();
	bool           detect_onset (float odf_val);

	State          _state;

	OnsetDetectDesc
	               _desc;
	ParamStateSet  _state_set;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;
	BufAlign       _buf_env_main;
	BufAlign       _buf_tmp;
	BufAlign       _buf_freq;
	std::array <BufAlign, 2>
	               _buf_mag_arr;
	BufAlign       _buf_psp;
	dsp::dyn::EnvFollowerRms            /*** To do: use one of the free envelopes from _al instead of this object ***/
	               _env_main;
	dsp::iir::OnePole
	               _onset_prefilter;
	int            _frame_len;          // Size of a frame
	int            _frame_pos;
	float          _vol_sq;             // Square of the currently detected volume of the attack

	float          _velo_gain;
	float          _thr_off;            // Level threshold for note off
	bool           _velo_clip_flag;
	bool           _note_on_flag;       // Indicates that we can accept note off events

	Stft           _stft;
	int            _buf_index;          // Current magnitude buffer, 0 or 1
	float          _odf_val_post_old;
	int            _dist_min;
	int            _dist_cur;
	float          _relax_coef;
	float          _relax_time;
	float          _psp_floor;
	std::array <float, _med_span>
	               _odf_mem_arr;
	int            _odf_mem_pos;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               OnsetDetect (const OnsetDetect &other)       = delete;
	OnsetDetect &  operator = (const OnsetDetect &other)        = delete;
	bool           operator == (const OnsetDetect &other) const = delete;
	bool           operator != (const OnsetDetect &other) const = delete;

}; // class OnsetDetect



}  // namespace osdet
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/osdet/OnsetDetect.hpp"



#endif   // mfx_pi_osdet_OnsetDetect_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
