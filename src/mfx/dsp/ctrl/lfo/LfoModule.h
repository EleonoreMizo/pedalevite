/*****************************************************************************

        LfoModule.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ctrl_lfo_LfoModule_HEADER_INCLUDED)
#define mfx_dsp_ctrl_lfo_LfoModule_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/ctrl/lfo/OscInterface.h"

#include <array>
#include <memory>

#include <cstdint>



namespace mfx
{
namespace dsp
{
namespace ctrl
{
namespace lfo
{



class OscStepSeq;

class LfoModule
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Type
	{
		Type_INVALID = -1,

		Type_SINE = 0,
		Type_TRIANGLE,
		Type_SQUARE,
		Type_SAW,
		Type_PARABOLA,
		Type_N_PHASE,
		Type_BIPHASE,
		Type_VARISLOPE,
		Type_NOISE_FLT1,
		Type_NOISE_FLT2,
		Type_STEP_SEQ,

		Type_NBR_ELT
	};

	               LfoModule ();
	virtual        ~LfoModule ();

	void           set_sample_freq (double sample_freq);
	void           set_period (double per);
	void           set_phase (double phase);
	void           set_chaos (double chaos);
	void           set_phase_dist (double dist);
	void           set_phase_dist_offset (double ofs);
	void           set_sign (bool inv_flag);
	void           set_polarity (bool unipolar_flag);
	void           set_variation (int param, double val);
	bool           is_using_variation (int param) const;
	void           set_type (Type type);
	void           set_snh (double ratio);
	void           set_smooth (double ratio);
	void           set_step_seq (bool flag);
	bool           has_step_seq () const;
	OscStepSeq &   use_step_seq ();
	void           tick (int nbr_spl);
	double         get_val () const;
	double         get_phase () const;
	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const size_t  _storage_size = 256; // Real storage size is reduced by _storage_alig - 1
	static const size_t  _storage_alig =  16;
	static const int     _smooth_order =   3; // Smoothing order, > 0. 1 is responsive, 3 is smoother
	typedef std::array <uint8_t, _storage_size> OscStorage;

	typedef std::unique_ptr <OscStepSeq> StepSeqUPtr;

	OscInterface * get_aligned_osc () const;
	inline const OscInterface &
	               use_osc () const;
	inline OscInterface &
	               use_osc ();
	void           apply_osc_settings ();
	void           update_snh ();
	void           update_smooth ();
	inline void    tick_sub (int nbr_spl);

	OscStorage     _osc;

	// Real parameters
	double         _sample_freq;		// Hz, > 0
	double         _period;				// s, > 0
	double         _phase_chaos;
	double         _phase_dist;
	double         _phase_dist_ofs;
	bool           _inv_flag;
	bool           _unipolar_flag;
	std::array <double, OscInterface::Variation_NBR_ELT>
	               _variation_arr;	// [0 ; 1]
	double         _snh_ratio;			// ratio relative to _period, >= 0. 0 = no Sample & Hold
	double         _smooth;				// ratio relative to _period, >= 0. 0 = no smoothing
	Type           _type;				// -1 = no oscillator constructed
	StepSeqUPtr    _step_seq_uptr;
	OscInterface * _osc_ptr;

	// Internal variables
	bool           _snh_flag;
	bool           _smooth_flag;

	double         _snh_pos;			// [0 ; 1[
	double         _snh_step;			// Step per sample
	double         _snh_state;

	std::array <double, _smooth_order>
	               _smooth_state_arr;
	double         _smooth_spl_coef;	// Smoothing LPF coefficient, per sample. ^n for n samples.



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               LfoModule (const LfoModule &other)         = delete;
	LfoModule &    operator = (const LfoModule &other)        = delete;
	bool           operator == (const LfoModule &other) const = delete;
	bool           operator != (const LfoModule &other) const = delete;

}; // class LfoModule



}  // namespace lfo
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/ctrl/lfo/LfoModule.hpp"



#endif   // mfx_dsp_ctrl_lfo_LfoModule_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
