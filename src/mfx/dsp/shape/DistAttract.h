/*****************************************************************************

        DistAttract.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_shape_DistAttract_HEADER_INCLUDED)
#define mfx_dsp_shape_DistAttract_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/dyn/EnvFollowerRmsSimple.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace shape
{



class DistAttract
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_sample_freq (double sample_freq) noexcept;
	void           clear_buffers () noexcept;
	inline float   process_sample (float x) noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum Polarity
	{
		Polarity_POS = 0,
		Polarity_NEG,

		Polarity_NBR_ELT
	};

	typedef std::array <float, Polarity_NBR_ELT> BipolarVal;

	inline void		attract (float x, float env_val) noexcept;

	static inline constexpr Polarity
						val_to_pol (float val) noexcept;
	static inline constexpr float
						pol_to_val (Polarity polarity) noexcept;

	float          _sample_freq   = 0;  // Hz. 0 = not set

	BipolarVal		_lvl_a {{ 1, -1 }};  // |a| < b
	float          _lvl_b         = 2.f;

	bool           _mad_flag      = false; // Mode
	float          _sign          = 1;  // for Mad mode
	float          _speed         = 0;  // for Mad mode
	float          _speed_lim_min = 1.0f / 256;
	float          _ratio_f       = 1;  // 44100 / sample_freq

	float          _cur_val       = 0;  // Output

	dyn::EnvFollowerRmsSimple
	               _env;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const DistAttract &other) const = delete;
	bool           operator != (const DistAttract &other) const = delete;

}; // class DistAttract



}  // namespace shape
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/shape/DistAttract.hpp"



#endif   // mfx_dsp_shape_DistAttract_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
