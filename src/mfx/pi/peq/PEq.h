/*****************************************************************************

        PEq.h
        Author: Laurent de Soras, 2016

Template parameters:

- NB: number of bands, > 0

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_peq_PEq_HEADER_INCLUDED)
#define mfx_pi_peq_PEq_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/util/NotificationFlagCascadeSingle.h"
#include "fstb/DataAlign.h"
#include "mfx/dsp/iir/BiquadPackSimd.h"
#include "mfx/pi/peq/BandParam.h"
#include "mfx/pi/peq/PEqDesc.h"
#include "mfx/pi/peq/PEqType.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>



namespace mfx
{
namespace pi
{
namespace peq
{



template <int NB>
class PEq
:	public piapi::PluginInterface
{

	static_assert ((NB > 0), "NB must be strictly positive");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef PEqDesc <NB> DescType;

	static const int  _nbr_bands    = DescType::_nbr_bands;
	static const int  _update_resol = 64; // Samples, multiple of 4

	explicit       PEq (piapi::HostInterface &host);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef dsp::iir::BiquadPackSimd <
		fstb::DataAlign <true>,
		fstb::DataAlign <true>
	>              BiqPack;

	class BandInfo
	{
	public:
		bool           is_active () const;
		bool           is_bypass () const;

		BandParam      _param;                    // Cached parameters

		int            _neutral_duration =  0;    // Time elapsed (samples) since a band is considered as neutral. If == _neutral_time, band is removed from the pack.
		int            _stage_index      = -1;    // Stage index in the biquad pack. -1: deactivated.
		fstb::util::NotificationFlagCascadeSingle
		               _change_flag;
	};

	typedef std::array <BandInfo, _nbr_bands> BandInfoArray;
	typedef std::array <int     , _nbr_bands> StageInfoArr;

	void           clear_buffers ();
	void           update_param (bool force_flag);

	int            find_empty_stage () const;
	int            count_nbr_stages () const;
	void           neutralise_stage_immediate (int stage_index);
	void           clear_buffers_stage (int stage_index);

	inline bool    is_band_active (int band) const;
	void           activate_band (int band);
	void           deactivate_band (int band);

	void           collect_parameters (int band);
	void           update_filter_eq (int band);
	void           cook_all_bands ();
	void           cook_band (int band);

	static bool    is_unit_gain (float gain);
	static bool    is_ramping_ok (float a1d, float a2d, float a1s, float a2s);
	static constexpr float
	               compute_pole_delta (float a1, float a2);

	piapi::HostInterface &
	               _host;
	State          _state;

	DescType       _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;
	float          _sample_freq;        // Hz, > 0. <= 0: not initialized
	float          _inv_fs;             // 1 / _sample_freq
	int            _nbr_chn;

	BiqPack        _biq_pack;
	BandInfoArray  _band_info_arr;
	StageInfoArr   _stage_to_band_arr;  // -1 indicates that the stage is not attributed to a band.
	fstb::util::NotificationFlag
	               _param_change_flag;
	int            _neutral_time;       // Should be greater than the block length
	bool           _ramp_flag;          // Next parameter changes should ramp (default)



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PEq ()                                    = delete;
	               PEq (const PEq <NB> &other)               = delete;
	               PEq (PEq <NB> &&other)                    = delete;
	PEq <NB> &     operator = (const PEq <NB> &other)        = delete;
	PEq <NB> &     operator = (PEq <NB> &&other)             = delete;
	bool           operator == (const PEq <NB> &other) const = delete;
	bool           operator != (const PEq <NB> &other) const = delete;

}; // class PEq



}  // namespace peq
}  // namespace pi
}  // namespace mfx



#include "mfx/pi/peq/PEq.hpp"



#endif   // mfx_pi_peq_PEq_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
