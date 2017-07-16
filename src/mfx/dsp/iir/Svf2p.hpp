/*****************************************************************************

        Svf2p.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_iir_Svf2p_CODEHEADER_INCLUDED)
#define mfx_dsp_iir_Svf2p_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace iir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Svf2p::set_sample_freq (float fs)
{
	assert (fs > 0);

	_sample_freq =        fs;
	_inv_fs      = 1.0f / fs;
}



float	Svf2p::get_sample_freq () const
{
	return _sample_freq;
}



void	Svf2p::set_freq (float f0)
{
	assert (f0 > 0);
	assert (f0 < _sample_freq * 0.5f);

	_f0 = f0;
}



float	Svf2p::get_freq () const
{
	return _f0;
}



void	Svf2p::set_type (Type type)
{
	assert (type >= 0);
	assert (type < Type_NBR_ELT);

	_type = type;
}



Svf2p::Type	Svf2p::get_type () const
{
	return _type;
}



void	Svf2p::set_reso (float reso)
{
	_reso = reso;
}



float	Svf2p::get_reso () const
{
	return _reso;
}



void	Svf2p::set_q (float q)
{
	assert (q > 0);

	_q = q;
}



float	Svf2p::get_q () const
{
	return _q;
}



float	Svf2p::get_g0 () const
{
	return _g0;
}



float	Svf2p::get_g1 () const
{
	return _g1;
}



float	Svf2p::get_g2 () const
{
	return _g2;
}



float	Svf2p::get_v0m () const
{
	return _v0m;
}



float	Svf2p::get_v1m () const
{
	return _v1m;
}



float	Svf2p::get_v2m () const
{
	return _v2m;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_iir_Svf2p_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
