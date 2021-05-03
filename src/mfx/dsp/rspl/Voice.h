/*****************************************************************************

        Voice.h
        Author: Laurent de Soras, 2016

About loop span and directions:

Here N is the loop length.

                  0     1            N-2   N-1
Sample       -----*-----*--- . . . ---*-----*-----

Normal loop,      *------------------------------>
forward:          *------------------------------>

Normal loop, <------------------------------*
backward:    <------------------------------*

Ping-pong         *------------------------------>
loop:        <------------------------------*

When the playback direction is changed, the engine automatically ensure that
the cursor is in the right range.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_rspl_Voice_HEADER_INCLUDED)
#define	mfx_dsp_rspl_Voice_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"fstb/FixedPoint.h"
#include	"mfx/dsp/rspl/Cst.h"

#include <array>



namespace mfx
{
namespace dsp
{

class SplDataRetrievalInterface;

namespace rspl
{



class InterpolatorInterface;

class Voice
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum LoopMode
	{
		LoopMode_NONE,
		LoopMode_NORMAL,
		LoopMode_PING_PONG,

		LoopMode_NBR_ELT
	};

	               Voice ()                        = default;
	               Voice (const Voice &other)      = default;
	               Voice (Voice &&other)           = default;
	virtual        ~Voice ()                       = default;
	Voice &        operator = (const Voice &other) = default;
	Voice &        operator = (Voice &&other)      = default;

	void           set_interpolator (InterpolatorInterface &interp) noexcept;
	const InterpolatorInterface &
	               use_interpolator () const noexcept;
	void           set_tmp_buf (float *buf_ptr, int len) noexcept;

	void           set_sample_freq (double sample_freq, int ovrspl_l2);
	void           set_fade_duration (double fade_time) noexcept;

	bool           is_active () const noexcept;
	void           activate (SplDataRetrievalInterface &data_provider, int64_t len, int nbr_chn, double sample_freq) noexcept;
	void           deactivate () noexcept;

	void           start (bool fade_flag) noexcept;
	void           stop (bool fade_flag) noexcept;

	void           set_rate (double rate, double rate_step) noexcept;
	void           set_direction (bool backward_flag) noexcept;
	void           set_loop_info (LoopMode loop_mode, int64_t pos, int64_t len) noexcept;
	void           set_playback_pos (int64_t pos_int, uint32_t pos_frac) noexcept;

	void           get_playback_pos (int64_t &pos_int, uint32_t &pos_frac) const noexcept;
	bool           is_running_backward () const noexcept;

	void           process_block (float *out_ptr_arr [], int nbr_spl) noexcept;

	static void    conv_pos_flt_to_fix (int64_t &pos_int, uint32_t &pos_frac, double pos) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef	std::array <float *, Cst::MAX_NBR_CHN>	DataPtrArray;
	typedef	std::array <int64_t, 2>	LoopPosArray;

	inline bool    is_playing () const noexcept;
	inline void    fade_to (float val) noexcept;
	void           update_tmp_buf_info () noexcept;
	void           linearise_sample (int nbr_spl) noexcept;
	bool           compute_next_stop (int64_t &stop_pos, int64_t cur_pos, int dir) const noexcept;
	int            collect_source_spl (int offset, int max_len, int cur_dir, int64_t pos, int64_t stop_pos) noexcept;
	void           advance_cursor_pos (int64_t &pos, int &dir, int stride) const noexcept;
	void           check_finished (float *out_ptr_arr [], int &dest_pos, int nbr_spl) noexcept;
	void           compensate_pos_for_group_delay (int64_t &pos_int, uint32_t &pos_frac, int dir) const noexcept;

	inline static int
	               compute_dir_fix (int dir) noexcept;

	// 0: interpolator not set.
	InterpolatorInterface *	
	               _interp_ptr = nullptr;

	// Impulse length, samples. > 0
	int            _imp_len = 1;

	// Group delay, samples. [0 ; _imp_len - 1]
	fstb::FixedPoint
	               _group_dly { 0, 0 };

	// 0 indicates that the voice is not active
	SplDataRetrievalInterface *
	               _data_provider_ptr = nullptr;

	// Base-2 logarithm of the oversampling. >= 0.
	int            _ovrspl_l2 = 0;

	// Output (possibly oversampled) sample frequency, Hz, > 0.
	double         _sample_freq = 44100;

	// Fade time, seconds, >= 0.
	double         _fade_time = 0.005;

	// 0: not set.
	float *        _tmp_buf_ptr = nullptr;

	// > 0
	int            _tmp_buf_len = 0;

	// Number of source samples. Valid only in active mode
	int            _room_for_lin_src = -1;

	// Input sample frequency, Hz, > 0
	double         _in_fs = 44100;

	// Sample length, sample frames, > 0
	int64_t        _in_len = 0;

	// Number of channels [1 ; Constants::MAX_NBR_CHN]
	int            _nbr_chn = 1;

	// Resampling rate
	fstb::FixedPoint
	               _rate { 1, 0 };

	// Resampling rate increment (per output sample)
	fstb::FixedPoint
	               _rate_step { 0, 0 };

	LoopMode       _loop_mode = LoopMode_NONE;

	// [Start | End]
	LoopPosArray   _loop_pos_arr {};

	// Current position in the file, integer part
	int64_t        _pos_int = 0;

	// Current position in the file, fractionnal part. On backward playback, it represents a negative fractionnal offset. Therefore the complete position is _pos_int - _pos_frac / 2^32.
	uint32_t       _pos_frac = 0;

	// +1 or -1 depending on the playback direction
	int            _pbk_dir  = 1;

	// Reflects start
	bool           _started_flag = false;

	// Reflects end
	bool           _stopped_flag = false;

	// 0 if no fade currently
	int            _fade_rem_spl = 0;

	// Current fade value
	float          _fade_val = 0;

	// Fade step (per output sample)
	float          _fade_step = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Voice &other) = delete;
	bool           operator != (const Voice &other) = delete;

};	// class Voice



}	// namespace rspl
}	// namespace dsp
}	// namespace mfx



//#include "mfx/dsp/rspl/Voice.hpp"



#endif	// mfx_dsp_rspl_Voice_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
