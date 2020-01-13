/*****************************************************************************

        InterpFirPolyphase.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_rspl_InterpFirPolyphase_CODEHEADER_INCLUDED)
#define mfx_dsp_rspl_InterpFirPolyphase_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace rspl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: set_impulse
Description:
	Set the FIR impulse. Its length should be IMPULSE_LEN. Theoretically its
	length is odd, with a padding 0 at the beginning.
	This function must be called at least once before using interpolate().
Input parameters:
	- impulse_ptr: pointer on the first coefficient of the impulse.
Throws: Nothing
==============================================================================
*/

template <class IP, int NPL2>
void	InterpFirPolyphase <IP, NPL2>::set_impulse (const double impulse_ptr [IMPULSE_LEN])
{
	assert (impulse_ptr != nullptr);

	double         next_coef_dbl = 0;

	for (int fir_pos = PHASE_LEN - 1; fir_pos >= 0; --fir_pos)
	{
		for (int phase_cnt = NBR_PHASES - 1; phase_cnt >= 0; --phase_cnt)
		{
			const int      imp_pos  = fir_pos * NBR_PHASES + phase_cnt;
			const double   coef_dbl = impulse_ptr [imp_pos];

			const float    coef = float (coef_dbl);
			const float    dif  = float (next_coef_dbl - coef_dbl);

			const int      table_pos = PHASE_LEN - 1 - fir_pos;
			InterpPhase &  phase     = _phase_arr [phase_cnt];
			phase.set_data (table_pos, coef, dif);

			next_coef_dbl = coef_dbl;
		}
	}
}



/*
==============================================================================
Name: interpolate
Description:
	Interpolate sample data.
Input parameters:
	- data_ptr: pointer on sample data, at the position of interpolation.
		Latency caused by the FIR has to be fixed manually (by offsetting
		data_ptr, for example).
	- frac_pos: fractional interpolation position, full 32-bit scale.
Returns: Interpolated sample
Throws: Nothing
==============================================================================
*/

template <class IP, int NPL2>
float	InterpFirPolyphase <IP, NPL2>::interpolate (const float data_ptr [], uint32_t frac_pos) const
{
	assert (data_ptr != nullptr);

	float          q;
	int            phase_index;
	compute_phase_info (q, phase_index, frac_pos);

	const InterpPhase &  phase = _phase_arr [phase_index];
	const float          val   = phase.convolve (data_ptr, q);

	return (val);
}



template <class IP, int NPL2>
void	InterpFirPolyphase <IP, NPL2>::interpolate_multi_chn (float * const out_ptr_arr [], long out_offset, const float * const in_ptr_arr [], long in_offset, uint32_t frac_pos, int nbr_chn) const
{
	assert (out_ptr_arr != nullptr);
	assert (in_ptr_arr != nullptr);
	assert (nbr_chn > 0);

	float          q;
	int            phase_index;
	compute_phase_info (q, phase_index, frac_pos);

	const InterpPhase &  phase = _phase_arr [phase_index];

	typename InterpPhase::Buffer  impulse;
	phase.precompute_impulse (impulse, q);

	for (int chn_index = 0; chn_index < nbr_chn; ++ chn_index)
	{
		const float *	in_ptr = in_ptr_arr [chn_index];
		assert (in_ptr != nullptr);
		const float		value = phase.convolve (in_ptr + in_offset, impulse);
		float *			out_ptr = out_ptr_arr [chn_index];
		out_ptr [out_offset] = value;
	}
}



template <class IP, int NPL2>
void	InterpFirPolyphase <IP, NPL2>::interpolate_multi_chn (float out_ptr [], const float in_ptr [], uint32_t frac_pos, int nbr_chn, long chn_stride_in, long chn_stride_out) const
{
	assert (out_ptr != nullptr);
	assert (in_ptr != nullptr);
	assert (nbr_chn > 0);

	float          q;
	int            phase_index;
	compute_phase_info (q, phase_index, frac_pos);

	const InterpPhase &  phase = _phase_arr [phase_index];
	
	typename InterpPhase::Buffer  impulse;
	phase.precompute_impulse (impulse, q);

	for (int chn_index = 0; chn_index < nbr_chn; ++ chn_index)
	{
		*out_ptr = phase.convolve (in_ptr, impulse);

		in_ptr  += chn_stride_in;
		out_ptr += chn_stride_out;
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class IP, int NPL2>
void	InterpFirPolyphase <IP, NPL2>::compute_phase_info (float &q, int &phase_index, uint32_t frac_pos)
{
	// q is made of the lower bits of the fractional position, scaled in the
	// range [0 ; 1[.
	const float    q_scl = 1.0f / (65536.0f * 65536.0f);
	q = float (frac_pos << NBR_PHASES_L2) * q_scl;

	// Compute phase index (the high-order bits)
	phase_index = frac_pos >> (32 - NBR_PHASES_L2);
}



}  // namespace rspl
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_rspl_InterpFirPolyphase_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
