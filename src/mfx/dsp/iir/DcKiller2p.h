/*****************************************************************************

        DcKiller2p.h
        Author: Laurent de Soras, 2020

Forward Euler-based SFV, simplified
Algorithm by Andrew Simper
https://discord.com/channels/507604115854065674/507630527847596046/714703152368058399

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_DcKiller2p_HEADER_INCLUDED)
#define mfx_dsp_iir_DcKiller2p_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace iir
{



class DcKiller2p
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_sample_freq (double sample_freq);
	void           set_cutoff_freq (float f0);
	inline void    copy_z_eq (const DcKiller2p &other);
	inline float   process_sample (float x);
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl);

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           update_coefs ();

	float          _sample_freq = 0; // Sampling rate, Hz, > 0. 0 = not set
	float          _cutoff_freq = 5; // Cutoff frequency, Hz, > 0

	// Coefficients
	float          _m  = 1;
	float          _d  = 0;
	float          _g  = 0;

	// States
	float          _v1 = 0;
	float          _v2 = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const DcKiller2p &other) const = delete;
	bool           operator != (const DcKiller2p &other) const = delete;

}; // class DcKiller2p



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/iir/DcKiller2p.hpp"



#endif   // mfx_dsp_iir_DcKiller2p_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
