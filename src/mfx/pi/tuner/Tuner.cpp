/*****************************************************************************

        Tuner.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/mix/Align.h"
#include "mfx/pi/tuner/Tuner.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace tuner
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



float	Tuner::get_freq () const
{
	return _freq;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



std::string	Tuner::do_get_unique_id () const
{
	return _desc.get_unique_id ();
}



std::string	Tuner::do_get_name () const
{
	return _desc.get_name ();
}



void	Tuner::do_get_nbr_io (int &nbr_i, int &nbr_o) const
{
	_desc.get_nbr_io (nbr_i, nbr_o);
}



bool	Tuner::do_prefer_stereo () const
{
	return _desc.prefer_stereo ();
}



int	Tuner::do_get_nbr_param (piapi::ParamCateg categ) const
{
	return _desc.get_nbr_param (categ);
}



const piapi::ParamDescInterface &	Tuner::do_get_param_info (piapi::ParamCateg categ, int index) const
{
	return _desc.get_param_info (categ, index);
}



piapi::PluginInterface::State	Tuner::do_get_state () const
{
	return _state;
}



double	Tuner::do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const
{
	return 0;
}



int	Tuner::do_reset (double sample_freq, int max_buf_len, int &latency)
{
	int            ret_val = Err_OK;

	latency = 0;

	if (max_buf_len % _sub_spl == 0)
	{
		_buffer.resize (max_buf_len / _sub_spl);
		_analyser.set_sample_freq (sample_freq / _sub_spl);
		_state = State_ACTIVE;
	}
	else
	{
		assert (false);
		ret_val = Err_VALUE_NOT_SUPPORTED;
	}

	return ret_val;
}



void	Tuner::do_process_block (ProcInfo &proc)
{
	const int      nbr_spl_s = proc._nbr_spl / _sub_spl;
	const float *  src_ptr   = proc._src_arr [0];
	for (int k = 0; k < nbr_spl_s; ++k)
	{
		static_assert (_sub_spl == 4, "subsampling rate");
		_buffer [k] =
			  src_ptr [k * 4    ]
			+ src_ptr [k * 4 + 1]
			+ src_ptr [k * 4 + 2]
			+ src_ptr [k * 4 + 3];
	}
	_freq = _analyser.process_block (&_buffer [0], nbr_spl_s);

	for (int chn = 0; chn < proc._nbr_chn_arr [Dir_OUT]; ++chn)
	{
		dsp::mix::Align::clear (&proc._dst_arr [chn] [0], proc._nbr_spl);
	}
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace tuner
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
