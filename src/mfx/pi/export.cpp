/*****************************************************************************

        export.cpp
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

#include "fstb/Err.h"
#include "mfx/piapi/FactoryTpl.h"
#include "mfx/pi/dist1/DistoSimple.h"
#include "mfx/pi/dist1/DistoSimpleDesc.h"
#include "mfx/pi/dwm/DryWet.h"
#include "mfx/pi/dwm/DryWetDesc.h"
#include "mfx/pi/freqsh/FrequencyShifter.h"
#include "mfx/pi/freqsh/FreqShiftDesc.h"
#include "mfx/pi/trem1/Tremolo.h"
#include "mfx/pi/trem1/TremoloDesc.h"
#include "mfx/pi/tuner/Tuner.h"
#include "mfx/pi/tuner/TunerDesc.h"
#include "mfx/pi/wha1/Wha.h"
#include "mfx/pi/wha1/WhaDesc.h"
#include "mfx/pi/export.h"

#include <cassert>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



fstb_EXPORT (int fstb_CDECL enum_factories (std::vector <std::shared_ptr <mfx::piapi::FactoryInterface> > &fact_list))
{
	int            ret_val = fstb::Err_OK;

	try
	{
		static const std::vector <std::shared_ptr <mfx::piapi::FactoryInterface> > l =
		{
			mfx::piapi::FactoryTpl <mfx::pi::dwm::DryWetDesc       , mfx::pi::dwm::DryWet             >::create (),
			mfx::piapi::FactoryTpl <mfx::pi::tuner::TunerDesc      , mfx::pi::tuner::Tuner            >::create (),
			mfx::piapi::FactoryTpl <mfx::pi::dist1::DistoSimpleDesc, mfx::pi::dist1::DistoSimple      >::create (),
			mfx::piapi::FactoryTpl <mfx::pi::freqsh::FreqShiftDesc , mfx::pi::freqsh::FrequencyShifter>::create (),
			mfx::piapi::FactoryTpl <mfx::pi::trem1::TremoloDesc    , mfx::pi::trem1::Tremolo          >::create (),
			mfx::piapi::FactoryTpl <mfx::pi::wha1::WhaDesc         , mfx::pi::wha1::Wha               >::create ()
		};

		fact_list = l;
	}
	catch (...)
	{
		assert (false);
		ret_val = fstb::Err_EXCEPTION;
		fact_list.clear ();
	}

	return ret_val;
}




/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
