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

	               OscNoiseFlt () noexcept;
	               OscNoiseFlt (const OscNoiseFlt &other) = default;
	               OscNoiseFlt (OscNoiseFlt &&other)      = default;
	               ~OscNoiseFlt ()                        = default;

	OscNoiseFlt &  operator = (const OscNoiseFlt &other)  = default;
	OscNoiseFlt &  operator = (OscNoiseFlt &&other)       = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::dsp::ctrl::lfo::OscInterface
	void           do_set_sample_freq (double sample_freq) noexcept final;
	void           do_set_period (double per) noexcept final;
	void           do_set_phase (double phase) noexcept final;
	void           do_set_chaos (double chaos) noexcept final;
	void           do_set_phase_dist (double dist) noexcept final;
	void           do_set_phase_dist_offset (double ofs) noexcept final;
	void           do_set_sign (bool inv_flag) noexcept final;
	void           do_set_polarity (bool unipolar_flag) noexcept final;
	void           do_set_variation (int param, double val) noexcept final;
	bool           do_is_using_variation (int param) const noexcept final;
	void           do_tick (int nbr_spl) noexcept final;
	double         do_get_val () const noexcept final;
	double         do_get_phase () const noexcept final;
	void           do_clear_buffers () noexcept final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::array <double, ORDER + 1> StateArray;

	void           update_period () noexcept;
	void           gen_new_val () noexcept;

	PhaseGen       _phase_gen;
	std::minstd_rand
	               _rnd_gen;
	std::uniform_real_distribution <double>
	               _rnd_dist { -1, 1 };
	double         _amp         = 0;
	StateArray     _cur_val_arr {};
	double         _filter_coef = 0;
	int            _noise_pos   = 0;
	int            _noise_res   = 64;



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
