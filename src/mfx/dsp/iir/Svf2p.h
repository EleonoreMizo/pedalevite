/*****************************************************************************

        Svf2p.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_Svf2p_HEADER_INCLUDED)
#define mfx_dsp_iir_Svf2p_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace iir
{



class Svf2p
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Type
	{
		Type_LOWPASS = 0,
		Type_HIGHPASS,
		Type_RESONATOR,
		Type_BANDPASS,
		Type_LOWSHELF,
		Type_HIGHSHELF,

		Type_NBR_ELT
	};

	               Svf2p ();
	               Svf2p (const Svf2p &other)      = default;
	virtual        ~Svf2p ()                       = default;

	Svf2p &        operator = (const Svf2p &other) = default;

	// Input
	inline void    set_sample_freq (float fs);
	inline float   get_sample_freq () const;
	inline void    set_freq (float f0);
	inline float   get_freq () const;
	inline void    set_type (Type type);
	inline Type    get_type () const;
	inline void    set_reso (float reso);
	inline float   get_reso () const;
	inline void    set_q (float q);
	inline float   get_q () const;

	void           update_eq ();

	// Output
	inline float   get_g0 () const;
	inline float   get_g1 () const;
	inline float   get_g2 () const;
	inline float   get_v0m () const;
	inline float   get_v1m () const;
	inline float   get_v2m () const;

	// Main conversion functions
	static void    conv_poles (float &g0, float &g1, float &g2, double f0_fs, float k);
	static void    conv_s_eq_to_svf (float &g0, float &g1, float &g2, float &v0m, float &v1m, float &v2m, const double b [3], const double a [3], double freq, double fs);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	float          _sample_freq;
	float          _f0;
	float          _q;
	Type           _type;
	float          _reso;

	float          _g0;
	float          _g1;
	float          _g2;
	float          _v0m;
	float          _v1m;
	float          _v2m;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Svf2p &other) const = delete;
	bool           operator != (const Svf2p &other) const = delete;

}; // class Svf2p



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/Svf2p.hpp"



#endif   // mfx_dsp_iir_Svf2p_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
