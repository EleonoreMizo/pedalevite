/*****************************************************************************

        Biquad4SimdMorph.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_Biquad4SimdMorph_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_Biquad4SimdMorph_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class VD, class VS, class VP>
Biquad4SimdMorph <VD, VS, VP>::Biquad4SimdMorph ()
:	_biq ()
/*,	_step_b ()
,	_step_a ()
,	_target_b ()
,	_target_a ()
,	_prog_b ()
,	_prog_a ()*/
{
	_nbr_rem_spl = 0;
	_ramp_len    = 64;
	_prog_flag   = false;
}



template <class VD, class VS, class VP>
Biquad4SimdMorph <VD, VS, VP>::Biquad4SimdMorph (const Biquad4SimdMorph <VD, VS, VP> &other)
:	_biq (other._biq)
/*,	_step_b ()
,	_step_a ()
,	_target_b ()
,	_target_a ()
,	_prog_b ()
,	_prog_a ()*/
{
	assert (&other != 0);

	V128Par::store_f32 (_step_b [0]  , V128Par::load_f32 (other._step_b [0]  ));
	V128Par::store_f32 (_step_b [1]  , V128Par::load_f32 (other._step_b [1]  ));
	V128Par::store_f32 (_step_b [2]  , V128Par::load_f32 (other._step_b [2]  ));
	V128Par::store_f32 (_step_a [1]  , V128Par::load_f32 (other._step_a [1]  ));
	V128Par::store_f32 (_step_a [2]  , V128Par::load_f32 (other._step_a [2]  ));
	V128Par::store_f32 (_target_b [0], V128Par::load_f32 (other._target_b [0]));
	V128Par::store_f32 (_target_b [1], V128Par::load_f32 (other._target_b [1]));
	V128Par::store_f32 (_target_b [2], V128Par::load_f32 (other._target_b [2]));
	V128Par::store_f32 (_target_a [1], V128Par::load_f32 (other._target_a [1]));
	V128Par::store_f32 (_target_a [2], V128Par::load_f32 (other._target_a [2]));
	V128Par::store_f32 (_prog_b [0]  , V128Par::load_f32 (other._prog_b [0]  ));
	V128Par::store_f32 (_prog_b [1]  , V128Par::load_f32 (other._prog_b [1]  ));
	V128Par::store_f32 (_prog_b [2]  , V128Par::load_f32 (other._prog_b [2]  ));
	V128Par::store_f32 (_prog_a [1]  , V128Par::load_f32 (other._prog_a [1]  ));
	V128Par::store_f32 (_prog_a [2]  , V128Par::load_f32 (other._prog_a [2]  ));

	_nbr_rem_spl = other._nbr_rem_spl;
	_ramp_len    = other._ramp_len;
	_prog_flag   = other._prog_flag;
}



template <class VD, class VS, class VP>
void	Biquad4SimdMorph <VD, VS, VP>::set_ramp_time (int nbr_spl)
{
	assert (nbr_spl > 0);

	const long     ramp_len_old = _ramp_len;
	if (nbr_spl != ramp_len_old)
	{
		_ramp_len = int (nbr_spl);

		// Ramp is initiated but has not started yet.
		if (_nbr_rem_spl == ramp_len_old)
		{
			const float    scale_flt = float (ramp_len_old) / float (_ramp_len);
			auto           scale = fstb::ToolsSimd::set1_f32 (scale_flt);

			auto           sb_0 = V128Par::load_f32 (_step_b [0]);
			auto           sb_1 = V128Par::load_f32 (_step_b [1]);
			auto           sb_2 = V128Par::load_f32 (_step_b [2]);
			auto           sa_1 = V128Par::load_f32 (_step_a [1]);
			auto           sa_2 = V128Par::load_f32 (_step_a [2]);

			V128Par::store_f32 (_step_b [0], sb_0 * scale);
			V128Par::store_f32 (_step_b [1], sb_1 * scale);
			V128Par::store_f32 (_step_b [2], sb_2 * scale);
			V128Par::store_f32 (_step_a [1], sa_1 * scale);
			V128Par::store_f32 (_step_a [2], sa_2 * scale);

			_nbr_rem_spl = _ramp_len;
		}
	}
}



template <class VD, class VS, class VP>
int	Biquad4SimdMorph <VD, VS, VP>::get_ramp_time () const
{
	return (_ramp_len);
}



template <class VD, class VS, class VP>
void	Biquad4SimdMorph <VD, VS, VP>::set_z_eq (const VectFlt4 b [3], const VectFlt4 a [3], bool ramp_flag)
{
	assert (b != 0);
	assert (a != 0);

	const auto     b0 = fstb::ToolsSimd::loadu_f32 (b [0]);
	const auto     b1 = fstb::ToolsSimd::loadu_f32 (b [1]);
	const auto     b2 = fstb::ToolsSimd::loadu_f32 (b [2]);
	const auto     a1 = fstb::ToolsSimd::loadu_f32 (a [1]);
	const auto     a2 = fstb::ToolsSimd::loadu_f32 (a [2]);

	// Immediate change
	if (! ramp_flag)
	{
		_biq.set_z_eq (b, a);
		_nbr_rem_spl = 0;
		_prog_flag = false;
	}

	// Ramp
	else
	{
		// We are not ramping yet, initiates a new ramp from scratch.
		// Or the ramp has not been processed yet, changes the ramp settings.
		if (   _nbr_rem_spl == 0
		    || _nbr_rem_spl == _ramp_len)
		{
			_biq.get_z_eq (_tmp_b, _tmp_a);

			_nbr_rem_spl = _ramp_len;
			_prog_flag = false;

			const float    step_flt = 1.0f / _ramp_len;
			const auto     step_mul = fstb::ToolsSimd::set1_f32 (step_flt);
			const auto     dif_b0   = b0 - fstb::ToolsSimd::load_f32 (_tmp_b [0]);
			const auto     dif_b1   = b1 - fstb::ToolsSimd::load_f32 (_tmp_b [1]);
			const auto     dif_b2   = b2 - fstb::ToolsSimd::load_f32 (_tmp_b [2]);
			const auto     dif_a1   = a1 - fstb::ToolsSimd::load_f32 (_tmp_a [1]);
			const auto     dif_a2   = a2 - fstb::ToolsSimd::load_f32 (_tmp_a [2]);
			const auto     step_b0  = dif_b0 * step_mul;
			const auto     step_b1  = dif_b1 * step_mul;
			const auto     step_b2  = dif_b2 * step_mul;
			const auto     step_a1  = dif_a1 * step_mul;
			const auto     step_a2  = dif_a2 * step_mul;
			V128Par::store_f32 (_step_b [0], step_b0);
			V128Par::store_f32 (_step_b [1], step_b1);
			V128Par::store_f32 (_step_b [2], step_b2);
			V128Par::store_f32 (_step_a [1], step_a1);
			V128Par::store_f32 (_step_a [2], step_a2);

			V128Par::store_f32 (_target_b [0], b0);
			V128Par::store_f32 (_target_b [1], b1);
			V128Par::store_f32 (_target_b [2], b2);
			V128Par::store_f32 (_target_a [1], a1);
			V128Par::store_f32 (_target_a [2], a2);
		}

		// We are currently ramping
		else if (_nbr_rem_spl != 0)
		{
			_prog_flag = true;

			V128Par::store_f32 (_prog_b [0], b0);
			V128Par::store_f32 (_prog_b [1], b1);
			V128Par::store_f32 (_prog_b [2], b2);
			V128Par::store_f32 (_prog_a [1], a1);
			V128Par::store_f32 (_prog_a [2], a2);
		}
	}
}



template <class VD, class VS, class VP>
void	Biquad4SimdMorph <VD, VS, VP>::set_z_eq_same (const float b [3], const float a [3], bool ramp_flag)
{
	assert (b != 0);
	assert (a != 0);

	V128Par::store_f32 (_tmp_b [0], fstb::ToolsSimd::set1_f32 (b [0]));
	V128Par::store_f32 (_tmp_b [1], fstb::ToolsSimd::set1_f32 (b [1]));
	V128Par::store_f32 (_tmp_b [2], fstb::ToolsSimd::set1_f32 (b [2]));
	V128Par::store_f32 (_tmp_a [1], fstb::ToolsSimd::set1_f32 (a [1]));
	V128Par::store_f32 (_tmp_a [2], fstb::ToolsSimd::set1_f32 (a [2]));

	set_z_eq (_tmp_b, _tmp_a, ramp_flag);
}



template <class VD, class VS, class VP>
void	Biquad4SimdMorph <VD, VS, VP>::set_z_eq_one (int biq, const float b [3], const float a [3], bool ramp_flag)
{
	assert (biq >= 0);
	assert (biq < BiqSimd::_nbr_units);
	assert (b != 0);
	assert (a != 0);

	// Immediate change
	if (! ramp_flag)
	{
		_biq.set_z_eq_one (biq, b, a);
		if (_nbr_rem_spl > 0)
		{
			_step_b [0] [biq] = 0;
			_step_b [1] [biq] = 0;
			_step_b [2] [biq] = 0;
			_step_a [1] [biq] = 0;
			_step_a [2] [biq] = 0;

			_target_b [0] [biq] = b [0];
			_target_b [1] [biq] = b [1];
			_target_b [2] [biq] = b [2];
			_target_a [1] [biq] = a [1];
			_target_a [2] [biq] = a [2];

			if (_prog_flag)
			{
				_prog_b [0] [biq] = b [0];
				_prog_b [1] [biq] = b [1];
				_prog_b [2] [biq] = b [2];
				_prog_a [1] [biq] = a [1];
				_prog_a [2] [biq] = a [2];
			}
		}
	}

	// Ramp
	else
	{
		// We are not ramping yet, initiates a new ramp from scratch.
		if (_nbr_rem_spl == 0)
		{
			assert (! _prog_flag);

			_biq.get_z_eq (_target_b, _target_a);
			V128Par::store_f32 (_step_b [0], fstb::ToolsSimd::set_f32_zero ());
			V128Par::store_f32 (_step_b [1], fstb::ToolsSimd::set_f32_zero ());
			V128Par::store_f32 (_step_b [2], fstb::ToolsSimd::set_f32_zero ());
			V128Par::store_f32 (_step_a [1], fstb::ToolsSimd::set_f32_zero ());
			V128Par::store_f32 (_step_a [2], fstb::ToolsSimd::set_f32_zero ());
			_nbr_rem_spl = _ramp_len;

			const float    step_flt = 1.0f / _ramp_len;
			_step_b [0] [biq] = (b [0] - _target_b [0] [biq]) * step_flt;
			_step_b [1] [biq] = (b [1] - _target_b [1] [biq]) * step_flt;
			_step_b [2] [biq] = (b [2] - _target_b [2] [biq]) * step_flt;
			_step_a [1] [biq] = (a [1] - _target_a [1] [biq]) * step_flt;
			_step_a [2] [biq] = (a [2] - _target_a [2] [biq]) * step_flt;

			_target_b [0] [biq] = b [0];
			_target_b [1] [biq] = b [1];
			_target_b [2] [biq] = b [2];
			_target_a [1] [biq] = a [1];
			_target_a [2] [biq] = a [2];
		}

		// We are currently ramping
		else
		{
			// The ramp has not been processed yet, changes the ramp settings.
			if (_nbr_rem_spl == _ramp_len)
			{
				float	         b_old [3];
				float	         a_old [3];
				_biq.get_z_eq_one (biq, b_old, a_old);

				const float    step_flt = 1.0f / _ramp_len;
				_step_b [0] [biq] = (b [0] - b_old [0]) * step_flt;
				_step_b [1] [biq] = (b [1] - b_old [1]) * step_flt;
				_step_b [2] [biq] = (b [2] - b_old [2]) * step_flt;
				_step_a [1] [biq] = (a [1] - a_old [1]) * step_flt;
				_step_a [2] [biq] = (a [2] - a_old [2]) * step_flt;

				_target_b [0] [biq] = b [0];
				_target_b [1] [biq] = b [1];
				_target_b [2] [biq] = b [2];
				_target_a [1] [biq] = a [1];
				_target_a [2] [biq] = a [2];
			}

			// The ramp has already started, programs the subsequent one.
			else
			{
				if (! _prog_flag)
				{
					V128Par::store_f32 (_prog_b [0], V128Par::load_f32 (_target_b [0]));
					V128Par::store_f32 (_prog_b [1], V128Par::load_f32 (_target_b [1]));
					V128Par::store_f32 (_prog_b [2], V128Par::load_f32 (_target_b [2]));
					V128Par::store_f32 (_prog_a [1], V128Par::load_f32 (_target_a [1]));
					V128Par::store_f32 (_prog_a [2], V128Par::load_f32 (_target_a [2]));

					_prog_flag = true;
				}

				_prog_b [0] [biq] = b [0];
				_prog_b [1] [biq] = b [1];
				_prog_b [2] [biq] = b [2];
				_prog_a [1] [biq] = a [1];
				_prog_a [2] [biq] = a [2];
			}
		}
	}
}



template <class VD, class VS, class VP>
void	Biquad4SimdMorph <VD, VS, VP>::neutralise (bool ramp_flag)
{
	V128Par::store_f32 (&_tmp_b [0] [0], fstb::ToolsSimd::set1_f32 (1));
	V128Par::store_f32 (&_tmp_b [1] [0], fstb::ToolsSimd::set1_f32 (0));
	V128Par::store_f32 (&_tmp_b [2] [0], fstb::ToolsSimd::set1_f32 (0));

	set_z_eq (_tmp_b, _tmp_b, ramp_flag);
}



template <class VD, class VS, class VP>
void	Biquad4SimdMorph <VD, VS, VP>::neutralise_one (int biq, bool ramp_flag)
{
	const float    ab [3] = { 1, 0, 0 };
	set_z_eq_one (ab, ab, ramp_flag);
}



template <class VD, class VS, class VP>
void	Biquad4SimdMorph <VD, VS, VP>::get_z_eq (VectFlt4 b [3], VectFlt4 a [3]) const
{
	_biq.get_z_eq (b, a);
}



template <class VD, class VS, class VP>
void	Biquad4SimdMorph <VD, VS, VP>::get_z_eq_one (int biq, float b [3], float a [3]) const
{
	_biq.get_z_eq_one (biq, b, a);
}



template <class VD, class VS, class VP>
void	Biquad4SimdMorph <VD, VS, VP>::get_z_eq_one_target (int biq, float b [3], float a [3]) const
{
	assert (biq >= 0);
	assert (biq < BiqSimd::_nbr_units);
	assert (b != 0);
	assert (a != 0);


	if (_nbr_rem_spl == 0)
	{
		_biq.get_z_eq_one (biq, b, a);
	}
	else
	{
		if (_prog_flag)
		{
			b [0] = _prog_b [0] [biq];
			b [1] = _prog_b [1] [biq];
			b [2] = _prog_b [2] [biq];
			a [1] = _prog_a [1] [biq];
			a [2] = _prog_a [2] [biq];
		}
		else
		{
			b [0] = _target_b [0] [biq];
			b [1] = _target_b [1] [biq];
			b [2] = _target_b [2] [biq];
			a [1] = _target_a [1] [biq];
			a [2] = _target_a [2] [biq];
		}
	}
}



template <class VD, class VS, class VP>
void	Biquad4SimdMorph <VD, VS, VP>::set_state_one (int biq, float const mem_x [2], const float mem_y [2])
{
	_biq.set_state_one (biq, mem_x, mem_y);
}



template <class VD, class VS, class VP>
void	Biquad4SimdMorph <VD, VS, VP>::get_state_one (int biq, float mem_x [2], float mem_y [2]) const
{
	_biq.get_state_one (biq, mem_x, mem_y);
}



template <class VD, class VS, class VP>
bool	Biquad4SimdMorph <VD, VS, VP>::is_ramping () const
{
	return (_nbr_rem_spl > 0);
}



template <class VD, class VS, class VP>
void	Biquad4SimdMorph <VD, VS, VP>::process_block_parallel (fstb::ToolsSimd::VectF32 out_ptr [], const fstb::ToolsSimd::VectF32 in_ptr [], long nbr_spl)
{
	long           pos = 0;
	do
	{
		long           work_len = nbr_spl - pos;
		if (_nbr_rem_spl > 0)
		{
			work_len = std::min (work_len, long (_nbr_rem_spl));
			_biq.process_block_parallel (
				out_ptr + pos,
				in_ptr + pos,
				work_len,
				reinterpret_cast <const fstb::ToolsSimd::VectF32 *> (_step_b),
				reinterpret_cast <const fstb::ToolsSimd::VectF32 *> (_step_a)
			);
		}
		else
		{
			_biq.process_block_parallel (
				out_ptr + pos,
				in_ptr + pos,
				work_len
			);
		}

		handle_ramp_post (work_len);
		pos += work_len;
	}
	while (pos < nbr_spl);
}



template <class VD, class VS, class VP>
void	Biquad4SimdMorph <VD, VS, VP>::process_block_parallel (fstb::ToolsSimd::VectF32 out_ptr [], const float in_ptr [], long nbr_spl)
{
	long           pos = 0;
	do
	{
		long           work_len = nbr_spl - pos;
		if (_nbr_rem_spl > 0)
		{
			work_len = std::min (work_len, long (_nbr_rem_spl));
			_biq.process_block_parallel (
				out_ptr + pos,
				in_ptr + pos,
				work_len,
				reinterpret_cast <const fstb::ToolsSimd::VectF32 *> (_step_b),
				reinterpret_cast <const fstb::ToolsSimd::VectF32 *> (_step_a)
			);
		}
		else
		{
			_biq.process_block_parallel (
				out_ptr + pos,
				in_ptr + pos,
				work_len
			);
		}

		handle_ramp_post (work_len);
		pos += work_len;
	}
	while (pos < nbr_spl);
}



template <class VD, class VS, class VP>
void	Biquad4SimdMorph <VD, VS, VP>::process_block_serial_latency (float out_ptr [], const float in_ptr [], long nbr_spl)
{
	long           pos = 0;
	do
	{
		long           work_len = nbr_spl - pos;
		if (_nbr_rem_spl > 0)
		{
			work_len = std::min (work_len, long (_nbr_rem_spl));
			_biq.process_block_serial_latency (
				out_ptr + pos,
				in_ptr + pos,
				work_len,
				reinterpret_cast <const fstb::ToolsSimd::VectF32 *> (_step_b),
				reinterpret_cast <const fstb::ToolsSimd::VectF32 *> (_step_a)
			);
		}
		else
		{
			_biq.process_block_serial_latency (
				out_ptr + pos,
				in_ptr + pos,
				work_len
			);
		}

		handle_ramp_post (work_len);
		pos += work_len;
	}
	while (pos < nbr_spl);
}



template <class VD, class VS, class VP>
void	Biquad4SimdMorph <VD, VS, VP>::process_block_serial_immediate (float out_ptr [], const float in_ptr [], long nbr_spl)
{
	long           pos = 0;
	do
	{
		long           work_len = nbr_spl - pos;
		if (_nbr_rem_spl > 0)
		{
			work_len = std::min (work_len, long (_nbr_rem_spl));
			_biq.process_block_serial_immediate (
				out_ptr + pos,
				in_ptr + pos,
				work_len,
				reinterpret_cast <const fstb::ToolsSimd::VectF32 *> (_step_b),
				reinterpret_cast <const fstb::ToolsSimd::VectF32 *> (_step_a)
			);
		}
		else
		{
			_biq.process_block_serial_immediate (
				out_ptr + pos,
				in_ptr + pos,
				work_len
			);
		}

		handle_ramp_post (work_len);
		pos += work_len;
	}
	while (pos < nbr_spl);
}



template <class VD, class VS, class VP>
void	Biquad4SimdMorph <VD, VS, VP>::process_block_2x2_latency (float out_ptr [], const float in_ptr [], long nbr_spl)
{
	long           pos = 0;
	do
	{
		long           work_len = nbr_spl - pos;
		if (_nbr_rem_spl > 0)
		{
			work_len = std::min (work_len, long (_nbr_rem_spl));
			_biq.process_block_2x2_latency (
				out_ptr + pos * 2,
				in_ptr + pos * 2,
				work_len,
				reinterpret_cast <const fstb::ToolsSimd::VectF32 *> (_step_b),
				reinterpret_cast <const fstb::ToolsSimd::VectF32 *> (_step_a)
			);
		}
		else
		{
			_biq.process_block_2x2_latency (
				out_ptr + pos * 2,
				in_ptr + pos * 2,
				work_len
			);
		}

		handle_ramp_post (work_len);
		pos += work_len;
	}
	while (pos < nbr_spl);
}



template <class VD, class VS, class VP>
void	Biquad4SimdMorph <VD, VS, VP>::process_block_2x2_immediate (float out_ptr [], const float in_ptr [], long nbr_spl)
{
	long           pos = 0;
	do
	{
		long           work_len = nbr_spl - pos;
		if (_nbr_rem_spl > 0)
		{
			work_len = std::min (work_len, long (_nbr_rem_spl));
			_biq.process_block_2x2_immediate (
				out_ptr + pos * 2,
				in_ptr + pos * 2,
				work_len,
				reinterpret_cast <const fstb::ToolsSimd::VectF32 *> (_step_b),
				reinterpret_cast <const fstb::ToolsSimd::VectF32 *> (_step_a)
			);
		}
		else
		{
			_biq.process_block_2x2_immediate (
				out_ptr + pos * 2,
				in_ptr + pos * 2,
				work_len
			);
		}

		handle_ramp_post (work_len);
		pos += work_len;
	}
	while (pos < nbr_spl);
}



template <class VD, class VS, class VP>
void	Biquad4SimdMorph <VD, VS, VP>::clear_buffers ()
{
	_biq.clear_buffers ();

	if (_nbr_rem_spl > 0)
	{
		if (_prog_flag)
		{
			_biq.set_z_eq (_prog_b, _prog_a);
		}

		else
		{
			_biq.set_z_eq (_target_b, _target_a);
		}
	}

	_nbr_rem_spl = 0;
	_prog_flag = false;
}



template <class VD, class VS, class VP>
void	Biquad4SimdMorph <VD, VS, VP>::clear_buffers_one (int biq)
{
	assert (biq >= 0);
	assert (biq < BiqSimd::_nbr_units);

	_biq.clear_buffers_one (biq);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class VD, class VS, class VP>
void	Biquad4SimdMorph <VD, VS, VP>::handle_ramp_post (long nbr_spl)
{
	assert (nbr_spl > 0);
	assert (_nbr_rem_spl <= 0 || nbr_spl <= _nbr_rem_spl);

	if (_nbr_rem_spl > 0)
	{
		_nbr_rem_spl -= nbr_spl;
		assert (_nbr_rem_spl >= 0);
		if (_nbr_rem_spl == 0)
		{
			// Make sure we continue with the exact equation (because of the
			// possible rounding errors during the ramp).
			_biq.set_z_eq (_target_b, _target_a);

			if (_prog_flag)
			{
				const auto     b0 = V128Par::load_f32 (_prog_b [0]);
				const auto     b1 = V128Par::load_f32 (_prog_b [1]);
				const auto     b2 = V128Par::load_f32 (_prog_b [2]);
				const auto     a1 = V128Par::load_f32 (_prog_a [1]);
				const auto     a2 = V128Par::load_f32 (_prog_a [2]);

				const auto     b0_old = V128Par::load_f32 (_target_b [0]);
				const auto     b1_old = V128Par::load_f32 (_target_b [1]);
				const auto     b2_old = V128Par::load_f32 (_target_b [2]);
				const auto     a1_old = V128Par::load_f32 (_target_a [1]);
				const auto     a2_old = V128Par::load_f32 (_target_a [2]);

				const auto     b0_dif = b0 - b0_old;
				const auto     b1_dif = b1 - b1_old;
				const auto     b2_dif = b2 - b2_old;
				const auto     a1_dif = a1 - a1_old;
				const auto     a2_dif = a2 - a2_old;

				const float    step_flt = 1.0f / _ramp_len;
				const auto     step = fstb::ToolsSimd::set1_f32 (step_flt);

				V128Par::store_f32 (_step_b [0], b0_dif * step);
				V128Par::store_f32 (_step_b [1], b1_dif * step);
				V128Par::store_f32 (_step_b [2], b2_dif * step);
				V128Par::store_f32 (_step_a [1], a1_dif * step);
				V128Par::store_f32 (_step_a [2], a2_dif * step);

				V128Par::store_f32 (_target_b [0], b0);
				V128Par::store_f32 (_target_b [1], b1);
				V128Par::store_f32 (_target_b [2], b2);
				V128Par::store_f32 (_target_a [1], a1);
				V128Par::store_f32 (_target_a [2], a2);

				_prog_flag   = false;
				_nbr_rem_spl = _ramp_len;
			}
		}
	}
}



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_Biquad4SimdMorph_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
