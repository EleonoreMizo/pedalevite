/*****************************************************************************

        BiquadPackSimd.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_BiquadPackSimd_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_BiquadPackSimd_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/mix/Generic.h"
#include "mfx/dsp/mix/Simd.h"

#include <algorithm>

#include <cassert>
#include <cstring>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: ctor
Throws: Nothing
==============================================================================
*/

template <class VD, class VS>
BiquadPackSimd <VD, VS>::BiquadPackSimd ()
:	_pack_list ()
,	_tmp_buf ()
,	_buf_len (0)
,	_nbr_stages (0)
,	_nbr_chn (0)
,	_group_size (1)
,	_nbr_packs (0)
,	_ramp_len (64)
,	_proc_conf (ProcConf_PARALLEL)
,	_biq_info_list ()
{
	dsp::mix::Generic::setup ();

	set_config (0, 0);
	set_buf_len (256);
}



/*
==============================================================================
Name: set_config
Description:
	Changes the pack configuration. All biquad equations are neutralised and
	must be set again. Buffers are cleared.
Input parameters:
	- nbr_stages: Number of biquad stages per channel, >= 0
	- nbr_chn: Number of channels. >= 0
Throws: memory allocation exceptions.
==============================================================================
*/

template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::set_config (int nbr_stages, int nbr_chn)
{
	assert (nbr_stages >= 0);
	assert (nbr_chn >= 0);

	set_config_internal (nbr_stages, nbr_chn);
}



/*
==============================================================================
Name: reserve
Description:
	Reserve memory in order to be able to change the configuration during the
	processing without requiering any memory management (with adapt_config()).
	Over the specified number of stages or number of channels, memory
	allocation may be needed during.
Input parameters:
	- nbr_stages: Maximum number of stages per channel, >= 0
	- nbr_chn: Maximum number of channels, >= 0
Throws: memory allocation exceptions.
==============================================================================
*/

template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::reserve (int nbr_stages, int nbr_chn)
{
	assert (nbr_stages >= 0);
	assert (nbr_chn >= 0);

	const int      nbr_biq = nbr_stages * nbr_chn;
	if (nbr_biq > int (_biq_info_list.size ()))
	{
		_biq_info_list.resize (nbr_biq);
	}

	const int      bound_1 = ((nbr_stages + 3) >> 2) * nbr_chn;
	const int      bound_2 = nbr_stages * ((nbr_chn + 3) >> 2);
	const int      max_nbr_pack = std::max (bound_1, bound_2);
	const int      max_nbr_pack_old = int (_pack_list.size ());
	if (max_nbr_pack > max_nbr_pack_old)
	{
		_pack_list.resize (max_nbr_pack);
		for (int pos = max_nbr_pack_old; pos < max_nbr_pack; ++pos)
		{
			Pack4 &     pack = _pack_list [pos];
			pack.neutralise ();
			pack.set_ramp_time (_ramp_len);
			pack.clear_buffers ();
		}
	}
}



/*
==============================================================================
Name: set_buf_len
Description:
	Sets the internal buffer length. If one of the operand (source or
	destination) are aligned, the buffer length must be of the same
	alignment.
Input parameters:
	- nbr_spl: Internal buffer length, in sample frame (independant on the
		number of channels). > 0
Throws: memory allocation exceptions.
==============================================================================
*/

template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::set_buf_len (long nbr_spl)
{
	assert (nbr_spl > 0);

	_buf_len = nbr_spl;
	_tmp_buf.resize (_buf_len * 4);
}



/*
==============================================================================
Name: set_ramp_time
Description:
	Sets the ramping time for a biquad equation change. The function can be
	called even if some biquads are currently ramping.
Input parameters:
	- nbr_spl: Duration of the ramp, > 0.
Throws: Nothing.
==============================================================================
*/

template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::set_ramp_time (long nbr_spl)
{
	assert (nbr_spl > 0);

	if (_ramp_len != nbr_spl)
	{
		_ramp_len = nbr_spl;
		for (int p_cnt = 0; p_cnt < _nbr_packs; ++p_cnt)
		{
			Pack4 &        pack = _pack_list [p_cnt];
			pack.set_ramp_time (_ramp_len);
		}
	}
}



/*
==============================================================================
Name: adapt_config
Description:
	Changes the pack configuration in a friendly way regarding the processing.
	However, if some filters were ramping, ramps are stopped and the final
	states are immediately reached.
Input parameters:
	- nbr_stages: New number of biquad stages per channel, >= 0
	- nbr_chn: New number of channels, >= 0
Throws:
	Nothing if we stay in the reserve() bounds, memory related exceptions
	otherwise.
==============================================================================
*/

template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::adapt_config (int nbr_stages, int nbr_chn)
{
	assert (nbr_stages >= 0);
	assert (nbr_chn >= 0);

	const int      nbr_stages_old = _nbr_stages;
	const int      nbr_chn_old    = _nbr_chn;

	if (   nbr_stages != nbr_stages_old
	    || nbr_chn    != nbr_chn_old)
	{
		save_info ();
		set_config_internal (nbr_stages, nbr_chn);
		load_info (nbr_stages_old, nbr_chn_old);
	}
}



/*
==============================================================================
Name: set_biquad
Description:
	Change a biquad equation, in Z-domain.
	It is possible to smooth the change with ramping. However, if a change is
	already initated in the group which this biquad belongs to, the new value
	is stored for the subsequent change, beginning after the current change
	completion.
Input parameters:
	- stage: Stage index, [0 ; max number of stages[.
	- chn: Channel index, [0 ; max number of channels[.
	- b: Num. of the biquad in the Z-plane, index is the power of z.
	- a: Denom. of the biquad in the Z-plane.
	- ramp_flag: indicates that the change is not immediate. The biquad will
		morph from its current state to the new state in the standard ramp time.
Throws: Nothing
==============================================================================
*/

template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::set_biquad (int stage, int chn, const float b [3], const float a [3], bool ramp_flag)
{
	assert (stage >= 0);
	assert (stage < _nbr_stages);
	assert (chn >= 0);
	assert (chn < _nbr_chn);
	assert (b != 0);
	assert (a != 0);

	int            pack_index;
	int            biq_index;
	find_biq (pack_index, biq_index, stage, chn);

	Pack4 &        pack = _pack_list [pack_index];
	pack.set_z_eq_one (biq_index, b, a, ramp_flag);
}



/*
==============================================================================
Name: get_biquad
Description:
	Retrieve a biquad equation in the Z domain. The equation is the one
	currently in use (coefficients may ramp).
Input parameters:
	- stage: Stage index, [0 ; max number of stages[.
	- chn: Channel index, [0 ; max number of channels[.
	- b: Num. of the biquad in the Z-plane, index is the power of z.
	- a: Denom. of the biquad in the Z-plane.
Throws: Nothing
==============================================================================
*/

template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::get_biquad (int stage, int chn, float b [3], float a [3]) const
{
	assert (stage >= 0);
	assert (stage < _nbr_stages);
	assert (chn >= 0);
	assert (chn < _nbr_chn);
	assert (b != 0);
	assert (a != 0);

	int            pack_index;
	int            biq_index;
	find_biq (pack_index, biq_index, stage, chn);

	const Pack4 &  pack = _pack_list [pack_index];
	pack.get_z_eq_one (biq_index, b, a);
}



/*
==============================================================================
Name: get_biquad_target
Description:
	Retrieve a biquad equation in the Z domain. The equation is the target,
	if a ramp is active.
	However it does not return a subsequent programmed equation.
Input parameters:
	- stage: Stage index, [0 ; max number of stages[.
	- chn: Channel index, [0 ; max number of channels[.
	- b: Num. of the biquad in the Z-plane, index is the power of z.
	- a: Denom. of the biquad in the Z-plane.
Throws: Nothing
==============================================================================
*/

template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::get_biquad_target (int stage, int chn, float b [3], float a [3]) const
{
	assert (stage >= 0);
	assert (stage < _nbr_stages);
	assert (chn >= 0);
	assert (chn < _nbr_chn);
	assert (b != 0);
	assert (a != 0);

	int            pack_index;
	int            biq_index;
	find_biq (pack_index, biq_index, stage, chn);

	const Pack4 &  pack = _pack_list [pack_index];
	if (pack.is_ramping ())
	{
		pack.get_z_eq_one_ramp (biq_index, b, a);
	}
	else
	{
		pack.get_z_eq_one (biq_index, b, a);
	}
}



/*
==============================================================================
Name: process_block
Description:
	Filters a block of data. Can work in-place, if the input and output
	channels are in the same order.
Input parameters:
	- in_ptr_arr: List of pointers on each channel input data.
	- pos_beg: Offset of the first sample frame to process (independant from
		the number of channels). Must be aligned if one of the operand (source
		or destination) is aligned.
	- pos_end: Offset of the frame after the last sample frame to process
		(so pos_end - pos_beg is the number of processed frames). Must be
		greater than pos_beg. Must be aligned if one of the operand (source
		or destination) is aligned.
Output parameters:
	- out_ptr_arr: List of pointers on each channel output data.
Throws: Nothing
==============================================================================
*/

template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::process_block (float * const out_ptr_arr [], const float * const in_ptr_arr [], long pos_beg, long pos_end)
{
	assert (out_ptr_arr != 0);
	assert (in_ptr_arr != 0);
	assert (pos_end - pos_beg > 0);

	if (_nbr_stages == 0)
	{
		const long     nbr_spl = pos_end - pos_beg;
		for (int chn = 0; chn < _nbr_chn; ++chn)
		{
			const float *  in_ptr = in_ptr_arr [chn] + pos_beg;
			float	*        out_ptr = out_ptr_arr [chn] + pos_beg;
			if (in_ptr != out_ptr)
			{
				memmove (out_ptr, in_ptr, sizeof (*out_ptr) * nbr_spl);
			}
		}
	}

	else
	{
		switch (_proc_conf)
		{
		case	ProcConf_PARALLEL:
			process_block_parallel (out_ptr_arr, in_ptr_arr, pos_beg, pos_end, false);
			break;

		case	ProcConf_SERIAL:
			process_block_serial (out_ptr_arr, in_ptr_arr, pos_beg, pos_end);
			break;

		case	ProcConf_2X2:
			process_block_2x2 (out_ptr_arr, in_ptr_arr, pos_beg, pos_end);
			break;

		default:
			assert (false);
			break;
		}
	}
}



/*
==============================================================================
Name: process_block
Description:
	Filters a block of data, using the same data channel as input for all
	the filter channels.
Input parameters:
	- in_ptr: Pointer on the input data. There is no alignment requirement.
	- pos_beg: Offset of the first sample frame to process (independant from
		the number of channels). Must be aligned if the destination is aligned.
	- pos_end: Offset of the frame after the last sample frame to process
		(so pos_end - pos_beg is the number of processed frames). Must be
		greater than pos_beg. Must be aligned if one of the destination is
		aligned.
Output parameters:
	- out_ptr_arr: List of pointers on each channel output data.
Throws: Nothing
==============================================================================
*/

template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::process_block (float * const out_ptr_arr [], const float in_ptr [], long pos_beg, long pos_end)
{
	assert (out_ptr_arr != 0);
	assert (in_ptr != 0);
	assert (pos_end - pos_beg > 0);

	if (_nbr_stages == 0)
	{
		const float *  src_ptr = in_ptr + pos_beg;
		const long     nbr_spl = pos_end - pos_beg;
		for (int chn = 0; chn < _nbr_chn; ++chn)
		{
			float	*        out_ptr = out_ptr_arr [chn] + pos_beg;
			if (src_ptr != out_ptr)
			{
				memmove (out_ptr, src_ptr, sizeof (*out_ptr) * nbr_spl);
			}
		}
	}

	else
	{
		switch (_proc_conf)
		{
		case	ProcConf_PARALLEL:
			process_block_parallel (out_ptr_arr, &in_ptr, pos_beg, pos_end, true);
			break;

		case	ProcConf_SERIAL:
			process_block_serial (out_ptr_arr, in_ptr, pos_beg, pos_end);
			break;

		case	ProcConf_2X2:
			process_block_2x2 (out_ptr_arr, in_ptr, pos_beg, pos_end);
			break;

		default:
			assert (false);
			break;
		}
	}
}



/*
==============================================================================
Name: clear_buffers
Description:
	Stops any ramp to reach immediately the final stages. Biquad buffers are
	cleared too.
Throws: Nothing
==============================================================================
*/

template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::clear_buffers ()
{
	for (int pack_index = 0; pack_index < _nbr_packs; ++pack_index)
	{
		Pack4 &        pack = _pack_list [pack_index];
		pack.clear_buffers ();
	}
}



/*
==============================================================================
Name: clear_buffers_one
Description: Clears the buffer of only one biquad.
Input parameters:
	- stage: Stage index
	- chn: Biquad index
Throws: Nothing
==============================================================================
*/

template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::clear_buffers_one (int stage, int chn)
{
	assert (stage >= 0);
	assert (stage < _nbr_stages);
	assert (chn >= 0);
	assert (chn < _nbr_chn);

	int            pack_index;
	int            biq_index;
	find_biq (pack_index, biq_index, stage, chn);

	Pack4 &        pack = _pack_list [pack_index];
	pack.clear_buffers_one (biq_index);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::find_biq (int &pack_index, int &biq_index, int stage, int chn) const
{
	assert (&pack_index != 0);
	assert (&biq_index != 0);
	assert (stage >= 0);
	assert (stage < _nbr_stages);
	assert (chn >= 0);
	assert (chn < _nbr_chn);

	switch (_proc_conf)
	{
	case	ProcConf_PARALLEL:
		pack_index = (chn >> 2) * _group_size + stage;
		biq_index  = chn & 3;
		break;

	case	ProcConf_SERIAL:
		pack_index = chn * _group_size + (stage >> 2);
		biq_index  = stage & 3;
		break;

	case	ProcConf_2X2:
		pack_index = 0;
		biq_index  = stage * 2 + chn;
		break;

	default:
		pack_index = 0;
		biq_index  = 0;
		assert (false);
		break;
	}
}



template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::set_config_internal (int nbr_stages, int nbr_chn)
{
	assert (nbr_stages >= 0);
	assert (nbr_chn >= 0);

	reserve (nbr_stages, nbr_chn);

	compute_config_info (
		_proc_conf,
		_nbr_packs,
		_group_size,
		nbr_stages,
		nbr_chn
	);

	_nbr_stages = nbr_stages;
	_nbr_chn    = nbr_chn;

	for (int pack_index = 0; pack_index < _nbr_packs; ++pack_index)
	{
		Pack4 &        pack = _pack_list [pack_index];
		pack.neutralise ();
	}

	clear_buffers ();
}



template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::save_info ()
{
	assert (int (_biq_info_list.size ()) >= _nbr_stages * _nbr_chn);

	for (int stage = 0; stage < _nbr_stages; ++stage)
	{
		for (int chn = 0; chn < _nbr_chn; ++chn)
		{
			const int      index = stage * _nbr_chn + chn;
			BiqInfo &      info = _biq_info_list [index];

			int            pack_index;
			int            biq_index;
			find_biq (pack_index, biq_index, stage, chn);

			const Pack4 &	pack = _pack_list [pack_index];
			pack.get_state_one (biq_index, info._mem_x, info._mem_y);
			pack.get_z_eq_one_final (biq_index, info._b, info._a);
		}
	}
}



template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::load_info (int nbr_stages, int nbr_chn)
{
	assert (int (_biq_info_list.size ()) >= nbr_stages * nbr_chn);
	assert (nbr_stages >= 0);
	assert (nbr_chn >= 0);

	const int      stage_end = std::min (nbr_stages, _nbr_stages);
	const int      chn_end   = std::min (nbr_chn, _nbr_chn);
	for (int stage = 0; stage < stage_end; ++stage)
	{
		for (int chn = 0; chn < chn_end; ++chn)
		{
			const int      index = stage * nbr_chn + chn;
			const BiqInfo& info = _biq_info_list [index];

			int            pack_index;
			int            biq_index;
			find_biq (pack_index, biq_index, stage, chn);

			Pack4 &        pack = _pack_list [pack_index];
			assert (! pack.is_ramping ());
			pack.set_z_eq_one (biq_index, info._b, info._a);
			pack.set_state_one (biq_index, info._mem_x, info._mem_y);
		}
	}
}



template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::process_block_parallel (float * const out_ptr_arr [], const float * const in_ptr_arr [], long pos_beg, long pos_end, bool mono_source_flag)
{
	int            pack_index = 0;
	for (int chn_base = 0; chn_base < _nbr_chn; chn_base += 4)
	{
		const int      pack_beg = pack_index;

		long           block_pos = pos_beg;
		do
		{
			pack_index = pack_beg;
			long           block_len = pos_end - block_pos;
			block_len = std::min (block_len, _buf_len);

			const int      nbr_end_chn = _nbr_chn - chn_base;
			const int      group_end   = pack_index + _nbr_stages;

			// Mono source : first processing + interleaving
			if (mono_source_flag)
			{
				Pack4 &        pack = _pack_list [pack_index];
				pack.process_block_parallel (
					reinterpret_cast <fstb::ToolsSimd::VectF32 *> (&_tmp_buf [0]),
					in_ptr_arr [0] + block_pos,
					block_len
				);

				++ pack_index;
			}

			// Multi-channel source : interleaving only
			else
			{
				if (nbr_end_chn >= 4)
				{
					dsp::mix::Simd <fstb::DataAlign <true>, V128Src>::copy_4_4i (
						&_tmp_buf [0],
						in_ptr_arr [chn_base + 0] + block_pos,
						in_ptr_arr [chn_base + 1] + block_pos,
						in_ptr_arr [chn_base + 2] + block_pos,
						in_ptr_arr [chn_base + 3] + block_pos,
						block_len
					);
				}
				else
				{
					for (int chn = 0; chn < nbr_end_chn; ++chn)
					{
						dsp::mix::Generic::copy_1_ni1 (
							&_tmp_buf [chn],
							in_ptr_arr [chn_base + chn] + block_pos,
							block_len,
							4
						);
					}
				}
			}

			// Processing
			while (pack_index < group_end)
			{
				Pack4 &        pack = _pack_list [pack_index];
				pack.process_block_parallel (
					reinterpret_cast <fstb::ToolsSimd::VectF32 *> (&_tmp_buf [0]),
					reinterpret_cast <fstb::ToolsSimd::VectF32 *> (&_tmp_buf [0]),
					block_len
				);

				++ pack_index;
			}

			// Deinterleaving
			if (nbr_end_chn >= 4)
			{
				dsp::mix::Simd <V128Dest, fstb::DataAlign <true> >::copy_4i_4 (
					out_ptr_arr [chn_base + 0] + block_pos,
					out_ptr_arr [chn_base + 1] + block_pos,
					out_ptr_arr [chn_base + 2] + block_pos,
					out_ptr_arr [chn_base + 3] + block_pos,
					&_tmp_buf [0],
					block_len
				);
			}
			else
			{
				for (int chn = 0; chn < nbr_end_chn; ++chn)
				{
					dsp::mix::Generic::copy_ni1_1 (
						out_ptr_arr [chn_base + chn] + block_pos,
						&_tmp_buf [chn],
						block_len,
						4
					);
				}
			}

			block_pos += block_len;
		}
		while (block_pos < pos_end);
	}
}



template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::process_block_serial (float * const out_ptr_arr [], const float * const in_ptr_arr [], long pos_beg, long pos_end)
{
	int            pack_index = 0;
	for (int chn = 0; chn < _nbr_chn; ++chn)
	{
		process_block_serial_one_chn (
			out_ptr_arr [chn],
			in_ptr_arr [chn],
			pos_beg,
			pos_end,
			pack_index
		);
	}
}



template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::process_block_serial (float * const out_ptr_arr [], const float in_ptr [], long pos_beg, long pos_end)
{
	int            pack_index = 0;
	for (int chn = 0; chn < _nbr_chn; ++chn)
	{
		process_block_serial_one_chn (
			out_ptr_arr [chn],
			in_ptr,
			pos_beg,
			pos_end,
			pack_index
		);
	}
}



template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::process_block_serial_one_chn (float out_ptr [], const float in_ptr [], long pos_beg, long pos_end, int &pack_index)
{
	const float *  src_ptr = in_ptr;
	const int      group_end = pack_index + _group_size;
	do
	{
		Pack4 &        pack = _pack_list [pack_index];
		pack.process_block_serial_immediate (
			out_ptr + pos_beg,
			src_ptr + pos_beg,
			pos_end - pos_beg
		);

		src_ptr = out_ptr;
		++ pack_index;
	}
	while (pack_index < group_end);
}



template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::process_block_2x2 (float * const out_ptr_arr [], const float * const in_ptr_arr [], long pos_beg, long pos_end)
{
	assert (_nbr_packs == 1);

	Pack4 &        pack = _pack_list [0];
	long           pos = pos_beg;
	do
	{
		long           work_len = pos_end - pos;
		work_len = std::min (work_len, _buf_len);

		// Interleaving
		dsp::mix::Simd <
			fstb::DataAlign <true>,
			fstb::DataAlign <false>
		>::copy_2_2i (
			&_tmp_buf [0],
			in_ptr_arr [0] + pos,
			in_ptr_arr [1] + pos,
			work_len
		);

		// Processing
		pack.process_block_2x2_immediate (
			&_tmp_buf [0],
			&_tmp_buf [0],
			work_len
		);

		// Deinterleaving
		dsp::mix::Simd <
			fstb::DataAlign <false>,
			fstb::DataAlign <true>
		>::copy_2i_2 (
			out_ptr_arr [0] + pos,
			out_ptr_arr [1] + pos,
			&_tmp_buf [0],
			work_len
		);

		pos += work_len;
	}
	while (pos < pos_end);
}



template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::process_block_2x2 (float * const out_ptr_arr [], const float in_ptr [], long pos_beg, long pos_end)
{
	const float * const in_ptr_arr [2] = { in_ptr, in_ptr };
	process_block_2x2 (out_ptr_arr, in_ptr_arr, pos_beg, pos_end);
}



/*

Chn Stg Config
----------------
 2   2  2x2
 2   3  Serial
4*k  ?  Parallel
 ?   1  Parallel
 1   ?  Serial
 ?  4*k Serial
 ?   ?  Parallel

*/

template <class VD, class VS>
typename BiquadPackSimd <VD, VS>::ProcConf	BiquadPackSimd <VD, VS>::compute_proc_conf (int nbr_stages, int nbr_chn)
{
	assert (nbr_stages >= 0);
	assert (nbr_chn >= 0);

	ProcConf       proc_conf = ProcConf_PARALLEL;

	if (nbr_chn == 2 && nbr_stages == 2)
	{
		proc_conf = ProcConf_2X2;
	}
	else if (nbr_chn == 2 && nbr_stages == 3)
	{
		proc_conf = ProcConf_SERIAL;
	}
	else if ((nbr_chn & 3) == 0)
	{
		proc_conf = ProcConf_PARALLEL;
	}
	else if (nbr_stages == 1)
	{
		proc_conf = ProcConf_PARALLEL;
	}
	else if (nbr_chn == 1)
	{
		proc_conf = ProcConf_SERIAL;
	}
	else if ((nbr_stages & 3) == 0)
	{
		proc_conf = ProcConf_SERIAL;
	}

	return (proc_conf);
}



template <class VD, class VS>
void	BiquadPackSimd <VD, VS>::compute_config_info (ProcConf &proc_conf, int &nbr_packs, int &group_size, int nbr_stages, int nbr_chn)
{
	assert (&proc_conf != 0);
	assert (&nbr_packs != 0);
	assert (&group_size != 0);
	assert (nbr_stages >= 0);
	assert (nbr_chn >= 0);

	proc_conf = compute_proc_conf (nbr_stages, nbr_chn);

	switch (proc_conf)
	{
	case	ProcConf_PARALLEL:
		group_size = nbr_stages;
		nbr_packs  = ((nbr_chn + 3) >> 2) * group_size;
		break;

	case	ProcConf_SERIAL:
		group_size = (nbr_stages + 3) >> 2;
		nbr_packs  = nbr_chn * group_size;
		break;

	case	ProcConf_2X2:
		group_size = 1;
		nbr_packs  = 1;
		break;

	default:
		assert (false);
		break;
	}
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_BiquadPackSimd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
