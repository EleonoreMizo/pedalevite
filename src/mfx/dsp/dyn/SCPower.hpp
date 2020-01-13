/*****************************************************************************

        SCPower.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_dyn_SCPower_CODEHEADER_INCLUDED)
#define mfx_dsp_dyn_SCPower_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/DataAlign.h"
#include "fstb/ToolsSimd.h"



namespace mfx
{
namespace dsp
{
namespace dyn
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class AP>
typename SCPower <AP>::AddProc &	SCPower <AP>::use_add_proc ()
{
	return _add_proc;
}



// Sums the square of all channels into the destination buffer.
// Input channels may not be aligned on 16-byte boundaries, but destination is.
// The pos_beg position in the input buffer corresponds to the 0 position in
// the output buffer.
template <class AP>
void	SCPower <AP>::prepare_env_input (float out_ptr [], const float * const chn_ptr_arr [], int nbr_chn, int pos_beg, int pos_end)
{
	assert (fstb::DataAlign <true>::check_ptr (out_ptr));
	assert (chn_ptr_arr != nullptr);
	assert (pos_beg >= 0);
	assert (pos_beg < pos_end);
	assert (chn_ptr_arr [0] != nullptr);
	assert (chn_ptr_arr [nbr_chn - 1] != nullptr);

	if (nbr_chn == 1)
	{
		prepare_env_input_1chn (out_ptr, chn_ptr_arr, pos_beg, pos_end);
	}
	else if (nbr_chn == 2)
	{
		prepare_env_input_2chn (out_ptr, chn_ptr_arr, pos_beg, pos_end);
	}
	else
	{
		prepare_env_input_nchn (out_ptr, chn_ptr_arr, nbr_chn, pos_beg, pos_end);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class AP>
void	SCPower <AP>::prepare_env_input_1chn (float out_ptr [], const float * const chn_ptr_arr [], int pos_beg, int pos_end)
{
	static const float   not_zero_flt = 1e-30f;  // -600 dB

	float *        dest_ptr = out_ptr - pos_beg;
	const auto     not_zero = fstb::ToolsSimd::set1_f32 (not_zero_flt);

	// Processes blocks
	const int      nbr_spl       = pos_end - pos_beg;
	const int      pos_end_block = pos_beg + (nbr_spl & ~15);
	int            pos           = pos_beg;
	while (pos < pos_end_block)
	{
		auto           s_0 = not_zero;
		auto           s_1 = not_zero;
		auto           s_2 = not_zero;
		auto           s_3 = not_zero;

		const float *  src_ptr = chn_ptr_arr [0] + pos;

		auto           x_0 = fstb::ToolsSimd::loadu_f32 (src_ptr     );
		auto           x_1 = fstb::ToolsSimd::loadu_f32 (src_ptr +  4);
		auto           x_2 = fstb::ToolsSimd::loadu_f32 (src_ptr +  8);
		auto           x_3 = fstb::ToolsSimd::loadu_f32 (src_ptr + 12);
		fstb::ToolsSimd::mac (s_0, x_0, x_0);
		fstb::ToolsSimd::mac (s_1, x_1, x_1);
		fstb::ToolsSimd::mac (s_2, x_2, x_2);
		fstb::ToolsSimd::mac (s_3, x_3, x_3);

		float *        b_ptr = dest_ptr + pos;

		s_0 = _add_proc.process_vect (s_0);
		s_1 = _add_proc.process_vect (s_1);
		s_2 = _add_proc.process_vect (s_2);
		s_3 = _add_proc.process_vect (s_3);

		fstb::ToolsSimd::store_f32 (b_ptr     , s_0);
		fstb::ToolsSimd::store_f32 (b_ptr +  4, s_1);
		fstb::ToolsSimd::store_f32 (b_ptr +  8, s_2);
		fstb::ToolsSimd::store_f32 (b_ptr + 12, s_3);

		pos += 16;
	}

	// Processes remaining samples
	while (pos < pos_end)
	{
		const float    cur = chn_ptr_arr [0] [pos];
		float          sum = not_zero_flt + cur * cur;
		sum = _add_proc.process_scalar (sum);
		dest_ptr [pos] = sum;
		++ pos;
	}
}



template <class AP>
void	SCPower <AP>::prepare_env_input_2chn (float out_ptr [], const float * const chn_ptr_arr [], int pos_beg, int pos_end)
{
	static const float   not_zero_flt = 1e-30f;  // -600 dB

	float *        dest_ptr = out_ptr - pos_beg;
	const auto     not_zero = fstb::ToolsSimd::set1_f32 (not_zero_flt);

	// Processes blocks
	const int      nbr_spl       = pos_end - pos_beg;
	const int      pos_end_block = pos_beg + (nbr_spl & ~15);
	int            pos           = pos_beg;
	while (pos < pos_end_block)
	{
		auto           s_0 = not_zero;
		auto           s_1 = not_zero;
		auto           s_2 = not_zero;
		auto           s_3 = not_zero;

		const float *  src0_ptr = chn_ptr_arr [0] + pos;
		const float *  src1_ptr = chn_ptr_arr [1] + pos;

		auto           x_0 = fstb::ToolsSimd::loadu_f32 (src0_ptr     );
		auto           x_1 = fstb::ToolsSimd::loadu_f32 (src0_ptr +  4);
		auto           x_2 = fstb::ToolsSimd::loadu_f32 (src0_ptr +  8);
		auto           x_3 = fstb::ToolsSimd::loadu_f32 (src0_ptr + 12);
		fstb::ToolsSimd::mac (s_0, x_0, x_0);
		fstb::ToolsSimd::mac (s_1, x_1, x_1);
		fstb::ToolsSimd::mac (s_2, x_2, x_2);
		fstb::ToolsSimd::mac (s_3, x_3, x_3);
		x_0 = fstb::ToolsSimd::loadu_f32 (src1_ptr     );
		x_1 = fstb::ToolsSimd::loadu_f32 (src1_ptr +  4);
		x_2 = fstb::ToolsSimd::loadu_f32 (src1_ptr +  8);
		x_3 = fstb::ToolsSimd::loadu_f32 (src1_ptr + 12);
		fstb::ToolsSimd::mac (s_0, x_0, x_0);
		fstb::ToolsSimd::mac (s_1, x_1, x_1);
		fstb::ToolsSimd::mac (s_2, x_2, x_2);
		fstb::ToolsSimd::mac (s_3, x_3, x_3);

		float *        b_ptr = dest_ptr + pos;

		s_0 = _add_proc.process_vect (s_0);
		s_1 = _add_proc.process_vect (s_1);
		s_2 = _add_proc.process_vect (s_2);
		s_3 = _add_proc.process_vect (s_3);

		fstb::ToolsSimd::store_f32 (b_ptr     , s_0);
		fstb::ToolsSimd::store_f32 (b_ptr +  4, s_1);
		fstb::ToolsSimd::store_f32 (b_ptr +  8, s_2);
		fstb::ToolsSimd::store_f32 (b_ptr + 12, s_3);

		pos += 16;
	}

	// Processes remaining samples
	const float *  src0_ptr = chn_ptr_arr [0];
	const float *  src1_ptr = chn_ptr_arr [1];
	while (pos < pos_end)
	{
		const float    cur0 = src0_ptr [pos];
		const float    cur1 = src1_ptr [pos];
		float          sum  = not_zero_flt + cur0 * cur0 + cur1 * cur1;
		sum = _add_proc.process_scalar (sum);
		dest_ptr [pos] = sum;
		++ pos;
	}
}



template <class AP>
void	SCPower <AP>::prepare_env_input_nchn (float out_ptr [], const float * const chn_ptr_arr [], int nbr_chn, int pos_beg, int pos_end)
{
	static const float   not_zero_flt = 1e-30f;  // -600 dB

	float *        dest_ptr = out_ptr - pos_beg;
	const auto     not_zero = fstb::ToolsSimd::set1_f32 (not_zero_flt);

	// Processes blocks
	const int      nbr_spl       = pos_end - pos_beg;
	const int      pos_end_block = pos_beg + (nbr_spl & ~15);
	int            pos           = pos_beg;
	while (pos < pos_end_block)
	{
		auto           s_0 = not_zero;
		auto           s_1 = not_zero;
		auto           s_2 = not_zero;
		auto           s_3 = not_zero;

		int            chn_index = 0;
		do
		{
			const float *  src_ptr = chn_ptr_arr [chn_index] + pos;

			auto           x_0 = fstb::ToolsSimd::loadu_f32 (src_ptr     );
			auto           x_1 = fstb::ToolsSimd::loadu_f32 (src_ptr +  4);
			auto           x_2 = fstb::ToolsSimd::loadu_f32 (src_ptr +  8);
			auto           x_3 = fstb::ToolsSimd::loadu_f32 (src_ptr + 12);
			fstb::ToolsSimd::mac (s_0, x_0, x_0);
			fstb::ToolsSimd::mac (s_1, x_1, x_1);
			fstb::ToolsSimd::mac (s_2, x_2, x_2);
			fstb::ToolsSimd::mac (s_3, x_3, x_3);

			++ chn_index;
		}
		while (chn_index < nbr_chn);

		float *        b_ptr = dest_ptr + pos;

		s_0 = _add_proc.process_vect (s_0);
		s_1 = _add_proc.process_vect (s_1);
		s_2 = _add_proc.process_vect (s_2);
		s_3 = _add_proc.process_vect (s_3);

		fstb::ToolsSimd::store_f32 (b_ptr     , s_0);
		fstb::ToolsSimd::store_f32 (b_ptr +  4, s_1);
		fstb::ToolsSimd::store_f32 (b_ptr +  8, s_2);
		fstb::ToolsSimd::store_f32 (b_ptr + 12, s_3);

		pos += 16;
	}

	// Processes remaining samples
	while (pos < pos_end)
	{
		float          sum       = not_zero_flt;

		int            chn_index = 0;
		do
		{
			const float    cur = chn_ptr_arr [chn_index] [pos];
			sum += cur * cur;
			++ chn_index;
		}
		while (chn_index < nbr_chn);

		sum = _add_proc.process_scalar (sum);

		dest_ptr [pos] = sum;

		++ pos;
	}
}



}  // namespace dyn
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_dyn_SCPower_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
