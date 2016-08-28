/*****************************************************************************

        Phase90Simd.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_pi_phase1_Phase90Simd_CODEHEADER_INCLUDED)
#define mfx_pi_phase1_Phase90Simd_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace phase1
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int NC>
int	Phase90Simd <NC>::do_get_nbr_coefs () const
{
	return NC;
}



template <int NC>
void	Phase90Simd <NC>::do_set_coefs (const double coef_arr [])
{
	_phaser.set_coefs (coef_arr);
}



template <int NC>
void	Phase90Simd <NC>::do_clear_buffers ()
{
	_phaser.clear_buffers ();
}



template <int NC>
void	Phase90Simd <NC>::do_process_block (float dst_0_ptr [], float dst_1_ptr [], const float src_ptr [], int nbr_spl)
{
	_phaser.process_block (dst_0_ptr, dst_1_ptr, src_ptr, nbr_spl);
}



template <int NC>
void	Phase90Simd <NC>::do_process_sample (float &dst_0, float &dst_1, float src)
{
	_phaser.process_sample (dst_0, dst_1, src);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace phase1
}  // namespace pi
}  // namespace mfx



#endif   // mfx_pi_phase1_Phase90Simd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
