/*****************************************************************************

        PluginDetails.h
        Author: Laurent de Soras, 2022

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_PluginDetails_HEADER_INCLUDED)
#define mfx_PluginDetails_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



#define mfx_PluginDetails_USE_TIMINGS



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/BitFieldSparse.h"
#include "mfx/piapi/PluginInterface.h"
#include "mfx/PluginPoolHostInterface.h"
#if defined (mfx_PluginDetails_USE_TIMINGS)
# include "mfx/dsp/dyn/MeterRmsPeakHold.h"
# include "mfx/MeterResult.h"
#endif // mfx_PluginDetails_USE_TIMINGS

#include <memory>
#include <vector>



namespace mfx
{



namespace piapi
{
	class PluginDescInterface;
	class PluginPoolHostInterface;
}

class PluginDetails
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef std::unique_ptr <piapi::PluginInterface> PluginUPtr;
	typedef std::unique_ptr <PluginPoolHostInterface> HostUPtr;

	int            reset_plugin (double sample_freq, int max_block_size, int &latency);

	PluginUPtr     _pi_uptr;
	HostUPtr       _host_uptr;
	const piapi::PluginDescInterface *
	               _desc_ptr = nullptr;

	// Must be set by the client after each reset()
	int            _latency  = 0;

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// R/W for the audio thread, R/O for others

	/*** To do:
	These variables should be atomic<>. However strict consistency is not
	an issue as all these variables are meant to be independent from each
	others. But it would be probably better to enforce synchronisation, if
	it doesn't harm the performance significantly.
	***/

	// Only for ParamCateg_GLOBAL. Read-only for non-audio threads.
	std::vector <float>
	               _param_arr;

	// Final value of modulated parameters. Negative if not available.
	// Only for ParamCateg_GLOBAL. Read-only for non-audio threads.
	// Only indicative, there is no guarantee of validity.
	std::vector <float>
	               _param_mod_arr;

#if defined (mfx_PluginDetails_USE_TIMINGS)
	// Valid only for "main" plug-ins. Includes the d/w/b aux plug-in.
	MeterResult    _dsp_use; // In [0 ; 1]
#endif // mfx_PluginDetails_USE_TIMINGS

	// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
	// Audio thread only

	// Same remark. Managed by the audio thread only.
	fstb::BitFieldSparse
	               _param_update;

	// Indicates that the latest change comes from the audio thread.
	// Same remark. Audio thread only
	std::vector <bool>
	               _param_update_from_audio;

	// Reset flags. Audio thread only
	bool           _rst_steady_flag = false;
	bool           _rst_full_flag   = false;

#if defined (mfx_PluginDetails_USE_TIMINGS)
	dsp::dyn::MeterRmsPeakHold
	               _proc_analyser;
#endif // mfx_PluginDetails_USE_TIMINGS



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PluginDetails &other) const = delete;
	bool           operator != (PluginDetails &other) const       = delete;

}; // class PluginDetails



}  // namespace mfx



//#include "mfx/PluginDetails.hpp"



#endif   // mfx_PluginDetails_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
