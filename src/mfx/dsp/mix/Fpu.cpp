/*****************************************************************************

        Fpu.cpp
        Author: Laurent de Soras, 2002

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/mix/Fpu.h"
#include "mfx/dsp/StereoLevel.h"

#include <cassert>
#include <cstring>



namespace mfx
{
namespace dsp
{
namespace mix
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*****************************************************************************
*
*       SCALE
*
*****************************************************************************/



void	Fpu::scale_1_v (float data_ptr [], int nbr_spl, float vol)
{
	assert (data_ptr != 0);
	assert (nbr_spl > 0);

	if (vol == 0.0f)
	{
		clear (data_ptr, nbr_spl);
	}
	else  if (vol != 1.0f)
	{
		int				pos = 0;
		do
		{
			data_ptr [pos] *= vol;
			++ pos;
		}
		while (pos < nbr_spl);
	}
}



void	Fpu::scale_1_vlr (float data_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	assert (data_ptr != 0);
	assert (nbr_spl > 0);

	const float		step = (e_vol - s_vol) / nbr_spl;
	int				pos = 0;
	do
	{
		data_ptr [pos] *= s_vol;
		s_vol += step;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::scale_1_vlrauto (float data_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	if (e_vol == s_vol)
	{
		scale_1_v (data_ptr, nbr_spl, s_vol);
	}
	else
	{
		scale_1_vlr (data_ptr, nbr_spl, s_vol, e_vol);
	}
}



void	Fpu::scale_2_v (float data_1_ptr [], float data_2_ptr [], int nbr_spl, float vol)
{
	assert (data_1_ptr != 0);
	assert (data_2_ptr != 0);

	if (vol == 0.0f)
	{
		clear (data_1_ptr, nbr_spl);
		clear (data_2_ptr, nbr_spl);
	}
	else if (vol != 1.0f)
	{
		int				pos = 0;
		do
		{
			data_1_ptr [pos] *= vol;
			data_2_ptr [pos] *= vol;
			++ pos;
		}
		while (pos < nbr_spl);
	}
}



void	Fpu::scale_2_vlr (float data_1_ptr [], float data_2_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	assert (data_1_ptr != 0);
	assert (data_2_ptr != 0);
	assert (nbr_spl > 0);

	const float		step = (e_vol - s_vol) / nbr_spl;
	int				pos = 0;
	do
	{
		data_1_ptr [pos] *= s_vol;
		data_2_ptr [pos] *= s_vol;
		s_vol += step;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::scale_2_vlrauto (float data_1_ptr [], float data_2_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	if (e_vol == s_vol)
	{
		scale_2_v (data_1_ptr, data_2_ptr, nbr_spl, s_vol);
	}
	else
	{
		scale_2_vlr (data_1_ptr, data_2_ptr, nbr_spl, s_vol, e_vol);
	}
}



/*****************************************************************************
*
*       COPY
*
*****************************************************************************/



void	Fpu::copy_1_1 (float out_ptr [], const float in_ptr [], int nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	using namespace std;

	memcpy (out_ptr, in_ptr, nbr_spl * sizeof (out_ptr [0]));
}



void	Fpu::copy_1_1_v (float out_ptr [], const float in_ptr [], int nbr_spl, float vol)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	if (vol == 0.0f)
	{
		clear (out_ptr, nbr_spl);
	}
	else if (vol == 1.0f)
	{
		copy_1_1 (out_ptr, in_ptr, nbr_spl);
	}
	else
	{
		int				pos = 0;
		do
		{
			out_ptr [pos] = in_ptr [pos] * vol;
			++ pos;
		}
		while (pos < nbr_spl);
	}
}



void	Fpu::copy_1_1_vlr (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	const float		step = (e_vol - s_vol) / nbr_spl;
	int				pos = 0;
	do
	{
		out_ptr [pos] = in_ptr [pos] * s_vol;
		s_vol += step;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_1_1_vlrauto (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	if (e_vol == s_vol)
	{
		copy_1_1_v (out_ptr, in_ptr, nbr_spl, s_vol);
	}
	else
	{
		copy_1_1_vlr (out_ptr, in_ptr, nbr_spl, s_vol, e_vol);
	}
}



void	Fpu::copy_1_2 (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	using namespace std;

	memcpy (out_1_ptr, in_ptr, nbr_spl * sizeof (out_1_ptr [0]));
	memcpy (out_2_ptr, in_ptr, nbr_spl * sizeof (out_2_ptr [0]));
}



void	Fpu::copy_1_2_v (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	if (vol == 0.0f)
	{
		clear (out_1_ptr, nbr_spl);
		clear (out_2_ptr, nbr_spl);
	}
	else if (vol == 1.0f)
	{
		copy_1_2 (out_1_ptr, out_2_ptr, in_ptr, nbr_spl);
	}
	else
	{
		int				pos = 0;
		do
		{
			const float		temp = in_ptr [pos] * vol;
			out_1_ptr [pos] = temp;
			out_2_ptr [pos] = temp;
			++ pos;
		}
		while (pos < nbr_spl);
	}
}



void	Fpu::copy_1_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	const float		step = (e_vol - s_vol) / nbr_spl;
	int				pos = 0;
	do
	{
		const float		temp = in_ptr [pos] * s_vol;
		out_1_ptr [pos] = temp;
		out_2_ptr [pos] = temp;
		s_vol += step;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_1_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	if (e_vol == s_vol)
	{
		copy_1_2_v (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol);
	}
	else
	{
		copy_1_2_vlr (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol, e_vol);
	}
}



void	Fpu::copy_2_1 (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		out_ptr [pos] = in_1_ptr [pos] + in_2_ptr [pos];
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_2_1_v (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol)
{
	assert (out_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	if (vol == 0.0f)
	{
		clear (out_ptr, nbr_spl);
	}
	else if (vol == 1.0f)
	{
		copy_2_1 (out_ptr, in_1_ptr, in_2_ptr, nbr_spl);
	}
	else
	{
		int				pos = 0;
		do
		{
			out_ptr [pos] = (in_1_ptr [pos] + in_2_ptr [pos]) * vol;
			++ pos;
		}
		while (pos < nbr_spl);
	}
}



void	Fpu::copy_2_1_vlr (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	assert (out_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	const float		step = (e_vol - s_vol) / nbr_spl;
	int				pos = 0;
	do
	{
		out_ptr [pos] = (in_1_ptr [pos] + in_2_ptr [pos]) * s_vol;
		s_vol += step;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_2_1_vlrauto (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	if (e_vol == s_vol)
	{
		copy_2_1_v (out_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol);
	}
	else
	{
		copy_2_1_vlr (out_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol, e_vol);
	}
}



void	Fpu::copy_2_2 (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	using namespace std;

	memcpy (out_1_ptr, in_1_ptr, nbr_spl * sizeof (out_1_ptr [0]));
	memcpy (out_2_ptr, in_2_ptr, nbr_spl * sizeof (out_2_ptr [0]));
}



void	Fpu::copy_2_2_v (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	if (vol == 0.0f)
	{
		clear (out_1_ptr, nbr_spl);
		clear (out_2_ptr, nbr_spl);
	}
	else if (vol == 1.0f)
	{
		copy_2_2 (out_1_ptr, out_2_ptr, in_1_ptr, in_2_ptr, nbr_spl);
	}
	else
	{
		int				pos = 0;
		do
		{
			const float		x_1 = in_1_ptr [pos];
			const float		x_2 = in_2_ptr [pos];
			out_1_ptr [pos] = x_1 * vol;
			out_2_ptr [pos] = x_2 * vol;
			++ pos;
		}
		while (pos < nbr_spl);
	}
}



void	Fpu::copy_2_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	const float		step = (e_vol - s_vol) / nbr_spl;
	int				pos = 0;
	do
	{
		const float		x_1 = in_1_ptr [pos];
		const float		x_2 = in_2_ptr [pos];
		out_1_ptr [pos] = x_1 * s_vol;
		out_2_ptr [pos] = x_2 * s_vol;
		s_vol += step;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_2_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	if (e_vol == s_vol)
	{
		copy_2_2_v (out_1_ptr, out_2_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol);
	}
	else
	{
		copy_2_2_vlr (out_1_ptr, out_2_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol, e_vol);
	}
}



/*****************************************************************************
*
*       SPREAD COPYING
*
*****************************************************************************/



void	Fpu::copy_spread_1_2_v (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol_l, float vol_r)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x = in_ptr [pos];
		out_1_ptr [pos] = x * vol_l;
		out_2_ptr [pos] = x * vol_r;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_spread_1_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	const float		divisor = 1.0f / nbr_spl;
	const float		step_l = (e_vol_l - s_vol_l) * divisor;
	const float		step_r = (e_vol_r - s_vol_r) * divisor;

	int				pos = 0;
	do
	{
		const float		x = in_ptr [pos];
		out_1_ptr [pos] = x * s_vol_l;
		out_2_ptr [pos] = x * s_vol_r;

		++ pos;
		s_vol_l += step_l;
		s_vol_r += step_r;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_spread_1_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r)
{
	if (e_vol_l == s_vol_l && e_vol_r == s_vol_r)
	{
		copy_spread_1_2_v (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol_l, s_vol_r);
	}
	else
	{
		copy_spread_1_2_vlr (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol_l, s_vol_r, e_vol_l, e_vol_r);
	}
}



/*****************************************************************************
*
*       CROSS-FADE COPYING
*
*****************************************************************************/



void	Fpu::copy_xfade_2_1_v (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float xf)
{
	assert (out_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x_1 = in_1_ptr [pos];
		const float		x_2 = in_2_ptr [pos];
		out_ptr [pos] = x_1 + (x_2 - x_1) * xf;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_xfade_2_1_vlr (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_xf, float e_xf)
{
	assert (out_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	const float		step = (e_xf - s_xf) / nbr_spl;
	int				pos = 0;
	do
	{
		const float		x_1 = in_1_ptr [pos];
		const float		x_2 = in_2_ptr [pos];
		out_ptr [pos] = x_1 + (x_2 - x_1) * s_xf;
		s_xf += step;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_xfade_2_1_vlrauto (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_xf, float e_xf)
{
	if (e_xf == s_xf)
	{
		copy_xfade_2_1_v (out_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_xf);
	}
	else
	{
		copy_xfade_2_1_vlr (out_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_xf, e_xf);
	}
}



void	Fpu::copy_xfade_3_1 (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], const float in_3_ptr [], int nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (in_3_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x_1 = in_1_ptr [pos];
		const float		x_2 = in_2_ptr [pos];
		const float    m   = in_3_ptr [pos];
		out_ptr [pos] = x_1 + (x_2 - x_1) * m;
		++ pos;
	}
	while (pos < nbr_spl);
}



/*****************************************************************************
*
*       MATRIX COPYING
*
*****************************************************************************/



void	Fpu::copy_mat_2_2_v (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &vol)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x_1 = in_1_ptr [pos];
		const float		x_2 = in_2_ptr [pos];

		out_1_ptr [pos] =   x_1 * vol.get_l2l ()
		                  + x_2 * vol.get_r2l ();
		out_2_ptr [pos] =   x_1 * vol.get_l2r ()
		                  + x_2 * vol.get_r2r ();
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_mat_2_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	StereoLevel		vol = s_vol;
	StereoLevel		step = e_vol;
	step -= s_vol;
	step *= 1.0f / nbr_spl;

	int				pos = 0;
	do
	{
		const float		x_1 = in_1_ptr [pos];
		const float		x_2 = in_2_ptr [pos];

		out_1_ptr [pos] =   x_1 * vol.get_l2l ()
		                  + x_2 * vol.get_r2l ();
		out_2_ptr [pos] =   x_1 * vol.get_l2r ()
		                  + x_2 * vol.get_r2r ();
		++ pos;
		vol += step;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_mat_2_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol)
{
	if (e_vol == s_vol)
	{
		copy_mat_2_2_v (out_1_ptr, out_2_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol);
	}
	else
	{
		copy_mat_2_2_vlr (out_1_ptr, out_2_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol, e_vol);
	}
}



/*****************************************************************************
*
*       COPY AND INTERLEAVE
*
*****************************************************************************/



void	Fpu::copy_1_2i (float out_ptr [], const float in_ptr [], int nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x = in_ptr [pos];
		out_ptr [pos * 2    ] = x;
		out_ptr [pos * 2 + 1] = x;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_1_2i_v (float out_ptr [], const float in_ptr [], int nbr_spl, float vol)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x = in_ptr [pos] * vol;
		out_ptr [pos * 2    ] = x;
		out_ptr [pos * 2 + 1] = x;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_1_2i_vlr (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	const float		step = (e_vol - s_vol) / nbr_spl;
	int				pos = 0;
	do
	{
		const float		x = in_ptr [pos] * s_vol;
		out_ptr [pos * 2    ] = x;
		out_ptr [pos * 2 + 1] = x;
		s_vol += step;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_1_2i_vlrauto (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	if (e_vol == s_vol)
	{
		copy_1_2i_v (out_ptr, in_ptr, nbr_spl, s_vol);
	}
	else
	{
		copy_1_2i_vlr (out_ptr, in_ptr, nbr_spl, s_vol, e_vol);
	}
}



void	Fpu::copy_2_2i (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x_1 = in_1_ptr [pos];
		const float		x_2 = in_2_ptr [pos];
		out_ptr [pos * 2    ] = x_1;
		out_ptr [pos * 2 + 1] = x_2;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_2_2i_v (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol)
{
	assert (out_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x_1 = in_1_ptr [pos];
		const float		x_2 = in_2_ptr [pos];
		out_ptr [pos * 2    ] = x_1 * vol;
		out_ptr [pos * 2 + 1] = x_2 * vol;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_2_2i_vlr (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	assert (out_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	const float		step = (e_vol - s_vol) / nbr_spl;
	int				pos = 0;
	do
	{
		const float		x_1 = in_1_ptr [pos];
		const float		x_2 = in_2_ptr [pos];
		out_ptr [pos * 2    ] = x_1 * s_vol;
		out_ptr [pos * 2 + 1] = x_2 * s_vol;
		s_vol += step;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_2_2i_vlrauto (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	if (e_vol == s_vol)
	{
		copy_2_2i_v (out_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol);
	}
	else
	{
		copy_2_2i_vlr (out_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol, e_vol);
	}
}



void	Fpu::copy_4_4i (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], const float in_3_ptr [], const float in_4_ptr [], int nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (in_3_ptr != 0);
	assert (in_4_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		out_ptr [pos * 4    ] = in_1_ptr [pos];
		out_ptr [pos * 4 + 1] = in_2_ptr [pos];
		out_ptr [pos * 4 + 2] = in_3_ptr [pos];
		out_ptr [pos * 4 + 3] = in_4_ptr [pos];

		++ pos;
	}
	while (pos < nbr_spl);
}



// Content of the channels 3 and 4 is not specified.
void	Fpu::copy_2_4i2 (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		out_ptr [pos * 4    ] = in_1_ptr [pos];
		out_ptr [pos * 4 + 1] = in_2_ptr [pos];

		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_1_ni1 (float out_ptr [], const float in_ptr [], int nbr_spl, int nbr_chn_out)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);
	assert (nbr_chn_out > 0);

	int				pos = 0;
	do
	{
		out_ptr [pos * nbr_chn_out] = in_ptr [pos];
		++ pos;
	}
	while (pos < nbr_spl);
}



/*****************************************************************************
*
*       COPY AND DEINTERLEAVE
*
*****************************************************************************/



void	Fpu::copy_2i_1 (float out_ptr [], const float in_ptr [], int nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		out_ptr [pos] = in_ptr [pos * 2] + in_ptr [pos * 2 + 1];
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_2i_1_v (float out_ptr [], const float in_ptr [], int nbr_spl, float vol)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		out_ptr [pos] = (in_ptr [pos * 2] + in_ptr [pos * 2 + 1]) * vol;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_2i_1_vlr (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	const float		step = (e_vol - s_vol) / nbr_spl;
	int				pos = 0;
	do
	{
		out_ptr [pos] = (in_ptr [pos * 2] + in_ptr [pos * 2 + 1]) * s_vol;
		s_vol += step;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_2i_1_vlrauto (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	if (e_vol == s_vol)
	{
		copy_2i_1_v (out_ptr, in_ptr, nbr_spl, s_vol);
	}
	else
	{
		copy_2i_1_vlr (out_ptr, in_ptr, nbr_spl, s_vol, e_vol);
	}
}



void	Fpu::copy_2i_2 (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x_1 = in_ptr [pos * 2    ];
		const float		x_2 = in_ptr [pos * 2 + 1];
		out_1_ptr [pos] = x_1;
		out_2_ptr [pos] = x_2;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_2i_2_v (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x_1 = in_ptr [pos * 2    ];
		const float		x_2 = in_ptr [pos * 2 + 1];
		out_1_ptr [pos] = x_1 * vol;
		out_2_ptr [pos] = x_2 * vol;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_2i_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	const float		step = (e_vol - s_vol) / nbr_spl;
	int				pos = 0;
	do
	{
		const float		x_1 = in_ptr [pos * 2    ];
		const float		x_2 = in_ptr [pos * 2 + 1];
		out_1_ptr [pos] = x_1 * s_vol;
		out_2_ptr [pos] = x_2 * s_vol;
		s_vol += step;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_2i_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	if (e_vol == s_vol)
	{
		copy_2i_2_v (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol);
	}
	else
	{
		copy_2i_2_vlr (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol, e_vol);
	}
}



void	Fpu::copy_4i_1 (float out_ptr [], const float in_ptr [], int nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		val_0 = in_ptr [pos * 4    ];
		const float		val_1 = in_ptr [pos * 4 + 1];
		const float		val_2 = in_ptr [pos * 4 + 2];
		const float		val_3 = in_ptr [pos * 4 + 3];
		const float		val = val_0 + val_1 + val_2 + val_3;
		out_ptr [pos] = val;

		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_4i_4 (float out_1_ptr [], float out_2_ptr [], float out_3_ptr [], float out_4_ptr [], const float in_ptr [], int nbr_spl)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (out_3_ptr != 0);
	assert (out_4_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		out_1_ptr [pos] = in_ptr [pos * 4    ];
		out_2_ptr [pos] = in_ptr [pos * 4 + 1];
		out_3_ptr [pos] = in_ptr [pos * 4 + 2];
		out_4_ptr [pos] = in_ptr [pos * 4 + 3];

		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_4i2_2 (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		out_1_ptr [pos] = in_ptr [pos * 4    ];
		out_2_ptr [pos] = in_ptr [pos * 4 + 1];

		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::copy_ni1_1 (float out_ptr [], const float in_ptr [], int nbr_spl, int nbr_chn_in)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);
	assert (nbr_chn_in > 0);

	int				pos = 0;
	do
	{
		out_ptr [pos] = in_ptr [pos * nbr_chn_in];
		++ pos;
	}
	while (pos < nbr_spl);
}



/*****************************************************************************
*
*       COPY AND CONVERT INTERLEAVING
*
*****************************************************************************/



void	Fpu::copy_nip_mip (float out_ptr [], const float in_ptr [], int nbr_spl, int nbr_chn_out, int nbr_chn_in, int nbr_chn_copy)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);
	assert (nbr_chn_out > 0);
	assert (nbr_chn_in > 0);
	assert (nbr_chn_copy > 0);
	assert (nbr_chn_copy <= nbr_chn_in);
	assert (nbr_chn_copy <= nbr_chn_out);

	const int		len_spl_in = nbr_spl * nbr_chn_in;
	int				pos_in = 0;
	int				pos_out = 0;
	do
	{
		int				chn_cnt = 0;
		do
		{
			out_ptr [pos_out + chn_cnt] = in_ptr [pos_in + chn_cnt];
			++ chn_cnt;
		}
		while (chn_cnt < nbr_chn_copy);

		pos_in += nbr_chn_in;
		pos_out += nbr_chn_out;
	}
	while (pos_in < len_spl_in);
}



/*****************************************************************************
*
*       MIX
*
*****************************************************************************/



void	Fpu::mix_1_1 (float out_ptr [], const float in_ptr [], int nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		out_ptr [pos] += in_ptr [pos];
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_1_1_v (float out_ptr [], const float in_ptr [], int nbr_spl, float vol)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	if (vol == 1.0f)
	{
		mix_1_1 (out_ptr, in_ptr, nbr_spl);
	}
	else if (vol != 0.0f)
	{
		int				pos = 0;
		do
		{
			out_ptr [pos] += in_ptr [pos] * vol;
			++ pos;
		}
		while (pos < nbr_spl);
	}
}



void	Fpu::mix_1_1_vlr (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	const float		step = (e_vol - s_vol) / nbr_spl;
	int				pos = 0;
	do
	{
		out_ptr [pos] += in_ptr [pos] * s_vol;
		s_vol += step;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_1_1_vlrauto (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	if (e_vol == s_vol)
	{
		mix_1_1_v (out_ptr, in_ptr, nbr_spl, s_vol);
	}
	else
	{
		mix_1_1_vlr (out_ptr, in_ptr, nbr_spl, s_vol, e_vol);
	}
}



void	Fpu::mix_1_2 (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x = in_ptr [pos];
		out_1_ptr [pos] += x;
		out_2_ptr [pos] += x;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_1_2_v (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	if (vol == 1.0f)
	{
		mix_1_2 (out_1_ptr, out_2_ptr, in_ptr, nbr_spl);
	}
	else if (vol != 0.0f)
	{
		int				pos = 0;
		do
		{
			const float		x = in_ptr [pos] * vol;
			out_1_ptr [pos] += x;
			out_2_ptr [pos] += x;
			++ pos;
		}
		while (pos < nbr_spl);
	}
}



void	Fpu::mix_1_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	const float		step = (e_vol - s_vol) / nbr_spl;
	int				pos = 0;
	do
	{
		const float		x = in_ptr [pos] * s_vol;
		out_1_ptr [pos] += x;
		out_2_ptr [pos] += x;
		s_vol += step;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_1_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	if (e_vol == s_vol)
	{
		mix_1_2_v (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol);
	}
	else
	{
		mix_1_2_vlr (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol, e_vol);
	}
}



void	Fpu::mix_2_1 (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		out_ptr [pos] += in_1_ptr [pos] + in_2_ptr [pos];
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_2_1_v (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol)
{
	assert (out_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	if (vol == 1.0f)
	{
		mix_2_1 (out_ptr, in_1_ptr, in_2_ptr, nbr_spl);
	}
	else if (vol != 0.0f)
	{
		int				pos = 0;
		do
		{
			out_ptr [pos] += (in_1_ptr [pos] + in_2_ptr [pos]) * vol;
			++ pos;
		}
		while (pos < nbr_spl);
	}
}



void	Fpu::mix_2_1_vlr (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	assert (out_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	const float		step = (e_vol - s_vol) / nbr_spl;
	int				pos = 0;
	do
	{
		out_ptr [pos] += (in_1_ptr [pos] + in_2_ptr [pos]) * s_vol;
		s_vol += step;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_2_1_vlrauto (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	if (e_vol == s_vol)
	{
		mix_2_1_v (out_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol);
	}
	else
	{
		mix_2_1_vlr (out_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol, e_vol);
	}
}



void	Fpu::mix_2_2 (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x_1 = in_1_ptr [pos];
		const float		x_2 = in_2_ptr [pos];
		out_1_ptr [pos] += x_1;
		out_2_ptr [pos] += x_2;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_2_2_v (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	if (vol == 1.0f)
	{
		mix_2_2 (out_1_ptr, out_2_ptr, in_1_ptr, in_2_ptr, nbr_spl);
	}
	else if (vol != 0.0f)
	{
		int				pos = 0;
		do
		{
			const float		x_1 = in_1_ptr [pos];
			const float		x_2 = in_2_ptr [pos];
			out_1_ptr [pos] += x_1 * vol;
			out_2_ptr [pos] += x_2 * vol;
			++ pos;
		}
		while (pos < nbr_spl);
	}
}



void	Fpu::mix_2_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	const float		step = (e_vol - s_vol) / nbr_spl;
	int				pos = 0;
	do
	{
		const float		x_1 = in_1_ptr [pos];
		const float		x_2 = in_2_ptr [pos];
		out_1_ptr [pos] += x_1 * s_vol;
		out_2_ptr [pos] += x_2 * s_vol;
		s_vol += step;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_2_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	if (e_vol == s_vol)
	{
		mix_2_2_v (out_1_ptr, out_2_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol);
	}
	else
	{
		mix_2_2_vlr (out_1_ptr, out_2_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol, e_vol);
	}
}



/*****************************************************************************
*
*       SPREAD MIXING
*
*****************************************************************************/



void	Fpu::mix_spread_1_2_v (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol_l, float vol_r)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x = in_ptr [pos];
		out_1_ptr [pos] += x * vol_l;
		out_2_ptr [pos] += x * vol_r;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_spread_1_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	const float		divisor = 1.0f / nbr_spl;
	const float		step_l = (e_vol_l - s_vol_l) * divisor;
	const float		step_r = (e_vol_r - s_vol_r) * divisor;

	int				pos = 0;
	do
	{
		const float		x = in_ptr [pos];
		out_1_ptr [pos] += x * s_vol_l;
		out_2_ptr [pos] += x * s_vol_r;

		++ pos;
		s_vol_l += step_l;
		s_vol_r += step_r;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_spread_1_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol_l, float s_vol_r, float e_vol_l, float e_vol_r)
{
	if (e_vol_l == s_vol_l && e_vol_r == s_vol_r)
	{
		mix_spread_1_2_v (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol_l, s_vol_r);
	}
	else
	{
		mix_spread_1_2_vlr (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol_l, s_vol_r, e_vol_l, e_vol_r);
	}
}



/*****************************************************************************
*
*       MATRIX MIXING
*
*****************************************************************************/



void	Fpu::mix_mat_2_2_v (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &vol)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x_1 = in_1_ptr [pos];
		const float		x_2 = in_2_ptr [pos];

		out_1_ptr [pos] +=   x_1 * vol.get_l2l ()
		                   + x_2 * vol.get_r2l ();
		out_2_ptr [pos] +=   x_1 * vol.get_l2r ()
		                   + x_2 * vol.get_r2r ();
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_mat_2_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	StereoLevel		vol = s_vol;
	StereoLevel		step = e_vol;
	step -= s_vol;
	step *= 1.0f / nbr_spl;

	int				pos = 0;
	do
	{
		const float		x_1 = in_1_ptr [pos];
		const float		x_2 = in_2_ptr [pos];

		out_1_ptr [pos] +=   x_1 * vol.get_l2l ()
		                   + x_2 * vol.get_r2l ();
		out_2_ptr [pos] +=   x_1 * vol.get_l2r ()
		                   + x_2 * vol.get_r2r ();
		++ pos;
		vol += step;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_mat_2_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, const StereoLevel &s_vol, const StereoLevel &e_vol)
{
	if (e_vol == s_vol)
	{
		mix_mat_2_2_v (out_1_ptr, out_2_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol);
	}
	else
	{
		mix_mat_2_2_vlr (out_1_ptr, out_2_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol, e_vol);
	}
}



/*****************************************************************************
*
*       MIX AND INTERLEAVE
*
*****************************************************************************/



void	Fpu::mix_1_2i (float out_ptr [], const float in_ptr [], int nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x = in_ptr [pos];
		out_ptr [pos * 2    ] += x;
		out_ptr [pos * 2 + 1] += x;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_1_2i_v (float out_ptr [], const float in_ptr [], int nbr_spl, float vol)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x = in_ptr [pos] * vol;
		out_ptr [pos * 2    ] += x;
		out_ptr [pos * 2 + 1] += x;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_1_2i_vlr (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	const float		step = (e_vol - s_vol) / nbr_spl;
	int				pos = 0;
	do
	{
		const float		x = in_ptr [pos] * s_vol;
		out_ptr [pos * 2    ] += x;
		out_ptr [pos * 2 + 1] += x;
		s_vol += step;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_1_2i_vlrauto (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	if (e_vol == s_vol)
	{
		mix_1_2i_v (out_ptr, in_ptr, nbr_spl, s_vol);
	}
	else
	{
		mix_1_2i_vlr (out_ptr, in_ptr, nbr_spl, s_vol, e_vol);
	}
}



void	Fpu::mix_2_2i (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x_1 = in_1_ptr [pos];
		const float		x_2 = in_2_ptr [pos];
		out_ptr [pos * 2    ] += x_1;
		out_ptr [pos * 2 + 1] += x_2;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_2_2i_v (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float vol)
{
	assert (out_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x_1 = in_1_ptr [pos];
		const float		x_2 = in_2_ptr [pos];
		out_ptr [pos * 2    ] += x_1 * vol;
		out_ptr [pos * 2 + 1] += x_2 * vol;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_2_2i_vlr (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	assert (out_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	const float		step = (e_vol - s_vol) / nbr_spl;
	int				pos = 0;
	do
	{
		const float		x_1 = in_1_ptr [pos];
		const float		x_2 = in_2_ptr [pos];
		out_ptr [pos * 2    ] += x_1 * s_vol;
		out_ptr [pos * 2 + 1] += x_2 * s_vol;
		s_vol += step;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_2_2i_vlrauto (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	if (e_vol == s_vol)
	{
		mix_2_2i_v (out_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol);
	}
	else
	{
		mix_2_2i_vlr (out_ptr, in_1_ptr, in_2_ptr, nbr_spl, s_vol, e_vol);
	}
}



/*****************************************************************************
*
*       MIX AND DEINTERLEAVE
*
*****************************************************************************/



void	Fpu::mix_2i_1 (float out_ptr [], const float in_ptr [], int nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		out_ptr [pos] += in_ptr [pos * 2] + in_ptr [pos * 2 + 1];
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_2i_1_v (float out_ptr [], const float in_ptr [], int nbr_spl, float vol)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		out_ptr [pos] += (in_ptr [pos * 2] + in_ptr [pos * 2 + 1]) * vol;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_2i_1_vlr (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	const float		step = (e_vol - s_vol) / nbr_spl;
	int				pos = 0;
	do
	{
		out_ptr [pos] += (in_ptr [pos * 2] + in_ptr [pos * 2 + 1]) * s_vol;
		s_vol += step;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_2i_1_vlrauto (float out_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	if (e_vol == s_vol)
	{
		mix_2i_1_v (out_ptr, in_ptr, nbr_spl, s_vol);
	}
	else
	{
		mix_2i_1_vlr (out_ptr, in_ptr, nbr_spl, s_vol, e_vol);
	}
}



void	Fpu::mix_2i_2 (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		out_1_ptr [pos] += in_ptr [pos * 2    ];
		out_2_ptr [pos] += in_ptr [pos * 2 + 1];
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_2i_2_v (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float vol)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		out_1_ptr [pos] += in_ptr [pos * 2    ] * vol;
		out_2_ptr [pos] += in_ptr [pos * 2 + 1] * vol;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_2i_2_vlr (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	const float		step = (e_vol - s_vol) / nbr_spl;
	int				pos = 0;
	do
	{
		out_1_ptr [pos] += in_ptr [pos * 2    ] * s_vol;
		out_2_ptr [pos] += in_ptr [pos * 2 + 1] * s_vol;
		s_vol += step;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mix_2i_2_vlrauto (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl, float s_vol, float e_vol)
{
	if (e_vol == s_vol)
	{
		mix_2i_2_v (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol);
	}
	else
	{
		mix_2i_2_vlr (out_1_ptr, out_2_ptr, in_ptr, nbr_spl, s_vol, e_vol);
	}
}



void	Fpu::mix_ni1_1 (float out_ptr [], const float in_ptr [], int nbr_spl, int nbr_chn_in)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);
	assert (nbr_chn_in > 0);

	int				pos = 0;
	do
	{
		out_ptr [pos] += in_ptr [pos * nbr_chn_in];
		++ pos;
	}
	while (pos < nbr_spl);
}



/*****************************************************************************
*
*       MULTIPLY
*
*****************************************************************************/



void	Fpu::mult_1_1 (float out_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x_1 = in_1_ptr [pos];
		const float		x_2 = in_2_ptr [pos];
		out_ptr [pos] = x_1 * x_2;

		++ pos;
	}
	while (pos < nbr_spl);
}



/*****************************************************************************
*
*       MULTIPLY, IN PLACE
*
*****************************************************************************/



void	Fpu::mult_ip_1_1 (float out_ptr [], const float in_ptr [], int nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x = in_ptr [pos];
		out_ptr [pos] *= x;

		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mult_ip_1_2 (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x = in_ptr [pos];
		out_1_ptr [pos] *= x;
		out_2_ptr [pos] *= x;

		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::mult_ip_2_2 (float out_1_ptr [], float out_2_ptr [], const float in_1_ptr [], const float in_2_ptr [], int nbr_spl)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (in_1_ptr != 0);
	assert (in_2_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		x_1 = in_1_ptr [pos];
		const float		x_2 = in_2_ptr [pos];
		out_1_ptr [pos] *= x_1;
		out_2_ptr [pos] *= x_2;

		++ pos;
	}
	while (pos < nbr_spl);
}



/*****************************************************************************
*
*       MISC
*
*****************************************************************************/



void	Fpu::clear (float out_ptr [], int nbr_spl)
{
	assert (out_ptr != 0);
	assert (nbr_spl > 0);

	using namespace std;

	memset (out_ptr, 0, nbr_spl * sizeof (out_ptr [0]));
}



void	Fpu::clear_nim (float out_ptr [], int nbr_spl, int clear_len, int skip_len)
{
	assert (out_ptr != 0);
	assert (nbr_spl > 0);
	assert (clear_len > 0);
	assert (skip_len >= 0);

	if (skip_len == 0)
	{
		clear (out_ptr, nbr_spl * clear_len);
	}

	else
	{
		const int		stride = clear_len + skip_len;
		const int		len = nbr_spl * stride;
		int				pos = 0;
		do
		{
			int				pos_clear = 0;
			do
			{
				out_ptr [pos + pos_clear] = 0;
				++ pos_clear;
			}
			while (pos_clear < clear_len);

			pos_clear += stride;
		}
		while (pos < len);
	}
}



void	Fpu::fill (float out_ptr [], int nbr_spl, float val)
{
	assert (out_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		out_ptr [pos] = val;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::fill_lr (float out_ptr [], int nbr_spl, float s_val, float e_val)
{
	assert (out_ptr != 0);
	assert (nbr_spl > 0);

	const float		step = (e_val - s_val) / nbr_spl;
	int				pos = 0;
	do
	{
		out_ptr [pos] = s_val;
		s_val += step;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::fill_lrauto (float out_ptr [], int nbr_spl, float s_val, float e_val)
{
	if (e_val == s_val)
	{
		fill (out_ptr, nbr_spl, s_val);
	}
	else
	{
		fill_lr (out_ptr, nbr_spl, s_val, e_val);
	}
}



void	Fpu::add_cst_1_1 (float out_ptr [], int nbr_spl, float val)
{
	assert (out_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		out_ptr [pos] += val;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::add_cst_1_2 (float out_1_ptr [], float out_2_ptr [], int nbr_spl, float val)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		out_1_ptr [pos] += val;
		out_2_ptr [pos] += val;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::linop_cst_1_1 (float out_ptr [], const float in_ptr [], int nbr_spl, float mul_val, float add_val)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		out_ptr [pos] = in_ptr [pos] * mul_val + add_val;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::linop_cst_ip_1 (float data_ptr [], int nbr_spl, float mul_val, float add_val)
{
	assert (data_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		data_ptr [pos] = data_ptr [pos] * mul_val + add_val;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::add_sub_ip_2_2 (float out_1_ptr [], float out_2_ptr [], int nbr_spl)
{
	assert (out_1_ptr != 0);
	assert (out_2_ptr != 0);
	assert (nbr_spl > 0);

	int				pos = 0;
	do
	{
		const float		a = out_1_ptr [pos];
		const float		b = out_2_ptr [pos];
		out_1_ptr [pos] = a + b;
		out_2_ptr [pos] = a - b;
		++ pos;
	}
	while (pos < nbr_spl);
}



void	Fpu::sum_square_n_1 (float out_ptr [], const float * const src_ptr_arr [], int nbr_spl, int nbr_chn, float init_val)
{
	assert (out_ptr != 0);
	assert (src_ptr_arr != 0);
	assert (src_ptr_arr [0] != 0);
	assert (nbr_spl > 0);
	assert (nbr_chn > 0);

	int           pos = 0;
	if (nbr_chn == 1)
	{
		const float *  c0_ptr = src_ptr_arr [0];
		do
		{
			const float    a = c0_ptr [pos];
			out_ptr [pos] = a * a + init_val;
			++ pos;
		}
		while (pos < nbr_spl);
	}
	else if (nbr_chn == 2)
	{
		const float *  c0_ptr = src_ptr_arr [0];
		const float *  c1_ptr = src_ptr_arr [1];
		do
		{
			const float    a = c0_ptr [pos];
			const float    b = c1_ptr [pos];
			out_ptr [pos] = a * a + b * b + init_val;
			++ pos;
		}
		while (pos < nbr_spl);
	}
	else
	{
		do
		{
			float          x   = init_val;
			int            chn = 0;
			do
			{
				const float    a = src_ptr_arr [chn] [pos];
				x += a * a;
				++ chn;
			}
			while (chn < nbr_chn);
			out_ptr [pos] = x;
			++ pos;
		}
		while (pos < nbr_spl);
	}
}



void	Fpu::sum_square_n_1_v (float out_ptr [], const float * const src_ptr_arr [], int nbr_spl, int nbr_chn, float init_val, float vol)
{
	assert (out_ptr != 0);
	assert (src_ptr_arr != 0);
	assert (nbr_spl > 0);
	assert (nbr_chn > 0);

	int           pos = 0;
	if (nbr_chn == 1)
	{
		const float *  c0_ptr = src_ptr_arr [0];
		do
		{
			const float    a = c0_ptr [pos];
			out_ptr [pos] = (a * a + init_val) * vol;
			++ pos;
		}
		while (pos < nbr_spl);
	}
	else if (nbr_chn == 2)
	{
		const float *  c0_ptr = src_ptr_arr [0];
		const float *  c1_ptr = src_ptr_arr [1];
		do
		{
			const float    a = c0_ptr [pos];
			const float    b = c1_ptr [pos];
			out_ptr [pos] = (a * a + b * b + init_val) * vol;
			++ pos;
		}
		while (pos < nbr_spl);
	}
	else
	{
		do
		{
			float          x   = init_val;
			int            chn = 0;
			do
			{
				const float    a = src_ptr_arr [chn] [pos];
				x += a * a;
				++ chn;
			}
			while (chn < nbr_chn);
			out_ptr [pos] = x * vol;
			++ pos;
		}
		while (pos < nbr_spl);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mix
}  // namespace dsp
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
