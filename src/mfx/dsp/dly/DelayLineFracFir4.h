/*****************************************************************************

        DelayLineFracFir4.h
        Author: Laurent de Soras, 2019

Template parameters:

- IM is a class generating the 4 FIR coefficients for an interpolator.
It must have the following functions:

	IM::IM ();
	void IM::make_interpolator (float fir_ptr [], float pos) noexcept;
	int IM::get_length () noexcept;
	int IM::get_delay () noexcept;

- AL is a standard allocator for the internal buffer.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_dly_DelayLineFracFir4_HEADER_INCLUDED)
#define mfx_dsp_dly_DelayLineFracFir4_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "mfx/dsp/dly/DelayLineFracFir4Base.h"
#include "mfx/dsp/rspl/InterpFirMakerLagrange4.h"

#include <array>
#include <memory>



namespace mfx
{
namespace dsp
{
namespace dly
{



template <typename IM = rspl::InterpFirMakerLagrange4 <float>, typename AL = std::allocator <float> >
class DelayLineFracFir4
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef IM InterpolatorMaker;
	typedef AL AllocatorType;

	explicit       DelayLineFracFir4 (const AllocatorType &al = AllocatorType ());
	virtual        ~DelayLineFracFir4 () = default;

	void           set_sample_freq (float sample_freq);
	inline float   get_sample_freq () const noexcept;
	void           set_max_time (float freq);
	inline float   get_max_time () const noexcept;
	inline float   get_min_time () const noexcept;
	void           set_time (float per) noexcept;
	inline float   get_time () const noexcept;
	inline InterpolatorMaker &
	               use_interpolator_maker () noexcept;
	inline const InterpolatorMaker &
	               use_interpolator_maker () const noexcept;

	fstb_FORCEINLINE float
	               process_sample (float sample) noexcept;
	void           process_block (float dest_ptr [], const float src_ptr [], int nbr_spl) noexcept;
	void           process_block_mix (float dest_ptr [], const float src_ptr [], int nbr_spl) noexcept;
	void           process_block_vt (float dest_ptr [], const float src_ptr [], int nbr_spl, float final_time) noexcept;
	void           process_block_vt_mix (float dest_ptr [], const float src_ptr [], int nbr_spl, float final_time) noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_buffer ();
	void           update_period () noexcept;
	fstb_FORCEINLINE void
	               update_interpolator () noexcept;
	inline float	init_variable_time (int nbr_spl, float final_time) noexcept;
	fstb_FORCEINLINE float
	               iterate_variable_time (float period_step, float sample) noexcept;

	std::array <float, 4>         // Base
	               _interp_fir;
	DelayLineFracFir4Base <float, AllocatorType>
	               _buf;
	float          _sample_freq;  // Hz, > 0
	float          _real_period;  // s
	float          _period;       // In samples, plus the interpolator delay, >= 4 - interp.delay
	float          _max_time;
	int            _period_int;
	InterpolatorMaker
	               _interpolator_maker;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DelayLineFracFir4 ()                               = delete;
	               DelayLineFracFir4 (const DelayLineFracFir4 &other) = delete;
	DelayLineFracFir4 &
	               operator = (const DelayLineFracFir4 &other)        = delete;
	bool           operator == (const DelayLineFracFir4 &other) const = delete;
	bool           operator != (const DelayLineFracFir4 &other) const = delete;

}; // class DelayLineFracFir4



}  // namespace dly
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/dly/DelayLineFracFir4.hpp"



#endif   // mfx_dsp_dly_DelayLineFracFir4_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
