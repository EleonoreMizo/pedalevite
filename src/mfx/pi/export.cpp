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
#include "mfx/pi/click/Click.h"
#include "mfx/pi/click/ClickDesc.h"
#include "mfx/pi/colorme/ColorMe.h"
#include "mfx/pi/colorme/ColorMeDesc.h"
#include "mfx/pi/cpx/Compex.h"
#include "mfx/pi/cpx/CompexDesc.h"
#include "mfx/pi/dist1/DistoSimple.h"
#include "mfx/pi/dist1/DistoSimpleDesc.h"
#include "mfx/pi/dist2/Disto2x.h"
#include "mfx/pi/dist2/Disto2xDesc.h"
#include "mfx/pi/distpwm/DistoPwm.h"
#include "mfx/pi/distpwm/DistoPwmDesc.h"
#include "mfx/pi/dly1/Delay.h"
#include "mfx/pi/dly1/DelayDesc.h"
#include "mfx/pi/dly2/Delay2.h"
#include "mfx/pi/dly2/Delay2Desc.h"
#include "mfx/pi/dtone1/DistTone.h"
#include "mfx/pi/dtone1/DistToneDesc.h"
#include "mfx/pi/dwm/DryWet.h"
#include "mfx/pi/dwm/DryWetDesc.h"
#include "mfx/pi/envf/EnvFollow.h"
#include "mfx/pi/envf/EnvFollowDesc.h"
#include "mfx/pi/freqsh/FrequencyShifter.h"
#include "mfx/pi/freqsh/FreqShiftDesc.h"
#include "mfx/pi/flancho/Flancho.h"
#include "mfx/pi/flancho/FlanchoDesc.h"
#include "mfx/pi/fv/Freeverb.h"
#include "mfx/pi/fv/FreeverbDesc.h"
#include "mfx/pi/iifix/IIFix.h"
#include "mfx/pi/iifix/IIFixDesc.h"
#include "mfx/pi/lfo1/Lfo.h"
#include "mfx/pi/lfo1/LfoDesc.h"
#include "mfx/pi/lpfs/Squeezer.h"
#include "mfx/pi/lpfs/SqueezerDesc.h"
#include "mfx/pi/nzbl/NoiseBleach.h"
#include "mfx/pi/nzbl/NoiseBleachDesc.h"
#include "mfx/pi/nzcl/NoiseChlorine.h"
#include "mfx/pi/nzcl/NoiseChlorineDesc.h"
#include "mfx/pi/peq/PEq.h"
#include "mfx/pi/peq/PEqDesc.h"
#include "mfx/pi/phase1/Phaser.h"
#include "mfx/pi/phase1/PhaserDesc.h"
#include "mfx/pi/phase2/Phaser2.h"
#include "mfx/pi/phase2/Phaser2Desc.h"
#include "mfx/pi/ramp/Ramp.h"
#include "mfx/pi/ramp/RampDesc.h"
#include "mfx/pi/spkem/SpeakerEmu.h"
#include "mfx/pi/spkem/SpeakerEmuDesc.h"
#include "mfx/pi/tost/ToStereo.h"
#include "mfx/pi/tost/ToStereoDesc.h"
#include "mfx/pi/trem1/Tremolo.h"
#include "mfx/pi/trem1/TremoloDesc.h"
#include "mfx/pi/tremh/HarmTrem.h"
#include "mfx/pi/tremh/HarmTremDesc.h"
#include "mfx/pi/tuner/Tuner.h"
#include "mfx/pi/tuner/TunerDesc.h"
#include "mfx/pi/wah1/Wah.h"
#include "mfx/pi/wah1/WahDesc.h"
#include "mfx/pi/wah2/Wah2.h"
#include "mfx/pi/wah2/Wah2Desc.h"
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
			mfx::piapi::FactoryTpl <mfx::pi::dwm::DryWetDesc        , mfx::pi::dwm::DryWet             >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::tuner::TunerDesc       , mfx::pi::tuner::Tuner            >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::dist1::DistoSimpleDesc , mfx::pi::dist1::DistoSimple      >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::freqsh::FreqShiftDesc  , mfx::pi::freqsh::FrequencyShifter>::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::trem1::TremoloDesc     , mfx::pi::trem1::Tremolo          >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::wah1::WahDesc          , mfx::pi::wah1::Wah               >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::dtone1::DistToneDesc   , mfx::pi::dtone1::DistTone        >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::iifix::IIFixDesc       , mfx::pi::iifix::IIFix            >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::flancho::FlanchoDesc   , mfx::pi::flancho::Flancho        >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::dly1::DelayDesc        , mfx::pi::dly1::Delay             >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::cpx::CompexDesc        , mfx::pi::cpx::Compex             >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::fv::FreeverbDesc       , mfx::pi::fv::Freeverb            >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::peq::PEqDesc <4>       , mfx::pi::peq::PEq <4>            >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::phase1::PhaserDesc     , mfx::pi::phase1::Phaser          >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::lpfs::SqueezerDesc     , mfx::pi::lpfs::Squeezer          >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::lfo1::LfoDesc          , mfx::pi::lfo1::Lfo               >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::envf::EnvFollowDesc    , mfx::pi::envf::EnvFollow         >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::dist2::Disto2xDesc     , mfx::pi::dist2::Disto2x          >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::spkem::SpeakerEmuDesc  , mfx::pi::spkem::SpeakerEmu       >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::tost::ToStereoDesc     , mfx::pi::tost::ToStereo          >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::distpwm::DistoPwmDesc  , mfx::pi::distpwm::DistoPwm       >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::tremh::HarmTremDesc    , mfx::pi::tremh::HarmTrem         >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::nzbl::NoiseBleachDesc  , mfx::pi::nzbl::NoiseBleach       >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::nzcl::NoiseChlorineDesc, mfx::pi::nzcl::NoiseChlorine     >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::click::ClickDesc       , mfx::pi::click::Click            >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::wah2::Wah2Desc         , mfx::pi::wah2::Wah2              >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::ramp::RampDesc         , mfx::pi::ramp::Ramp              >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::dly2::Delay2Desc       , mfx::pi::dly2::Delay2            >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::colorme::ColorMeDesc   , mfx::pi::colorme::ColorMe        >::create ()
		,	mfx::piapi::FactoryTpl <mfx::pi::phase2::Phaser2Desc    , mfx::pi::phase2::Phaser2         >::create ()
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
