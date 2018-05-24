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

	               Voice ();
	virtual        ~Voice () {}

	void           set_interpolator (InterpolatorInterface &interp);
	const InterpolatorInterface &
	               use_interpolator () const;
	void           set_tmp_buf (float *buf_ptr, int len);

	void           set_sample_freq (double sample_freq, int ovrspl_l2);
	void           set_fade_duration (double fade_time);

	bool           is_active () const;
	void           activate (SplDataRetrievalInterface &data_provider, int64_t len, int nbr_chn, double sample_freq);
	void           deactivate ();

	void           start (bool fade_flag);
	void           stop (bool fade_flag);

	void           set_rate (double rate, double rate_step);
	void           set_direction (bool backward_flag);
	void           set_loop_info (LoopMode loop_mode, int64_t pos, int64_t len);
	void           set_playback_pos (int64_t pos_int, uint32_t pos_frac);

	void           get_playback_pos (int64_t &pos_int, uint32_t &pos_frac) const;
	bool           is_running_backward () const;

	void           process_block (float *out_ptr_arr [], int nbr_spl);

	static void    conv_pos_flt_to_fix (int64_t &pos_int, uint32_t &pos_frac, double pos);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef	std::array <float *, Cst::MAX_NBR_CHN>	DataPtrArray;
	typedef	std::array <int64_t, 2>	LoopPosArray;

	inline bool    is_playing () const;
	inline void    fade_to (float val);
	void           update_tmp_buf_info ();
	bool           compute_next_stop (int64_t &stop_pos, int64_t cur_pos, int dir) const;
	void           linearise_sample (int nbr_spl);
	int            collect_source_spl (int offset, int max_len, int cur_dir, int64_t pos, int64_t stop_pos);
	void           advance_cursor_pos (int64_t &pos, int &dir, int stride) const;
	void           check_finished (float *out_ptr_arr [], int &dest_pos, int nbr_spl);
	void           compensate_pos_for_group_delay (int64_t &pos_int, uint32_t &pos_frac, int dir) const;

	inline static int
	               compute_dir_fix (int dir);

	InterpolatorInterface *	      // 0: interpolator not set.
	               _interp_ptr;
	int            _imp_len;      // Impulse length, samples. > 0
	fstb::FixedPoint              // Group delay, samples. [0 ; _imp_len - 1]
						_group_dly;

	SplDataRetrievalInterface *   // 0 indicates that the voice is not active
	               _data_provider_ptr;
	int            _ovrspl_l2;    // Base-2 logarithm of the oversampling. >= 0.
	double         _sample_freq;  // Output (possibly oversampled) sample frequency, Hz, > 0.
	double         _fade_time;    // Fade time, seconds, >= 0.

	float *        _tmp_buf_ptr;  // 0: not set.
	int            _tmp_buf_len;  // > 0
	int            _room_for_lin_src;   // Number of source samples. Valid only in active mode

	double         _in_fs;        // Input sample frequency, Hz, > 0
	int64_t        _in_len;       // Sample length, sample frames, > 0
	int            _nbr_chn;      // Number of channels [1 ; Constants::MAX_NBR_CHN]

	fstb::FixedPoint              // Resampling rate
	               _rate;
	fstb::FixedPoint              // Resampling rate increment (per output sample)
	               _rate_step;

	LoopMode       _loop_mode;
	LoopPosArray   _loop_pos_arr; // [Start | End]

	int64_t        _pos_int;      // Current position in the file, integer part
	uint32_t       _pos_frac;     // Current position in the file, fractionnal part. On backward playback, it represents a negative fractionnal offset. Therefore the complete position is _pos_int - _pos_frac / 2^32.
	int            _pbk_dir;      // +1 or -1 depending on the playback direction
	bool           _started_flag; // Reflects start
	bool           _stopped_flag; // Reflects end

	int            _fade_rem_spl; // 0 if no fade currently
	float          _fade_val;     // Current fade value
	float          _fade_step;    // Fade step (per output sample)



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Voice (const Voice &other);
	Voice &        operator = (const Voice &other);
	bool           operator == (const Voice &other);
	bool           operator != (const Voice &other);

};	// class Voice



}	// namespace rspl
}	// namespace dsp
}	// namespace mfx



//#include "mfx/dsp/rspl/Voice.hpp"



#endif	// mfx_dsp_rspl_Voice_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
