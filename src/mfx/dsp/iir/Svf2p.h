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

#include "fstb/Vf32.h"



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
		Type_ALLPASS,

		Type_NBR_ELT
	};

	               Svf2p () noexcept;
	               Svf2p (const Svf2p &other)      = default;
	               Svf2p (Svf2p &&other)           = default;

	virtual        ~Svf2p ()                       = default;

	Svf2p &        operator = (const Svf2p &other) = default;
	Svf2p &        operator = (Svf2p &&other)      = default;

	// Input
	inline void    set_sample_freq (float fs) noexcept;
	inline float   get_sample_freq () const noexcept;
	inline void    set_freq (float f0) noexcept;
	inline float   get_freq () const noexcept;
	inline void    set_type (Type type) noexcept;
	inline Type    get_type () const noexcept;
	inline void    set_reso (float reso) noexcept;
	inline float   get_reso () const noexcept;
	inline void    set_q (float q) noexcept;
	inline float   get_q () const noexcept;

	void           update_eq () noexcept;

	// Output
	inline float   get_g0 () const noexcept;
	inline float   get_g1 () const noexcept;
	inline float   get_g2 () const noexcept;
	inline float   get_v0m () const noexcept;
	inline float   get_v1m () const noexcept;
	inline float   get_v2m () const noexcept;

	// Main conversion functions
	static void    conv_poles (float &g0, float &g1, float &g2, double f0_fs, float k) noexcept;
	template <typename TE>
	static void    conv_s_eq_to_svf (float &g0, float &g1, float &g2, float &v0m, float &v1m, float &v2m, const TE b [3], const TE a [3], double freq, double fs) noexcept;

	static void    approx_s1s2 (fstb::Vf32 &s1, fstb::Vf32 &s2, fstb::Vf32 f0_fs) noexcept;
	static void    conv_poles (fstb::Vf32 &g0, fstb::Vf32 &g1, fstb::Vf32 &g2, fstb::Vf32 f0_fs, fstb::Vf32 k) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	float          _sample_freq = 44100;
	float          _inv_fs      = 1.f / _sample_freq;
	float          _f0          = 1000;
	float          _q           = 0.5f;
	Type           _type        = Type_RESONATOR;
	float          _reso        = 0;

	float          _g0  = 0;
	float          _g1  = 0;
	float          _g2  = 0;
	float          _v0m = 1;
	float          _v1m = 0;
	float          _v2m = 0;



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
