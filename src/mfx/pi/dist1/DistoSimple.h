/*****************************************************************************

        DistoSimple.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_dist1_DistoSimple_HEADER_INCLUDED)
#define mfx_pi_dist1_DistoSimple_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "fstb/util/NotificationFlag.h"
#include "fstb/AllocAlign.h"
#include "fstb/SingleObj.h"
#include "mfx/dsp/iir/Biquad.h"
#include "mfx/dsp/iir/OnePole.h"
#include "mfx/dsp/iir/Downsampler4xSimd.h"
#include "mfx/dsp/iir/Upsampler4xSimd.h"
#include "mfx/pi/dist1/DistoSimpleDesc.h"
#include "mfx/pi/ParamProcSimple.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace dist1
{


class DistoSimple final
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       DistoSimple (piapi::HostInterface &host);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	State          do_get_state () const final;
	double         do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const final;
	int            do_reset (double sample_freq, int max_buf_len, int &latency) final;
	void           do_process_block (piapi::ProcInfo &proc) final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int     _ovrspl      = 4;
	static const int     _nbr_coef_42 = 3;
	static const int     _nbr_coef_21 = 8;

	// Attenuation. 1/_attn is the maximum volume that the disortion can reach,
	// while gain at 0 remains unchanged.
	static constexpr float   _attn = 8;
	static constexpr float   _m_9  = fstb::ipowpc <9 - 1> (_attn) / 9;
	static constexpr float   _m_2  = fstb::ipowpc <2 - 1> (_attn) / 2;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	class Channel
	{
	public:
		typedef dsp::iir::Upsampler4xSimd <_nbr_coef_42, _nbr_coef_21> UpSpl;
		typedef dsp::iir::Downsampler4xSimd <_nbr_coef_42, _nbr_coef_21> DwSpl;

		dsp::iir::OnePole
		               _hpf_in;
		dsp::iir::Biquad
		               _env_lpf;
		fstb::SingleObj <UpSpl, fstb::AllocAlign <UpSpl, 16> >
		               _us;
		fstb::SingleObj <DwSpl, fstb::AllocAlign <DwSpl, 16> >
		               _ds;
		BufAlign       _buf;
		BufAlign       _buf_env;
	};

	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	void           clear_buffers ();
	void           update_filter_in ();
	void           distort_block (float dst_ptr [], const float src_ptr [], int nbr_spl);

	piapi::HostInterface &
	               _host;
	State          _state;

	DistoSimpleDesc
	               _desc;
	ParamStateSet  _state_set;
	ParamProcSimple
	               _param_proc;

	fstb::util::NotificationFlag
	               _param_change_flag;

	float          _sample_freq;
	float          _inv_fs;
	float          _gain;
	float          _hpf_in_freq;
	float          _bias;
	BufAlign       _buf_ovrspl;
	ChannelArray   _chn_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DistoSimple ()                               = delete;
	               DistoSimple (const DistoSimple &other)       = delete;
	               DistoSimple (DistoSimple &&other)            = delete;
	DistoSimple &  operator = (const DistoSimple &other)        = delete;
	DistoSimple &  operator = (DistoSimple &&other)             = delete;
	bool           operator == (const DistoSimple &other) const = delete;
	bool           operator != (const DistoSimple &other) const = delete;

}; // class DistoSimple



}  // namespace dist1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/dist1/DistoSimple.hpp"



#endif   // mfx_pi_dist1_DistoSimple_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
