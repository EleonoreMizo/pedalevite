/*****************************************************************************

        FreeverbCore.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_spat_fv_FreeverbCore_HEADER_INCLUDED)
#define mfx_dsp_spat_fv_FreeverbCore_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/AllocAlign.h"
#include "mfx/dsp/spat/fv/DelayAllPassSimd.h"
#include "mfx/dsp/spat/fv/DelayComb.h"

#include <array>
#include <vector>



namespace mfx
{
namespace dsp
{
namespace spat
{
namespace fv
{



class FreeverbCore
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _max_nbr_chn = 2;

	               FreeverbCore ();
	               FreeverbCore (const FreeverbCore &other) = default;
	               FreeverbCore (FreeverbCore &&other)      = default;

	               ~FreeverbCore ()                         = default;

	FreeverbCore & operator = (const FreeverbCore &other)   = default;
	FreeverbCore & operator = (FreeverbCore &&other)        = default;

	void           reset (double sample_freq, int max_buf_len);
	void           set_reflectivity (float fdbk) noexcept;
	void           set_reflectivity (float fdbk, int chn_index) noexcept;
	void           set_damp (float damp) noexcept;
	void           set_damp (float damp, int chn_index) noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl, int chn_index) noexcept;
	void           clear_buffers () noexcept;

	static const float
	               _scalewet;
	static const float
	               _scalein;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _nbr_comb = 8;
	static const int  _nbr_ap   = 4;

	typedef std::array <DelayComb, _nbr_comb> CombArray;
	typedef std::array <DelayAllPassSimd, _nbr_ap> AllPassArray;

	class Channel
	{
	public:
		CombArray      _comb_arr;
		AllPassArray   _ap_arr;
	};

	typedef std::array <Channel, _max_nbr_chn> ChnArray;

	double         _sample_freq = 0;    // Hz, > 0. <= 0: not initialized
	ChnArray       _chn_arr;
	std::vector <float, fstb::AllocAlign <float, 16> >
	               _buf;

	static const int
	               _stereospread        = 23;
	static const int
	               _stereospread_mp     =  5;
	static const std::array <int, _nbr_comb>
	               _comb_len_arr;
	static const std::array <int, _nbr_ap>
	               _ap_len_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const FreeverbCore &other) const = delete;
	bool           operator != (const FreeverbCore &other) const = delete;

}; // class FreeverbCore



}  // namespace fv
}  // namespace spat
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/spat/fv/FreeverbCore.hpp"



#endif   // mfx_dsp_spat_fv_FreeverbCore_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
