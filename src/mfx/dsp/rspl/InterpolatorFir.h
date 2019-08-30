/*****************************************************************************

        InterpolatorFir.h
        Author: Laurent de Soras, 2016

Template parameters:

- IP: Interpolator-convolver, for example InterpFirPolyphase.
	Should have the following members:
	static const int IP::PHASE_LEN
	float IP::interpolate (const float data_ptr [], uint32_t frac_pos);
	void IP::interpolate_multi_chn (float * const out_ptr_arr [], int out_offset, const float * const in_ptr_arr [], int in_offset, uint32_t frac_pos, int nbr_chn);

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_rspl_InterpolatorFir_HEADER_INCLUDED)
#define	mfx_dsp_rspl_InterpolatorFir_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/FixedPoint.h"
#include "mfx/dsp/rspl/InterpolatorInterface.h"
#include "mfx/dsp/rspl/SnhTool.h"



namespace mfx
{
namespace dsp
{
namespace rspl
{



template <class IT>
class InterpolatorFir
:	public InterpolatorInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef IT Convolver;

	static const int PHASE_LEN = Convolver::PHASE_LEN;

	               InterpolatorFir ();
	virtual        ~InterpolatorFir () {}

	void           set_convolver (IT &convolver);
	void           set_group_delay (double grp_dly);

	bool           is_ready () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// InterpolatorInterface
	virtual void   do_set_ovrspl_l2 (int ovrspl_l2);
	virtual int    do_get_impulse_len () const;
	virtual fstb::FixedPoint
	               do_get_group_delay () const;

	virtual void   do_start (int nbr_chn);
	virtual int    do_process_block (float * const dest_ptr_arr [], const float * const src_ptr_arr [], int pos_dest, fstb::FixedPoint pos_src, int end_dest, int beg_src, int end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step);
	virtual float  do_process_sample (const float src_ptr [], fstb::FixedPoint pos_src, fstb::FixedPoint rate);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	int            process_block_multi_chn (float * const dest_ptr_arr [], const float * const src_ptr_arr [], int pos_dest, fstb::FixedPoint pos_src, int end_dest, int beg_src, int end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step);
	int            process_block_mono (float dest_ptr [], const float src_ptr [], int pos_dest, fstb::FixedPoint pos_src, int end_dest, int beg_src, int end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step);

	int            process_block_multi_chn_sparse (float * const dest_ptr_arr [], const float * const src_ptr_arr [], int pos_dest, fstb::FixedPoint pos_src, int end_dest, int beg_src, int end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step, int hold_time, int rep_index);
	int            process_block_mono_sparse (float dest_ptr [], const float src_ptr [], int pos_dest, fstb::FixedPoint pos_src, int end_dest, int beg_src, int end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step, int hold_time, int rep_index);

	SnhTool        _snh_tool;
	fstb::FixedPoint
	               _grp_dly;
	Convolver *    _conv_ptr;  // 0 = not initialised
	int            _nbr_chn;
	int            _ovrspl_l2; // Log base 2 of the oversampling. >= 0.



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               InterpolatorFir (const InterpolatorFir &other);
	InterpolatorFir &
	               operator = (const InterpolatorFir &other);
	bool           operator == (const InterpolatorFir &other);
	bool           operator != (const InterpolatorFir &other);

};	// class InterpolatorFir



}	// namespace rspl
}	// namespace dsp
}	// namespace mfx



#include "mfx/dsp/rspl/InterpolatorFir.hpp"



#endif	// mfx_dsp_rspl_InterpolatorFir_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
