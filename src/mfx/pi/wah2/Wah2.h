/*****************************************************************************

        Wah2.h
        Author: Laurent de Soras, 2017

Original model and algorithm by Transmogrifox
http://cackleberrypines.net/transmogrifox/src/bela/inductor_wah_C_src

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_wah2_Wah2_HEADER_INCLUDED)
#define mfx_pi_wah2_Wah2_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "mfx/dsp/iir/OnePole.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/pi/wah2/Wah2Desc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"
#include "mfx/piapi/ProcInfo.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace wah2
{



class Wah2 final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       Wah2 (piapi::HostInterface &host);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class Spec
	{
	public:
		enum RpPar
		{
			RpPar_ALONE = 0,
			RpPar_RI,
			RpPar_RE,

			RpPar_NBR_ELT
		};

		float          _rc;     // BJT gain stage collector resistor
		float          _rpot;   // Pot resistance value
		float          _rbias;  // Typically 470k bias resistor shows up in parallel with output
		float          _re;     // BJT gain stage emitter resistor
		float          _beta;   // BJT forward gain
		float          _cf;
		float          _ci;
		float          _ri;
		float          _rp_main;
		RpPar          _rp_par;
		float          _lp;
		float          _rs;
	};

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	class Channel
	{
	public:
		dsp::iir::OnePole
		               _hpf;
		std::array <float, 2>
		               _mem_x {{ 0, 0 }};
		std::array <float, 2>
		               _mem_y {{ 0, 0 }};
		void           clear_buffers ();
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);
	void           update_model ();
	void           update_model (float f_hp, float f0, float q, float gf, float gi, float gbpf);
	void           process_reso (Channel &chn, float spl_ptr [], int nbr_spl) const;

	static constexpr float
	               para (float a, float b);
	static constexpr float
	               para (float a, float b, float c);
	static constexpr float
	               shaper (float x);

	piapi::HostInterface &
	               _host;
	State          _state;

	Wah2Desc       _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_freq;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_type;

	ChannelArray   _chn_arr;

	int            _model;

	std::array <float, 3>               // Fixed coefficients
	               _bz;
	std::array <float, 3>               // Depends on the pedal position
	               _az;
	std::array <float, 3>
	               _az_base;
	std::array <float, 3>
	               _az_delta;
	float          _ax;

	static const Spec
	               _spec_arr [6];



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Wah2 ()                               = delete;
	               Wah2 (const Wah2 &other)              = delete;
	               Wah2 (Wah2 &&other)                   = delete;
	Wah2 &         operator = (const Wah2 &other)        = delete;
	Wah2 &         operator = (Wah2 &&other)             = delete;
	bool           operator == (const Wah2 &other) const = delete;
	bool           operator != (const Wah2 &other) const = delete;

}; // class Wah2



}  // namespace wah2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/wah2/Wah2.hpp"



#endif   // mfx_pi_wah2_Wah2_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
