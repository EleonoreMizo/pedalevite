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

#include "fstb/def.h"

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "mfx/dsp/iir/SvfMixerPeak.h"
#include "mfx/pi/colorme/ColorMeDesc.h"
#include "mfx/pi/colorme/Cst.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/piapi/PluginInterface.h"

#if defined (fstb_HAS_SIMD)
	#include "fstb/AllocAlign.h"
	#include "fstb/DataAlign.h"
	#include "mfx/dsp/iir/SvfCore4Simd.h"

	#include <vector>

#else
	#include "mfx/dsp/iir/SvfCore.h"

#endif

#include <array>



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

	explicit       ColorMe (piapi::HostInterface &host);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class Channel
	{
	public:
#if defined (fstb_HAS_SIMD)
		typedef dsp::iir::SvfCore4Simd <
			fstb::DataAlign <true>,
			fstb::DataAlign <true>,
			fstb::DataAlign <true>,
			dsp::iir::SvfMixerPeak
		> FormantFilter;
#else
		class FormantFilter
		{
		public:
			void           neutralise ();
			void           set_coefs_one (int unit, float g0, float g1, float g2);
			void           set_mix_one (int unit, float v0m, float v1m, float v2m);
			void           process_block_ser_imm (float dst_ptr [], const float src_ptr [], int nbr_spl);
			void           clear_buffers ();
		private:
			typedef std::array <
				dsp::iir::SvfCore <dsp::iir::SvfMixerPeak>,
				Cst::_nbr_formants
			> SvfArray;
			SvfArray       _biq_arr;
		};
#endif
		FormantFilter  _formant_filter;
	};
#if defined (fstb_HAS_SIMD)
	typedef std::vector <Channel, fstb::AllocAlign <Channel, 16> > ChannelArray;
#else
	typedef std::array <Channel, _max_nbr_chn> ChannelArray;
#endif

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

	piapi::HostInterface &
	               _host;
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

	               ColorMe ()                               = delete;
	               ColorMe (const ColorMe &other)           = delete;
	               ColorMe (ColorMe &&other)                = delete;
	ColorMe &      operator = (const ColorMe &other)        = delete;
	ColorMe &      operator = (ColorMe &&other)             = delete;
	bool           operator == (const ColorMe &other) const = delete;
	bool           operator != (const ColorMe &other) const = delete;

}; // class ColorMe



}  // namespace colorme
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/colorme/ColorMe.hpp"



#endif   // mfx_pi_colorme_ColorMe_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
