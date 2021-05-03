/*****************************************************************************

        CosineSum.h
        Author: Laurent de Soras, 2018

Taken from "A Family Of Cosine-Sum Windows For High-Resolution Measurements"
by Hans-Helge Albrecht, IEEE International Conference on Acoustics, Speech,
and Signal Processing, 2001. Proceedings. (ICASSP '01)

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_wnd_CosineSum_HEADER_INCLUDED)
#define mfx_dsp_wnd_CosineSum_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/wnd/WndInterface.h"



namespace mfx
{
namespace dsp
{
namespace wnd
{



template <class T>
class CosineSum
:	public WndInterface <T>
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static constexpr int _min_order =  1;
	static constexpr int _max_order = 10;

	void           set_order (int order) noexcept;

	/*** To do: information retrieval ***/



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// WndInterface
	void           do_make_win (T data_ptr [], int len) override;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	struct Info
	{
		int            _nbr_terms;
		double         _highest_sidelobe_level;   // dB
		double         _coherent_gain;            // dB
		double         _scallop_loss;             // dB
		double         _enbw;                     // Bins, Equivalent-Noise BandWidth
		double         _bw_3db;                   // Bins
		double         _bw_6db;                   // Bins
		double         _coef_arr [_max_order + 1];
	};

	int            _order = 4;

	static const Info
						_info_arr [_max_order - _min_order + 1];



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const CosineSum <T> &other) const = delete;
	bool           operator != (const CosineSum <T> &other) const = delete;

}; // class CosineSum



}  // namespace wnd
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/wnd/CosineSum.hpp"



#endif   // mfx_dsp_wnd_CosineSum_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

