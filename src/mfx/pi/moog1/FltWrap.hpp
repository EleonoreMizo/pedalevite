/*****************************************************************************

        FltWrap.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_pi_moog1_FltWrap_CODEHEADER_INCLUDED)
#define mfx_pi_moog1_FltWrap_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace moog1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class F>
void	FltWrap <F>::init (float sample_freq, float max_mod_freq, float sig_scale)
{
	_filter.set_scale (sig_scale);
	_filter.set_sample_freq (sample_freq);
	_filter.set_max_mod_freq (max_mod_freq);
}



template <class F>
void	FltWrap <F>::set_param (float freq, float reso, float gcomp)
{
	_filter.set_freq_compensated (freq);
	_filter.set_reso_norm (reso);
	_filter.set_gain_comp (gcomp);
}



template <class F>
float	FltWrap <F>::process_sample_pitch_mod (float x, float m, float stage_in_ptr [])
{
	return _filter.process_sample_pitch_mod (x, m, stage_in_ptr);
}



template <class F>
void	FltWrap <F>::process_block (float dst_ptr [], const float src_ptr [], int nbr_spl)
{
	_filter.process_block (dst_ptr, src_ptr, nbr_spl);
}



template <class F>
void	FltWrap <F>::process_block_pitch_mod (float dst_ptr [], const float src_ptr [], const float mod_ptr [], int nbr_spl)
{
	_filter.process_block_pitch_mod (dst_ptr, src_ptr, mod_ptr, nbr_spl);
}



template <class F>
void	FltWrap <F>::clear_buffers ()
{
	_filter.clear_buffers ();
}



template <class F>
dsp::va::MoogLadderDAngeloData <4> &	FltWrap <F>::use_data ()
{
	return _filter.use_data ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace moog1
}  // namespace pi
}  // namespace mfx



#endif   // mfx_pi_moog1_FltWrap_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
