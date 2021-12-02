/*****************************************************************************

        Lipidipi.h
        Author: Laurent de Soras, 2021

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_lipid_Lipidipi_HEADER_INCLUDED)
#define mfx_pi_lipid_Lipidipi_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/AllocAlign.h"
#include "fstb/DataAlign.h"
#include "fstb/def.h"
#include "fstb/fnc.h"
#include "fstb/SingleObj.h"
#include "fstb/Vf32.h"
#include "mfx/dsp/osc/OscSinCosStable4Simd.h"
#include "mfx/pi/cdsp/PhaseHalfPi.h"
#include "mfx/pi/lipid/Cst.h"
#include "mfx/pi/lipid/LipidipiDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace lipid
{



class Lipidipi final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       Lipidipi (piapi::HostInterface &host);
	               ~Lipidipi () = default;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static constexpr int _nbr_osc_pair_per_vec = fstb::Vf32::_length / 2;
	static constexpr int _max_nbr_vec =
		fstb::div_ceil (Cst::_max_image_pairs * 2, fstb::Vf32::_length);
	static constexpr int _osc_arr_len = _max_nbr_vec * fstb::Vf32::_length;

	static constexpr int _nbr_coef    = 8; // For the pi/2 phaser

	typedef std::vector <
		float, fstb::AllocAlign <float, fstb_SIMD128_ALIGN>
	> BufAlign;

	class Channel
	{
	public:
		cdsp::PhaseHalfPi <_nbr_coef>
		               _ssb;
	};

	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	typedef std::array <float, _osc_arr_len> OscArrBuf;

	class Aligned
	{
	public:
		ChannelArray   _chn_arr;

		// Oscillator are grouped by pairs, one for the negative frequency
		// shift, the other one for the equivalent positive shift.
		alignas (fstb_SIMD128_ALIGN) std::array <
			dsp::osc::OscSinCosStable4Simd <fstb::DataAlign <true> >,
			_max_nbr_vec
		>              _osc_arr;
	};

	void           clear_buffers ();
	void           update_param (bool force_flag = false);

	piapi::HostInterface &
	               _host;
	State          _state = State_CREATED;

	LipidipiDesc   _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc { _state_set };
	float          _sample_freq = 0;    // Hz, > 0. <= 0: not initialized
	float          _inv_fs      = 0;    // 1 / _sample_freq

	fstb::util::NotificationFlag
	               _param_change_flag;

	fstb::SingleObj <Aligned, fstb::AllocAlign <Aligned, fstb_SIMD128_ALIGN> >
	               _ali;
	int            _nbr_osc     = 0;
	int            _nbr_osc_vec = 0;

	std::array <double, _nbr_coef>
	               _coef_list;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Lipidipi ()                               = delete;
	               Lipidipi (const Lipidipi &other)          = delete;
	               Lipidipi (Lipidipi &&other)               = delete;
	Lipidipi &     operator = (const Lipidipi &other)        = delete;
	Lipidipi &     operator = (Lipidipi &&other)             = delete;
	bool           operator == (const Lipidipi &other) const = delete;
	bool           operator != (const Lipidipi &other) const = delete;

}; // class Lipidipi



}  // namespace lipid
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/lipid/Lipidipi.hpp"



#endif   // mfx_pi_lipid_Lipidipi_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
