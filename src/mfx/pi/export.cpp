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
#include "mfx/pi/badrad/BadRadio.h"
#include "mfx/pi/badrad/BadRadioDesc.h"
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
#include "mfx/pi/lipid/Lipidipi.h"
#include "mfx/pi/lipid/LipidipiDesc.h"
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
#include "mfx/pi/sfreeze/SpectralFreeze.h"
#include "mfx/pi/sfreeze/SpectralFreezeDesc.h"
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
#include "mfx/pi/vfreeze/VelvetFreeze.h"
#include "mfx/pi/vfreeze/VelvetFreezeDesc.h"
#include "mfx/pi/wah1/Wah.h"
#include "mfx/pi/wah1/WahDesc.h"
#include "mfx/pi/wah2/Wah2.h"
#include "mfx/pi/wah2/Wah2Desc.h"
#include "mfx/pi/export.h"

#include <cassert>



template <class D, class P>
static void	add (std::vector <std::unique_ptr <mfx::piapi::FactoryInterface> > &fl)
{
	fl.emplace_back (mfx::piapi::FactoryTpl <D, P>::create ());
}



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



fstb_EXPORT (int fstb_CDECL enum_factories (std::vector <std::unique_ptr <mfx::piapi::FactoryInterface> > &fact_list))
{
	int            ret_val = fstb::Err_OK;

	try
	{
		using namespace mfx::pi;

		std::vector <std::unique_ptr <mfx::piapi::FactoryInterface> > fl;

		add <dwm::DryWetDesc            , dwm::DryWet              > (fl);
		add <tuner::TunerDesc           , tuner::Tuner             > (fl);
		add <dist1::DistoSimpleDesc     , dist1::DistoSimple       > (fl);
		add <freqsh::FreqShiftDesc      , freqsh::FrequencyShifter > (fl);
		add <trem1::TremoloDesc         , trem1::Tremolo           > (fl);
		add <wah1::WahDesc              , wah1::Wah                > (fl);
		add <dtone1::DistToneDesc       , dtone1::DistTone         > (fl);
		add <iifix::IIFixDesc           , iifix::IIFix             > (fl);
		add <flancho::FlanchoDesc       , flancho::Flancho         > (fl);
		add <dly1::DelayDesc            , dly1::Delay              > (fl);
		add <cpx::CompexDesc            , cpx::Compex              > (fl);
		add <fv::FreeverbDesc           , fv::Freeverb             > (fl);
		add <peq::PEqDesc < 4>          , peq::PEq < 4>            > (fl);
		add <peq::PEqDesc < 8>          , peq::PEq < 8>            > (fl);
		add <peq::PEqDesc <16>          , peq::PEq <16>            > (fl);
		add <phase1::PhaserDesc         , phase1::Phaser           > (fl);
		add <lpfs::SqueezerDesc         , lpfs::Squeezer           > (fl);
		add <lfo1::LfoDesc <false>      , lfo1::Lfo <false>        > (fl);
		add <envf::EnvFollowDesc        , envf::EnvFollow          > (fl);
		add <dist2::Disto2xDesc         , dist2::Disto2x           > (fl);
		add <spkem::SpeakerEmuDesc      , spkem::SpeakerEmu        > (fl);
		add <tost::ToStereoDesc         , tost::ToStereo           > (fl);
		add <distpwm::DistoPwmDesc      , distpwm::DistoPwm        > (fl);
		add <tremh::HarmTremDesc        , tremh::HarmTrem          > (fl);
		add <nzbl::NoiseBleachDesc      , nzbl::NoiseBleach        > (fl);
		add <nzcl::NoiseChlorineDesc    , nzcl::NoiseChlorine      > (fl);
		add <click::ClickDesc           , click::Click             > (fl);
		add <wah2::Wah2Desc             , wah2::Wah2               > (fl);
		add <ramp::RampDesc             , ramp::Ramp               > (fl);
		add <dly2::Delay2Desc           , dly2::Delay2             > (fl);
		add <colorme::ColorMeDesc       , colorme::ColorMe         > (fl);
		add <phase2::Phaser2Desc        , phase2::Phaser2          > (fl);
		add <pidet::PitchDetectDesc     , pidet::PitchDetect       > (fl);
		add <osdet::OnsetDetectDesc     , osdet::OnsetDetect       > (fl);
		add <syn0::Synth0Desc           , syn0::Synth0             > (fl);
		add <hcomb::HyperCombDesc       , hcomb::HyperComb         > (fl);
		add <testgen::TestGenDesc       , testgen::TestGen         > (fl);
		add <psh1::PitchShift1Desc      , psh1::PitchShift1        > (fl);
		add <distpwm2::DistoPwm2Desc    , distpwm2::DistoPwm2      > (fl);
//		add <osdet2::OnsetDetect2Desc   , osdet2::OnsetDetect2     > (fl); // Does not work well enough
		add <distapf::DistApfDesc       , distapf::DistApf         > (fl);
		add <lfo1::LfoDesc <true>       , lfo1::Lfo <true>         > (fl);
		add <adsr::EnvAdsrDesc          , adsr::EnvAdsr            > (fl);
		add <dly0::DelayDesc            , dly0::Delay              > (fl);
		add <fsplit::FreqSplitDesc      , fsplit::FreqSplit        > (fl);
		add <dist3::Dist3Desc           , dist3::Dist3             > (fl);
		add <moog1::MoogLpfDesc         , moog1::MoogLpf           > (fl);
		add <dclip::DiodeClipperDesc    , dclip::DiodeClipper      > (fl);
		add <smood::SkoolMoodDesc       , smood::SkoolMood         > (fl);
		add <bmp1::BigMuff1Desc         , bmp1::BigMuff1           > (fl);
		add <verb2::PlatitudeDesc       , verb2::Platitude         > (fl);
		add <vclone::VolumeCloneDesc    , vclone::VolumeClone      > (fl);
		add <cmerge::ChnMergeDesc       , cmerge::ChnMerge         > (fl);
		add <csplit::ChnSplitDesc       , csplit::ChnSplit         > (fl);
		add <ms::MidSideDesc            , ms::MidSide              > (fl);
		add <pan::StereoPanDesc         , pan::StereoPan           > (fl);
		add <tomo::ToMonoDesc           , tomo::ToMono             > (fl);
		add <pidet2::PitchDetect2Desc   , pidet2::PitchDetect2     > (fl);
		add <scrush::SpectralCrusherDesc, scrush::SpectralCrusher  > (fl);
		add <sfreeze::SpectralFreezeDesc, sfreeze::SpectralFreeze  > (fl);
		add <vfreeze::VelvetFreezeDesc  , vfreeze::VelvetFreeze    > (fl);
		add <badrad::BadRadioDesc       , badrad::BadRadio         > (fl);
		add <lipid::LipidipiDesc        , lipid::Lipidipi          > (fl);

		fact_list.swap (fl);
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
