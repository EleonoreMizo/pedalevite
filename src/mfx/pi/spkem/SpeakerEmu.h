/*****************************************************************************

        SpeakerEmu.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_spkem_SpeakerEmu_HEADER_INCLUDED)
#define mfx_pi_spkem_SpeakerEmu_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/AllocAlign.h"
#include "fstb/DataAlign.h"
#include "mfx/dsp/dly/DelayLineData.h"
#include "mfx/dsp/iir/BiquadPackSimd.h"
#include "mfx/pi/spkem/SpeakerEmuDesc.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace spkem
{



class SpeakerEmu
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               SpeakerEmu ();
	virtual        ~SpeakerEmu () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	virtual State  do_get_state () const;
	virtual double do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const;
	virtual int    do_reset (double sample_freq, int max_buf_len, int &latency);
	virtual void   do_clean_quick ();
	virtual void   do_process_block (ProcInfo &proc);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _max_nbr_stages = 16;
	static const int  _nbr_types      =  3;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	typedef dsp::iir::BiquadPackSimd <
		fstb::DataAlign <true>,
		fstb::DataAlign <true>
	>              BiqPack;

	class Stage
	{
	public:
		std::array <float, 3>
		               _bs;
		std::array <float, 3>
		               _as;
		float          _freq;
	};
	typedef std::array <Stage, _max_nbr_stages> StageArray;
	typedef std::array <StageArray, _nbr_types> ConfigArray;

	class Channel
	{
	public:
		dsp::dly::DelayLineData <float>
		               _delay;
	};
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);
	void           update_filter ();

	void           set_peak (int conf, int stage, float freq, float lvl, float q);
	void           set_shelf_l (int conf, int stage, float freq, float lvl, float q);
	void           set_shelf_h (int conf, int stage, float freq, float lvl, float q);
	void           set_pass_l (int conf, int stage, float freq, float lvl, float q);
	void           set_pass_h (int conf, int stage, float freq, float lvl, float q);
	void           add_gain (int conf, float gain);

	State          _state;

	SpeakerEmuDesc _desc;
	ParamStateSet  _state_set;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq
	int            _nbr_chn;            // 0 = requires update
	uint32_t       _channels;           // Bitmask

	fstb::util::NotificationFlag
	               _param_change_flag;

	BiqPack        _biq_pack;
	ConfigArray    _config_arr;
	std::array <int, _nbr_types>
	               _nbr_stages_arr;
	ChannelArray   _chn_arr;
	int            _comb_time_spl;      // Delay time for the comb filter, in samples. > 0.
	int            _write_pos;          // Common for the delay lines of all channels.

	int            _config;
	float          _mid_freq;
	float          _mid_lvl;
	float          _treble_freq;
	float          _treble_lvl;
	float          _comb_fdbk;

	BufAlign       _buf_tmp;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               SpeakerEmu (const SpeakerEmu &other)        = delete;
	SpeakerEmu &   operator = (const SpeakerEmu &other)        = delete;
	bool           operator == (const SpeakerEmu &other) const = delete;
	bool           operator != (const SpeakerEmu &other) const = delete;

}; // class SpeakerEmu



}  // namespace spkem
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/spkem/SpeakerEmu.hpp"



#endif   // mfx_pi_spkem_SpeakerEmu_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
