/*****************************************************************************

        DistApf.h
        Author: Laurent de Soras, 2019

Effect based on this idea:
Jussi Pekonen,
Coefficient-modulated first-order allpass filter as distortion effect,
Proceedings of the 11th International Conference on Digital Audio Effects
(DAFx-08), 2008

TO DO:
- Oversampling (x2 or x4?) to avoid aliasing
- Option to side-chain the modulation signal

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_distapf_DistApf_HEADER_INCLUDED)
#define mfx_pi_distapf_DistApf_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/util/NotificationFlag.h"
#include "fstb/AllocAlign.h"
#include "mfx/pi/distapf/DistApfDesc.h"
#include "mfx/pi/ParamStateSet.h"
#include "mfx/piapi/PluginInterface.h"

#include <array>
#include <vector>



namespace mfx
{
namespace pi
{
namespace distapf
{



class DistApf
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               DistApf ();
	virtual        ~DistApf () = default;



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

	static const int  _freq_min =   500;   // Minimum frequency, Hz
	static const int  _freq_max = 21550;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	class Channel
	{
	public:
		float          _srl_state = 0;
		float          _apf_mem_x = 0;
		float          _apf_mem_y = 0;
	};

	typedef std::array <Channel, _max_nbr_chn> ChannelArray;

	void           update_param (bool force_flag = false);
	void           clear_buffers ();

	State          _state;

	DistApfDesc    _desc;
	ParamStateSet  _state_set;

	fstb::util::NotificationFlag
	               _param_change_flag;

	float          _sample_freq;
	float          _inv_fs;
	ChannelArray   _chn_arr;
	BufAlign       _buf_tmp;

	float          _gain;
	float          _map_a;
	float          _map_b;
	float          _f_min;
	float          _f_max;
	float          _freq_scale;
	float          _srl;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               DistApf (const DistApf &other)           = delete;
	DistApf &      operator = (const DistApf &other)        = delete;
	bool           operator == (const DistApf &other) const = delete;
	bool           operator != (const DistApf &other) const = delete;

}; // class DistApf



}  // namespace distapf
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/distapf/DistApf.hpp"



#endif   // mfx_pi_distapf_DistApf_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
