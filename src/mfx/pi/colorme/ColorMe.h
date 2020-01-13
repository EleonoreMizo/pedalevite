/*****************************************************************************

        ColorMe.h
        Author: Laurent de Soras, 2017

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_colorme_ColorMe_HEADER_INCLUDED)
#define mfx_pi_colorme_ColorMe_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/AllocAlign.h"
#include "fstb/DataAlign.h"
#include "mfx/dsp/iir/SvfCore4Simd.h"
#include "mfx/dsp/iir/SvfMixerPeak.h"
#include "mfx/pi/colorme/ColorMeDesc.h"
#include "mfx/pi/colorme/Cst.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace colorme
{



class ColorMe final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               ColorMe ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	class Channel
	{
	public:
		dsp::iir::SvfCore4Simd <
			fstb::DataAlign <true>,
			fstb::DataAlign <true>,
			fstb::DataAlign <true>,
			dsp::iir::SvfMixerPeak
		>              _formant_filter;
	};
	typedef std::vector <Channel, fstb::AllocAlign <Channel, 16> > ChannelArray;

	class VowelDesc
	{
	public:
		int            _type = 0;
		int            _set  = 0;
	};
	typedef std::array <VowelDesc, Cst::_nbr_vow_morph> VowDescList;

	typedef std::array <float    , Cst::_nbr_formants> Vowel; // Formant frequency, Hz
	typedef std::array <Vowel    , Cst::_nbr_vowels>   VoiceTone;
	typedef std::array <VoiceTone, Cst::_nbr_vow_sets> VoiceToneArray;

	void           clear_buffers ();
	void           update_param (bool force_flag = false);
	void           update_formants ();

	State          _state;

	ColorMeDesc    _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_vow;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_vow_misc;
	fstb::util::NotificationFlagCascadeSingle
	               _param_change_flag_vow_type;

	ChannelArray   _chn_arr;

	float          _vow_morph;
	float          _vow_reso;
	float          _vow_q;
	float          _vow_transp;         // Multiplier on frequencies
	int            _vow_nbr_vow;
	int            _vow_nbr_formants;
	VowDescList    _vow_desc_arr;
	float          _vow_vol_fix;

	static const VoiceToneArray
	               _voice_tone_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ColorMe (const ColorMe &other)           = delete;
	ColorMe &      operator = (const ColorMe &other)        = delete;
	bool           operator == (const ColorMe &other) const = delete;
	bool           operator != (const ColorMe &other) const = delete;

}; // class ColorMe



}  // namespace colorme
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/colorme/ColorMe.hpp"



#endif   // mfx_pi_colorme_ColorMe_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
