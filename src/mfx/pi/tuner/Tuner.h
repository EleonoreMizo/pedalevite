/*****************************************************************************

        Tuner.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_tuner_Tuner_HEADER_INCLUDED)
#define mfx_pi_tuner_Tuner_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/AllocAlign.h"
#include "mfx/dsp/ana/FreqYin.h"
#include "mfx/pi/tuner/TunerDesc.h"
#include "mfx/piapi/PluginInterface.h"

#include <vector>



namespace mfx
{
namespace pi
{
namespace tuner
{



class Tuner
:	public piapi::PluginInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Tuner ()  = default;
	virtual        ~Tuner () = default;

	float          get_freq () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginInterface
	virtual State  do_get_state () const;
	virtual double do_get_param_val (piapi::ParamCateg categ, int index, int note_id) const;
	virtual int    do_reset (double sample_freq, int max_buf_len, int &latency);
	virtual void   do_clean_quick ();
	virtual void   do_process_block (piapi::ProcInfo &proc);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int                    // Subsampling
	               _sub_spl = 4;

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	void           clear_buffers ();

	TunerDesc      _desc;
	State          _state   = State_CREATED;

	dsp::ana::FreqYin
	               _analyser;
	float          _freq    = 0;        // Hz. 0 = not found
	BufAlign       _buffer;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Tuner (const Tuner &other)             = delete;
	Tuner &        operator = (const Tuner &other)        = delete;
	bool           operator == (const Tuner &other) const = delete;
	bool           operator != (const Tuner &other) const = delete;

}; // class Tuner



}  // namespace tuner
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/tuner/Tuner.hpp"



#endif   // mfx_pi_tuner_Tuner_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
