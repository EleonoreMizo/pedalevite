/*****************************************************************************

        ChnMerger.h
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ana_r128_ChnMerger_HEADER_INCLUDED)
#define mfx_dsp_ana_r128_ChnMerger_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/dsp/ana/r128/FilterK.h"
#include "mfx/dsp/ana/r128/MeanSq.h"

#include <vector>



namespace mfx
{
namespace dsp
{
namespace ana
{
namespace r128
{



class ChnMerger
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           set_sample_freq (double sample_freq);
	void           set_nbr_chn (int nbr_chn);
	void           set_win_len (double t);

	void           set_chn_weights (const float weight_arr []) noexcept;
	void           set_chn_buffers (const float *src_ptr_arr []) noexcept;

	void           analyse_sample (int pos) noexcept;
	void           analyse_block (int pos_beg, int pos_end) noexcept;
	float          compute_msq () const noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int _buf_size = 256;

	class Channel
	{
	public:
		float          _weight  = 1;
		const float *  _src_ptr = nullptr;
		FilterK        _flt_k;
		MeanSq         _flt_msq;
	};
	typedef std::vector <Channel> ChannelArray;

	inline bool    is_chn_count_set () const noexcept;
	inline bool    is_setup () const noexcept;
	inline bool    is_ready () const noexcept;
	void           update_time_param ();

	double         _sample_freq = 0;    // Sampling frequency in Hz, > 0. 0 = not set
	double         _win_dur     = 0;    // 
	ChannelArray   _chn_arr;




/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const ChnMerger &other) const = delete;
	bool           operator != (const ChnMerger &other) const = delete;

}; // class ChnMerger



}  // namespace r128
}  // namespace ana
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/ana/r128/ChnMerger.hpp"



#endif   // mfx_dsp_ana_r128_ChnMerger_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
