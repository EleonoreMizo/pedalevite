/*****************************************************************************

        OscNoiseFlt.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ctrl_lfo_OscNoiseFlt_HEADER_INCLUDED)
#define mfx_dsp_ctrl_lfo_OscNoiseFlt_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/ctrl/lfo/OscInterface.h"
#include "mfx/dsp/ctrl/lfo/PhaseGen.h"

#include <array>
#include <random>



namespace mfx
{
namespace dsp
{
namespace ctrl
{
namespace lfo
{



template <int ORDER>
class OscNoiseFlt final
:	public OscInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               OscNoiseFlt ();
	               OscNoiseFlt (const OscNoiseFlt &other) = default;
	               OscNoiseFlt (OscNoiseFlt &&other)      = default;
	               ~OscNoiseFlt ()                        = default;

	OscNoiseFlt &  operator = (const OscNoiseFlt &other)  = default;
	OscNoiseFlt &  operator = (OscNoiseFlt &&other)       = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::dsp::ctrl::lfo::OscInterface
	void           do_set_sample_freq (double sample_freq) final;
	void           do_set_period (double per) final;
	void           do_set_phase (double phase) final;
	void           do_set_chaos (double chaos) final;
	void           do_set_phase_dist (double dist) final;
	void           do_set_phase_dist_offset (double ofs) final;
	void           do_set_sign (bool inv_flag) final;
	void           do_set_polarity (bool unipolar_flag) final;
	void           do_set_variation (int param, double val) final;
	bool           do_is_using_variation (int param) const final;
	void           do_tick (int nbr_spl) final;
	double         do_get_val () const final;
	double         do_get_phase () const final;
	void           do_clear_buffers () final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::array <double, ORDER + 1> StateArray;

	void           update_period ();
	void           gen_new_val ();

	PhaseGen       _phase_gen;
	std::minstd_rand
	               _rnd_gen;
	std::uniform_real_distribution <double>
	               _rnd_dist;
	double         _amp;
	StateArray     _cur_val_arr;
	double         _filter_coef;
	int            _noise_pos;
	int            _noise_res;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const OscNoiseFlt &other) const = delete;
	bool           operator != (const OscNoiseFlt &other) const = delete;

}; // class OscNoiseFlt



}  // namespace lfo
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/ctrl/lfo/OscNoiseFlt.hpp"



#endif   // mfx_dsp_ctrl_lfo_OscNoiseFlt_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
