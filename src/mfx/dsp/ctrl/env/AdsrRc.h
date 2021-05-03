/*****************************************************************************

        AdsrRc.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ctrl_env_AdsrRc_HEADER_INCLUDED)
#define mfx_dsp_ctrl_env_AdsrRc_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/ctrl/env/AdsrSeg.h"
#include "mfx/dsp/ctrl/env/SegmentRc.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace ctrl
{
namespace env
{



class AdsrRc
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const float   _infinite_sus;   // Seconds.

	               AdsrRc () noexcept;
	               AdsrRc (const AdsrRc &other)     = default;
	               AdsrRc (AdsrRc &&other)          = default;
	virtual        ~AdsrRc ()                       = default;

	AdsrRc &       operator = (const AdsrRc &other) = default;
	AdsrRc &       operator = (AdsrRc &&other)      = default;

	void           set_sample_freq (float fs) noexcept;
	void           set_atk_time (float at) noexcept;
	void           set_atk_virt_lvl (float atk_lvl) noexcept;
	void           set_dcy_time (float dt) noexcept;
	void           set_sus_lvl (float sl) noexcept;
	void           set_sus_time (float st) noexcept;
	void           set_rls_time (float rt) noexcept;
	inline AdsrSeg get_cur_seg () const noexcept;
	void           note_on () noexcept;
	void           note_off () noexcept;

	void           clear_buffers () noexcept;
	void           kill_ramps () noexcept;

	inline float   get_val () const noexcept;
	float          process_sample () noexcept;
	void           process_block (float data [], int nbr_spl) noexcept;
	void           skip_block (int nbr_spl) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	inline void    update_atk_seg () noexcept;
	inline void    update_dcy_seg () noexcept;
	inline void    check_and_set_next_seg () noexcept;

	float          _sample_freq  = 0;  // > 0
	float          _atk_time     = 0.005f;     // > 0
	float          _atk_virt_lvl = 1.5f; // > 1
	float          _dcy_time     = 0.050f;     // > 0
	float          _sus_time     = _infinite_sus;     // > 0
	float          _sus_lvl      = 0.75f;
	float          _rls_time     = 0.100f;     // > 0

	float          _cur_val      = 0;
	AdsrSeg        _seg          = AdsrSeg_NONE;
	SegmentRc *    _cur_seg_ptr  = nullptr;  // 0 = none
	SegmentRc      _seg_atk;
	SegmentRc      _seg_dcy;
	SegmentRc      _seg_sus;
	SegmentRc      _seg_rls;

	std::array <SegmentRc *, AdsrSeg_NBR_ELT>
	               _seg_ptr_arr {
	               	nullptr, &_seg_atk, &_seg_dcy, &_seg_sus, &_seg_rls
	               };

	static const std::array <AdsrSeg, AdsrSeg_NBR_ELT>
	               _next_seg;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const AdsrRc &other) const = delete;
	bool           operator != (const AdsrRc &other) const = delete;

}; // class AdsrRc



}  // namespace env
}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/ctrl/env/AdsrRc.hpp"



#endif   // mfx_dsp_ctrl_env_AdsrRc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
