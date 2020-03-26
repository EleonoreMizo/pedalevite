/*****************************************************************************

        DiodeClipper.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dclip_DiodeClipper_HEADER_INCLUDED)
#define mfx_pi_dclip_DiodeClipper_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/ctrl/Ramp.h"
#include "mfx/dsp/iir/DcKiller1p.h"
#include "mfx/dsp/iir/Downsampler4xSimd.h"
#include "mfx/dsp/iir/Upsampler4xSimd.h"
#include "mfx/dsp/va/DiodeClipDAngelo.h"
#include "mfx/pi/dclip/DiodeClipperDesc.h"
#include "mfx/pi/dclip/Shape.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <utility>
#include <vector>



namespace mfx
{
namespace pi
{
namespace dclip
{



class DiodeClipper final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               DiodeClipper ();
	               ~DiodeClipper () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _max_blk_size = 64; // Samples, standard sampling rate, multiple of 4
	static const int  _ovrspl       = 4;
	static const int  _nbr_coef_42  = 3;
	static const int  _nbr_coef_21  = 8;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;
	typedef dsp::iir::Upsampler4xSimd <_nbr_coef_42, _nbr_coef_21> UpSpl;
	typedef dsp::iir::Downsampler4xSimd <_nbr_coef_42, _nbr_coef_21> DwSpl;

	class Channel
	{
	public:
		UpSpl          _upspl;
		DwSpl          _dwspl;
		dsp::va::DiodeClipDAngelo
		               _dist;
		dsp::iir::DcKiller1p
		               _dckill;
	};
	typedef std::vector <Channel, fstb::AllocAlign <Channel, 16> > ChannelArray;

	class DiodeParam
	{
	public:
		float          _is1 = 0.1e-15f;
		float          _n1  = 1;
		float          _is2 = 0.1e-15f;
		float          _n2  = 1;
	};

	void           clear_buffers ();
	void           update_param (bool force_flag = false);

	static std::pair <float, float>
	               compute_gain_pre_post (float gain);
	static void    init_ovrspl_coef ();

	State          _state;

	DiodeClipperDesc
	               _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;

	ChannelArray   _chn_arr;

	dsp::ctrl::Ramp
	               _gain;

	BufAlign       _buf_tmp;
	BufAlign       _buf_ovr;

	static bool    _coef_init_flag;
	static std::array <double, _nbr_coef_42>
	               _coef_42;
	static std::array <double, _nbr_coef_21>
	               _coef_21;

	static const float
	               _sig_scale;
	static const float
	               _diode_scale;
	static const std::array <DiodeParam, Shape_NBR_ELT>
	               _diode_param_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DiodeClipper (const DiodeClipper &other)      = delete;
	               DiodeClipper (DiodeClipper &&other)           = delete;
	DiodeClipper & operator = (const DiodeClipper &other)        = delete;
	DiodeClipper & operator = (DiodeClipper &&other)             = delete;
	bool           operator == (const DiodeClipper &other) const = delete;
	bool           operator != (const DiodeClipper &other) const = delete;

}; // class DiodeClipper



}  // namespace dclip
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dclip/DiodeClipper.hpp"



#endif   // mfx_pi_dclip_DiodeClipper_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
