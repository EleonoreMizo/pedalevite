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

#include "fstb/def.h"

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "fstb/DataAlign.h"
#include "fstb/SingleObj.h"
#include "fstb/ToolsSimd.h"
#include "mfx/dsp/dyn/EnvFollowerARHelper.h"
#include "mfx/dsp/dyn/SCPower.h"
#include "mfx/pi/cpx/CompexDesc.h"
#include "mfx/pi/cpx/CompexGainFnc.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/piapi/PluginInterface.h"

#if defined (fstb_HAS_SIMD)
	#include "mfx/dsp/iir/Biquad4Simd.h"
	#include <vector>
#else
	#include "mfx/dsp/iir/Biquad.h"
#endif

#include <array>



namespace mfx
{
namespace pi
{
namespace cpx
{



class Compex final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       Compex (piapi::HostInterface &host);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	// Is also the buffer length. Must be a multiple of 4
	static constexpr int _update_resol = 64;

	typedef dsp::dyn::EnvFollowerARHelper <1> EnvFollower;

#if defined (fstb_HAS_SIMD)

	typedef std::vector <float, fstb::AllocAlign <float, 16> > SplBuf;

	typedef dsp::iir::Biquad4Simd <
		fstb::DataAlign <true>,
		fstb::DataAlign <true>,
		fstb::DataAlign <true>
	> Smoother;

	template <class T>
	using WrapperAlign = fstb::SingleObj <T, fstb::AllocAlign <T, 16> >;

#else // fstb_HAS_SIMD

	typedef std::array <float, _update_resol> SplBuf;

	class Smoother
	{
	public:
		static constexpr int _nbr_units = 4;
		void           set_z_eq_same (const float bz [3], const float az [3]);
		void           clear_buffers ();
		void           process_block_serial_immediate (float dst_ptr [], const float src_ptr [], int nbr_spl);
	private:
		typedef std::array <dsp::iir::Biquad, _nbr_units> BiquadArray;
		BiquadArray    _biq_arr;
	};

	template <class T>
	using WrapperAlign = T;

#endif // fstb_HAS_SIMD

	template <int NC>
	class AddProc
	{
	public:
		static fstb_FORCEINLINE constexpr float
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

	template <class T>
	static fstb_FORCEINLINE T &
	               usew (WrapperAlign <T> &wrap);

	piapi::HostInterface &
	               _host;
	State          _state;

	CompexDesc     _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
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

	WrapperAlign <EnvFollower>
						_env_fol_xptr;
	WrapperAlign <Smoother>
						_smoother_xptr;
	bool				_use_side_chain_flag;

	CompexGainFnc  _gain_fnc;

	dsp::dyn::SCPower <AddProc <1> >    // Power extraction from the analysed signal, mono
	               _sc_power_1;
	dsp::dyn::SCPower <AddProc <2> >    // Same for setero signals
	               _sc_power_2;
	SplBuf         _buf_tmp;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Compex ()                               = delete;
	               Compex (const Compex &other)            = delete;
	               Compex (Compex &&other)                 = delete;
	Compex &       operator = (const Compex &other)        = delete;
	Compex &       operator = (Compex &&other)             = delete;
	bool           operator == (const Compex &other) const = delete;
	bool           operator != (const Compex &other) const = delete;

}; // class Compex



}  // namespace cpx
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/cpx/Compex.hpp"



#endif   // mfx_pi_cpx_Compex_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
