/*****************************************************************************

        BandSplitAllPassPair.h
        Author: Laurent de Soras, 2016

Template parameters:

- AP0, AP1: a pair of all-pass filters. Requirements on AP0 and AP1 :
	AP0::AP0 ();
	AP0::~AP0 ();
	void AP0::process_block (float *, const float *, int) noexcept;
	float AP0::process_sample (float) noexcept;
	void AP0::clear_buffers () noexcept;
	It is recommended to set the lightest filter to AP0, because it is used
	for phase compensation too.

- VFF: volume fix flag. When set to false, the split result has a x2 gain
	(and a gain).

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_BandSplitAllPassPair_HEADER_INCLUDED)
#define mfx_dsp_BandSplitAllPassPair_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{



template <typename AP0, typename AP1, bool VFF = true>
class BandSplitAllPassPair
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef void (* AddSubProto) (float out_1_ptr [], float out_2_ptr [], int nbr_spl) noexcept;
	typedef void (* VolumeProto) (float out_ptr [], const float in_ptr [], int nbr_spl, float vol) noexcept;

	typedef AP0 ApFilter0;
	typedef AP1 ApFilter1;

	               BandSplitAllPassPair ();

	ApFilter0 &    use_filter_0 () noexcept;
	const ApFilter0 &
	               use_filter_0 () const noexcept;

	ApFilter1 &    use_filter_1 () noexcept;
	const ApFilter1 &
	               use_filter_1 () const noexcept;

	ApFilter0 &    use_fix_filter () noexcept;
	const ApFilter0 &
	               use_fix_filter () const noexcept;

	void           set_add_sub_function (AddSubProto fnc_ptr) noexcept;
	void           set_vol_function (VolumeProto vol_ptr) noexcept;

	inline void    split_sample (float &out_1, float &out_2, float sample) noexcept;
	void           split_block (float out_1_ptr [], float out_2_ptr [], const float in_ptr [], int nbr_spl) noexcept;

	inline float   compensate_sample (float sample) noexcept;
	void           compensate_block (float out_ptr [], const float in_ptr [], int nbr_spl) noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	ApFilter0      _filter_0;
	ApFilter1      _filter_1;
	ApFilter0      _filter_fix;
	AddSubProto    _add_sub_ptr;
	VolumeProto    _vol_ptr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               BandSplitAllPassPair (const BandSplitAllPassPair &other)  = delete;
	               BandSplitAllPassPair (const BandSplitAllPassPair &&other) = delete;
	BandSplitAllPassPair &
	               operator = (const BandSplitAllPassPair &other)        = delete;
	BandSplitAllPassPair &
	               operator = (const BandSplitAllPassPair &&other)       = delete;
	bool           operator == (const BandSplitAllPassPair &other) const = delete;
	bool           operator != (const BandSplitAllPassPair &other) const = delete;

}; // class BandSplitAllPassPair



}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/BandSplitAllPassPair.hpp"



#endif   // mfx_dsp_BandSplitAllPassPair_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
