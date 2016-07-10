/*****************************************************************************

        InterpolatorFir.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (mfx_dsp_rspl_InterpolatorFir_CURRENT_CODEHEADER)
	#error Recursive inclusion of InterpolatorFir code header.
#endif
#define	mfx_dsp_rspl_InterpolatorFir_CURRENT_CODEHEADER

#if ! defined (mfx_dsp_rspl_InterpolatorFir_CODEHEADER_INCLUDED)
#define	mfx_dsp_rspl_InterpolatorFir_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"mfx/dsp/rspl/SnhTool.h"

#include <algorithm>

#include <cstdint>



namespace mfx
{
namespace dsp
{
namespace rspl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class IT>
InterpolatorFir <IT>::InterpolatorFir ()
:	_snh_tool ()
,	_grp_dly (0)
,	_conv_ptr (0)
,	_nbr_chn (1)
,	_ovrspl_l2 (0)
{
	// Nothing
}



template <class IT>
void	InterpolatorFir <IT>::set_convolver (IT &convolver)
{
	assert (&convolver != 0);

	_conv_ptr = &convolver;
}



template <class IT>
void	InterpolatorFir <IT>::set_group_delay (double grp_dly)
{
	assert (grp_dly >= 0);
	assert (grp_dly <= PHASE_LEN - 1);

	_grp_dly.set_val (grp_dly);
}



template <class IT>
bool	InterpolatorFir <IT>::is_ready () const
{
	return (_conv_ptr != 0);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class IT>
void	InterpolatorFir <IT>::do_set_ovrspl_l2 (int ovrspl_l2)
{
	_ovrspl_l2 = ovrspl_l2;
	_snh_tool.set_ovrspl (_ovrspl_l2);
}



template <class IT>
long	InterpolatorFir <IT>::do_get_impulse_len () const
{
	return (PHASE_LEN);
}



template <class IT>
fstb::FixedPoint	InterpolatorFir <IT>::do_get_group_delay () const
{
	return (_grp_dly);
}



template <class IT>
void	InterpolatorFir <IT>::do_start (int nbr_chn)
{
	_nbr_chn = nbr_chn;
	_snh_tool.set_nbr_chn (_nbr_chn);
}



template <class IT>
long	InterpolatorFir <IT>::do_process_block (float * const dest_ptr_arr [], const float * const src_ptr_arr [], long pos_dest, fstb::FixedPoint pos_src, long end_dest, long beg_src, long end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step)
{
	assert (is_ready ());

	int				hold_time;
	int				rep_index;
	_snh_tool.compute_snh_data (
		hold_time,
		rep_index,
		end_dest - pos_dest,
		rate,
		rate_step
	);

	long				nbr_spl;

	// Compact
	if (hold_time == 1)
	{
		if (_nbr_chn == 1)
		{
			nbr_spl = process_block_mono (
				dest_ptr_arr [0],
				src_ptr_arr [0],
				pos_dest,
				pos_src,
				end_dest,
				beg_src,
				end_src,
				rate,
				rate_step
			);
		}

		else
		{
			nbr_spl = process_block_multi_chn (
				dest_ptr_arr,
				src_ptr_arr,
				pos_dest,
				pos_src,
				end_dest,
				beg_src,
				end_src,
				rate,
				rate_step
			);
		}
	}

	// Sparse
	else
	{
		if (_nbr_chn == 1)
		{
			nbr_spl = process_block_mono_sparse (
				dest_ptr_arr [0],
				src_ptr_arr [0],
				pos_dest,
				pos_src,
				end_dest,
				beg_src,
				end_src,
				rate,
				rate_step,
				hold_time,
				rep_index
			);
		}

		else
		{
			nbr_spl = process_block_multi_chn_sparse (
				dest_ptr_arr,
				src_ptr_arr,
				pos_dest,
				pos_src,
				end_dest,
				beg_src,
				end_src,
				rate,
				rate_step,
				hold_time,
				rep_index
			);
		}
	}

	_snh_tool.process_data (dest_ptr_arr, nbr_spl, rate, rate_step);

	return (nbr_spl);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class IT>
long	InterpolatorFir <IT>::process_block_multi_chn (float * const dest_ptr_arr [], const float * const src_ptr_arr [], long pos_dest, fstb::FixedPoint pos_src, long end_dest, long beg_src, long end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step)
{
	const long		pos_dest_old = pos_dest;
	const long		src_limit = end_src - PHASE_LEN + 1;

#if 0

	long				pos_src_int = pos_src.get_int_val ();
	do
	{
		const uint32_t pos_src_frac = pos_src.get_frac_val ();

		_conv_ptr->interpolate_multi_chn (
			dest_ptr_arr,
			pos_dest,
			src_ptr_arr,
			pos_src_int,
			pos_src_frac,
			_nbr_chn
		);

		pos_src += rate;
		rate += rate_step;
		++ pos_dest;

		pos_src_int = pos_src.get_int_val ();
	}
	while (   pos_dest < end_dest
	       && pos_src_int < src_limit
	       && pos_src_int >= beg_src);

#else

	const fstb::FixedPoint	save_pos_src (pos_src);
	const fstb::FixedPoint	save_rate (rate);

	for (int chn_cnt = 0; chn_cnt < _nbr_chn; ++chn_cnt)
	{
		pos_dest = pos_dest_old;
		pos_src = save_pos_src;
		rate = save_rate;
		long				pos_src_int = pos_src.get_int_val ();
		const float *	src_ptr = src_ptr_arr [chn_cnt];
		float *			dest_ptr = dest_ptr_arr [chn_cnt];
		do
		{
			const uint32_t pos_src_frac = pos_src.get_frac_val ();

			const float    val =
				_conv_ptr->interpolate (&src_ptr [pos_src_int], pos_src_frac);
			dest_ptr [pos_dest] = val;

			pos_src += rate;
			rate += rate_step;
			++ pos_dest;

			pos_src_int = pos_src.get_int_val ();
		}
		while (   pos_dest < end_dest
		       && pos_src_int < src_limit
		       && pos_src_int >= beg_src);
	}

#endif

	return (pos_dest - pos_dest_old);
}



template <class IT>
long	InterpolatorFir <IT>::process_block_mono (float dest_ptr [], const float src_ptr [], long pos_dest, fstb::FixedPoint pos_src, long end_dest, long beg_src, long end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step)
{
	const long		pos_dest_old = pos_dest;
	const long		src_limit = end_src - PHASE_LEN + 1;

	long				pos_src_int = pos_src.get_int_val ();
	do
	{
		const uint32_t pos_src_frac = pos_src.get_frac_val ();

		const float    val =
			_conv_ptr->interpolate (&src_ptr [pos_src_int], pos_src_frac);
		dest_ptr [pos_dest] = val;

		pos_src += rate;
		rate += rate_step;
		++ pos_dest;

		pos_src_int = pos_src.get_int_val ();
	}
	while (   pos_dest < end_dest
	       && pos_src_int < src_limit
	       && pos_src_int >= beg_src);

	return (pos_dest - pos_dest_old);
}



template <class IT>
long	InterpolatorFir <IT>::process_block_multi_chn_sparse (float * const dest_ptr_arr [], const float * const src_ptr_arr [], long pos_dest, fstb::FixedPoint pos_src, long end_dest, long beg_src, long end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step, int hold_time, int rep_index)
{
	const long		pos_dest_old = pos_dest;

	_snh_tool.adjust_rate_param (
		pos_dest,
		pos_src,
		rate,
		rate_step,
		hold_time,
		rep_index
	);

	const long		src_limit = end_src - PHASE_LEN + 1;

#if 0

	long				pos_src_int = pos_src.get_int_val ();
	while (   pos_dest < end_dest
	       && pos_src_int < src_limit
	       && pos_src_int >= beg_src)
	{
		const uint32_t pos_src_frac = pos_src.get_frac_val ();

		_conv_ptr->interpolate_multi_chn (
			dest_ptr_arr,
			pos_dest,
			src_ptr_arr,
			pos_src_int,
			pos_src_frac,
			_nbr_chn
		);

		pos_src += rate;
		rate += rate_step;
		pos_dest += hold_time;

		pos_src_int = pos_src.get_int_val ();
	}

#else

	const fstb::FixedPoint	save_pos_src (pos_src);
	const fstb::FixedPoint	save_rate (rate);

	for (int chn_cnt = 0; chn_cnt < _nbr_chn; ++chn_cnt)
	{
		pos_dest = pos_dest_old;
		pos_src = save_pos_src;
		rate = save_rate;
		const float *  src_ptr     = src_ptr_arr [chn_cnt];
		float *        dest_ptr    = dest_ptr_arr [chn_cnt];

		long           pos_src_int = pos_src.get_int_val ();
		while (   pos_dest < end_dest
		       && pos_src_int < src_limit
		       && pos_src_int >= beg_src)
		{
			const uint32_t pos_src_frac = pos_src.get_frac_val ();

			const float		val =
				_conv_ptr->interpolate (&src_ptr [pos_src_int], pos_src_frac);
			dest_ptr [pos_dest] = val;

			pos_src += rate;
			rate += rate_step;
			pos_dest += hold_time;

			pos_src_int = pos_src.get_int_val ();
		}
	}

#endif

	return (std::min (pos_dest, end_dest) - pos_dest_old);
}



template <class IT>
long	InterpolatorFir <IT>::process_block_mono_sparse (float dest_ptr [], const float src_ptr [], long pos_dest, fstb::FixedPoint pos_src, long end_dest, long beg_src, long end_src, fstb::FixedPoint rate, fstb::FixedPoint rate_step, int hold_time, int rep_index)
{
	const long		pos_dest_old = pos_dest;

	_snh_tool.adjust_rate_param (
		pos_dest,
		pos_src,
		rate,
		rate_step,
		hold_time,
		rep_index
	);

	const long		src_limit = end_src - PHASE_LEN + 1;

	long				pos_src_int = pos_src.get_int_val ();
	while (   pos_dest < end_dest
	       && pos_src_int < src_limit
	       && pos_src_int >= beg_src)
	{
		const uint32_t pos_src_frac = pos_src.get_frac_val ();

		const float		val =
			_conv_ptr->interpolate (&src_ptr [pos_src_int], pos_src_frac);
		dest_ptr [pos_dest] = val;

		pos_src += rate;
		rate += rate_step;
		pos_dest += hold_time;

		pos_src_int = pos_src.get_int_val ();
	}

	return (std::min (pos_dest, end_dest) - pos_dest_old);
}



}	// namespace rspl
}	// namespace dsp
}	// namespace mfx



#endif	// mfx_dsp_rspl_InterpolatorFir_CODEHEADER_INCLUDED

#undef mfx_dsp_rspl_InterpolatorFir_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
