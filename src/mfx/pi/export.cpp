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
#include "mfx/pi/adsr/EnvAdsr.h"
#include "mfx/pi/adsr/EnvAdsrDesc.h"
#include "mfx/pi/bmp1/BigMuff1.h"
#include "mfx/pi/bmp1/BigMuff1Desc.h"
#include "mfx/pi/click/Click.h"
#include "mfx/pi/click/ClickDesc.h"
#include "mfx/pi/cmerge/ChnMerge.h"
#include "mfx/pi/cmerge/ChnMergeDesc.h"
#include "mfx/pi/colorme/ColorMe.h"
#include "mfx/pi/colorme/ColorMeDesc.h"
#include "mfx/pi/cpx/Compex.h"
#include "mfx/pi/cpx/CompexDesc.h"
#include "mfx/pi/csplit/ChnSplit.h"
#include "mfx/pi/csplit/ChnSplitDesc.h"
#include "mfx/pi/dclip/DiodeClipper.h"
#include "mfx/pi/dclip/DiodeClipperDesc.h"
#include "mfx/pi/dist1/DistoSimple.h"
#include "mfx/pi/dist1/DistoSimpleDesc.h"
#include "mfx/pi/dist2/Disto2x.h"
#include "mfx/pi/dist2/Disto2xDesc.h"
#include "mfx/pi/dist3/Dist3.h"
#include "mfx/pi/dist3/Dist3Desc.h"
#include "mfx/pi/distapf/DistApf.h"
#include "mfx/pi/distapf/DistApfDesc.h"
#include "mfx/pi/distpwm/DistoPwm.h"
#include "mfx/pi/distpwm/DistoPwmDesc.h"
#include "mfx/pi/distpwm2/DistoPwm2.h"
#include "mfx/pi/distpwm2/DistoPwm2Desc.h"
#include "mfx/pi/dly0/Delay.h"
#include "mfx/pi/dly0/DelayDesc.h"
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
#include "mfx/pi/fsplit/FreqSplit.h"
#include "mfx/pi/fsplit/FreqSplitDesc.h"
#include "mfx/pi/fv/Freeverb.h"
#include "mfx/pi/fv/FreeverbDesc.h"
#include "mfx/pi/hcomb/HyperComb.h"
#include "mfx/pi/hcomb/HyperCombDesc.h"
#include "mfx/pi/iifix/IIFix.h"
#include "mfx/pi/iifix/IIFixDesc.h"
#include "mfx/pi/lfo1/Lfo.h"
#include "mfx/pi/lfo1/LfoDesc.h"
#include "mfx/pi/lpfs/Squeezer.h"
#include "mfx/pi/lpfs/SqueezerDesc.h"
#include "mfx/pi/moog1/MoogLpf.h"
#include "mfx/pi/moog1/MoogLpfDesc.h"
#include "mfx/pi/nzbl/NoiseBleach.h"
#include "mfx/pi/nzbl/NoiseBleachDesc.h"
#include "mfx/pi/nzcl/NoiseChlorine.h"
#include "mfx/pi/nzcl/NoiseChlorineDesc.h"
#include "mfx/pi/ms/MidSide.h"
#include "mfx/pi/ms/MidSideDesc.h"
#include "mfx/pi/osdet/OnsetDetect.h"
#include "mfx/pi/osdet/OnsetDetectDesc.h"
//#include "mfx/pi/osdet2/OnsetDetect2.h"
//#include "mfx/pi/osdet2/OnsetDetect2Desc.h"
#include "mfx/pi/pan/StereoPan.h"
#include "mfx/pi/pan/StereoPanDesc.h"
#include "mfx/pi/peq/PEq.h"
#include "mfx/pi/peq/PEqDesc.h"
#include "mfx/pi/phase1/Phaser.h"
#include "mfx/pi/phase1/PhaserDesc.h"
#include "mfx/pi/phase2/Phaser2.h"
#include "mfx/pi/phase2/Phaser2Desc.h"
#include "mfx/pi/pidet/PitchDetect.h"
#include "mfx/pi/pidet/PitchDetectDesc.h"
#include "mfx/pi/pidet2/PitchDetect2.h"
#include "mfx/pi/pidet2/PitchDetect2Desc.h"
#include "mfx/pi/psh1/PitchShift1.h"
#include "mfx/pi/psh1/PitchShift1Desc.h"
#include "mfx/pi/ramp/Ramp.h"
#include "mfx/pi/ramp/RampDesc.h"
#include "mfx/pi/scrush/SpectralCrusher.h"
#include "mfx/pi/scrush/SpectralCrusherDesc.h"
#include "mfx/pi/smood/SkoolMood.h"
#include "mfx/pi/smood/SkoolMoodDesc.h"
#include "mfx/pi/syn0/Synth0.h"
#include "mfx/pi/syn0/Synth0Desc.h"
#include "mfx/pi/spkem/SpeakerEmu.h"
#include "mfx/pi/spkem/SpeakerEmuDesc.h"
#include "mfx/pi/testgen/TestGen.h"
#include "mfx/pi/testgen/TestGenDesc.h"
#include "mfx/pi/tomo/ToMono.h"
#include "mfx/pi/tomo/ToMonoDesc.h"
#include "mfx/pi/tost/ToStereo.h"
#include "mfx/pi/tost/ToStereoDesc.h"
#include "mfx/pi/trem1/Tremolo.h"
#include "mfx/pi/trem1/TremoloDesc.h"
#include "mfx/pi/tremh/HarmTrem.h"
#include "mfx/pi/tremh/HarmTremDesc.h"
#include "mfx/pi/tuner/Tuner.h"
#include "mfx/pi/tuner/TunerDesc.h"
#include "mfx/pi/vclone/VolumeClone.h"
#include "mfx/pi/vclone/VolumeCloneDesc.h"
#include "mfx/pi/verb2/Platitude.h"
#include "mfx/pi/verb2/PlatitudeDesc.h"
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
		using namespace mfx::pi;
		using namespace mfx::piapi;

		static const std::vector <std::shared_ptr <mfx::piapi::FactoryInterface> > l =
		{
			FactoryTpl <dwm::DryWetDesc         , dwm::DryWet              >::create ()
		,	FactoryTpl <tuner::TunerDesc        , tuner::Tuner             >::create ()
		,	FactoryTpl <dist1::DistoSimpleDesc  , dist1::DistoSimple       >::create ()
		,	FactoryTpl <freqsh::FreqShiftDesc   , freqsh::FrequencyShifter >::create ()
		,	FactoryTpl <trem1::TremoloDesc      , trem1::Tremolo           >::create ()
		,	FactoryTpl <wah1::WahDesc           , wah1::Wah                >::create ()
		,	FactoryTpl <dtone1::DistToneDesc    , dtone1::DistTone         >::create ()
		,	FactoryTpl <iifix::IIFixDesc        , iifix::IIFix             >::create ()
		,	FactoryTpl <flancho::FlanchoDesc    , flancho::Flancho         >::create ()
		,	FactoryTpl <dly1::DelayDesc         , dly1::Delay              >::create ()
		,	FactoryTpl <cpx::CompexDesc         , cpx::Compex              >::create ()
		,	FactoryTpl <fv::FreeverbDesc        , fv::Freeverb             >::create ()
		,	FactoryTpl <peq::PEqDesc < 4>       , peq::PEq < 4>            >::create ()
		,	FactoryTpl <peq::PEqDesc < 8>       , peq::PEq < 8>            >::create ()
		,	FactoryTpl <peq::PEqDesc <16>       , peq::PEq <16>            >::create ()
		,	FactoryTpl <phase1::PhaserDesc      , phase1::Phaser           >::create ()
		,	FactoryTpl <lpfs::SqueezerDesc      , lpfs::Squeezer           >::create ()
		,	FactoryTpl <lfo1::LfoDesc <false>   , lfo1::Lfo <false>        >::create ()
		,	FactoryTpl <envf::EnvFollowDesc     , envf::EnvFollow          >::create ()
		,	FactoryTpl <dist2::Disto2xDesc      , dist2::Disto2x           >::create ()
		,	FactoryTpl <spkem::SpeakerEmuDesc   , spkem::SpeakerEmu        >::create ()
		,	FactoryTpl <tost::ToStereoDesc      , tost::ToStereo           >::create ()
		,	FactoryTpl <distpwm::DistoPwmDesc   , distpwm::DistoPwm        >::create ()
		,	FactoryTpl <tremh::HarmTremDesc     , tremh::HarmTrem          >::create ()
		,	FactoryTpl <nzbl::NoiseBleachDesc   , nzbl::NoiseBleach        >::create ()
		,	FactoryTpl <nzcl::NoiseChlorineDesc , nzcl::NoiseChlorine      >::create ()
		,	FactoryTpl <click::ClickDesc        , click::Click             >::create ()
		,	FactoryTpl <wah2::Wah2Desc          , wah2::Wah2               >::create ()
		,	FactoryTpl <ramp::RampDesc          , ramp::Ramp               >::create ()
		,	FactoryTpl <dly2::Delay2Desc        , dly2::Delay2             >::create ()
		,	FactoryTpl <colorme::ColorMeDesc    , colorme::ColorMe         >::create ()
		,	FactoryTpl <phase2::Phaser2Desc     , phase2::Phaser2          >::create ()
		,	FactoryTpl <pidet::PitchDetectDesc  , pidet::PitchDetect       >::create ()
		,	FactoryTpl <osdet::OnsetDetectDesc  , osdet::OnsetDetect       >::create ()
		,	FactoryTpl <syn0::Synth0Desc        , syn0::Synth0             >::create ()
		,	FactoryTpl <hcomb::HyperCombDesc    , hcomb::HyperComb         >::create ()
		,	FactoryTpl <testgen::TestGenDesc    , testgen::TestGen         >::create ()
		,	FactoryTpl <psh1::PitchShift1Desc   , psh1::PitchShift1        >::create ()
		,	FactoryTpl <distpwm2::DistoPwm2Desc , distpwm2::DistoPwm2      >::create ()
//		,	FactoryTpl <osdet2::OnsetDetect2Desc, osdet2::OnsetDetect2     >::create () // Does not work well enough
		,	FactoryTpl <distapf::DistApfDesc    , distapf::DistApf         >::create ()
		,	FactoryTpl <lfo1::LfoDesc <true>    , lfo1::Lfo <true>         >::create ()
		,	FactoryTpl <adsr::EnvAdsrDesc       , adsr::EnvAdsr            >::create ()
		,	FactoryTpl <dly0::DelayDesc         , dly0::Delay              >::create ()
		,	FactoryTpl <fsplit::FreqSplitDesc   , fsplit::FreqSplit        >::create ()
		,	FactoryTpl <dist3::Dist3Desc        , dist3::Dist3             >::create ()
		,	FactoryTpl <moog1::MoogLpfDesc      , moog1::MoogLpf           >::create ()
		,	FactoryTpl <dclip::DiodeClipperDesc , dclip::DiodeClipper      >::create ()
		,	FactoryTpl <smood::SkoolMoodDesc    , smood::SkoolMood         >::create ()
		,	FactoryTpl <bmp1::BigMuff1Desc      , bmp1::BigMuff1           >::create ()
		,	FactoryTpl <verb2::PlatitudeDesc    , verb2::Platitude         >::create ()
		,	FactoryTpl <vclone::VolumeCloneDesc , vclone::VolumeClone      >::create ()
		,	FactoryTpl <cmerge::ChnMergeDesc    , cmerge::ChnMerge         >::create ()
		,	FactoryTpl <csplit::ChnSplitDesc    , csplit::ChnSplit         >::create ()
		,	FactoryTpl <ms::MidSideDesc         , ms::MidSide              >::create ()
		,	FactoryTpl <pan::StereoPanDesc      , pan::StereoPan           >::create ()
		,	FactoryTpl <tomo::ToMonoDesc        , tomo::ToMono             >::create ()
		,	FactoryTpl <pidet2::PitchDetect2Desc, pidet2::PitchDetect2     >::create ()
		,	FactoryTpl <scrush::SpectralCrusherDesc,scrush::SpectralCrusher>::create ()
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
