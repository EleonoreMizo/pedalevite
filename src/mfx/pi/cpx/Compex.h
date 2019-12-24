/*****************************************************************************

        Compex.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_cpx_Compex_HEADER_INCLUDED)
#define mfx_pi_cpx_Compex_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "fstb/DataAlign.h"
#include "fstb/def.h"
#include "fstb/SingleObj.h"
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/dyn/EnvFollowerAR4SimdHelper.h"
#include "mfx/dsp/dyn/SCPower.h"
#include "mfx/dsp/iir/Biquad4Simd.h"
#include "mfx/pi/cpx/CompexDesc.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace cpx
{



class Compex
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Compex ();
	virtual        ~Compex () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	virtual State  do_get_state () const;
	virtual double do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const;
	virtual int    do_reset (double sample_freq, int max_buf_len, int &latency);
	virtual void   do_clean_quick ();
	virtual void   do_process_block (piapi::ProcInfo &proc);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _update_resol = 64;  // Must be a multiple of 4

	typedef std::vector <float, fstb::AllocAlign <float, 16> > SplBuf;

	typedef dsp::dyn::EnvFollowerAR4SimdHelper <
		fstb::DataAlign <true>,
		fstb::DataAlign <true>,
		fstb::DataAlign <true>,
		1
	> EnvFollower;

	typedef dsp::iir::Biquad4Simd <
		fstb::DataAlign <true>,
		fstb::DataAlign <true>,
		fstb::DataAlign <true>
	> Smoother;

	template <int NC>
	class AddProc
	{
	public:
		static fstb_FORCEINLINE float
		               process_scalar (float in);
		static fstb_FORCEINLINE fstb::ToolsSimd::VectF32
		               process_vect (const fstb::ToolsSimd::VectF32 &in);
	};

	void           clear_buffers ();
	void           update_param (bool force_flag = false);
	void           update_param_ar ();
	void           update_param_vol_curves_ss ();
	float          compute_env_coef (float t) const;

	void           process_block_part (float * const out_ptr_arr [], const float * const in_ptr_arr [], const float * const sc_ptr_arr [], int pos_beg, int pos_end);
	void           conv_env_to_log (int nbr_spl);
	template <bool store_flag>
	fstb_FORCEINLINE fstb::ToolsSimd::VectF32
	               compute_gain (const fstb::ToolsSimd::VectF32 env_2l2);

	State          _state;

	CompexDesc     _desc;
	ParamStateSet  _state_set;
	double         _sample_freq;        // Hz, > 0. <= 0: not initialized

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_ar;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_vol_curves_ss;

	int            _nbr_chn;				// > 0
	int            _nbr_chn_in;         // > 0
	int            _nbr_chn_ana;			// > 0

	fstb::SingleObj <EnvFollower, fstb::AllocAlign <EnvFollower, 16> >
						_env_fol_xptr;
	fstb::SingleObj <Smoother, fstb::AllocAlign <Smoother, 16> >
						_smoother_xptr;
	bool				_use_side_chain_flag;

	float          _vol_offset_pre;     // log2 (threshold)
	float          _vol_offset_post;    // log2 (threshold) + log2 (gain)
	float          _ratio_hi;           // 1 / ratio_hi(dB)
	float          _ratio_lo;           // 1 / ratio_lo(dB)
	std::array <float, 3>               // Knee formula with log2(vol) as input and log2(vol) as output. Index = coef order
	               _knee_coef_arr;
	float          _knee_th_abs;        // knee_lvl(dB) * 0.5 / 6.0206. * 0.5 because it's a radius.

	dsp::dyn::SCPower <AddProc <1> >
	               _sc_power_1;
	dsp::dyn::SCPower <AddProc <2> >
	               _sc_power_2;
	SplBuf         _buf_tmp;
	float          _cur_gain;           // Stored as log2.

	static const float
	               _gain_min_l2;
	static const float
	               _gain_max_l2;
	static const float                  // Gain is always 0 dB for detected volum below this value.
	               _active_thr_l2;
	static const float
	               _active_mul;
	static const float                  // Reference level, does not move when the ratio are changed.
	               _lvl_ref_l2;




/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Compex (const Compex &other)            = delete;
	Compex &       operator = (const Compex &other)        = delete;
	bool           operator == (const Compex &other) const = delete;
	bool           operator != (const Compex &other) const = delete;

}; // class Compex



}  // namespace cpx
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/cpx/Compex.hpp"



#endif   // mfx_pi_cpx_Compex_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
