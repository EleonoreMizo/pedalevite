/*****************************************************************************

        Tag.cpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/piapi/Tag.h"

#include <cassert>



namespace mfx
{
namespace piapi
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



const char *	Tag::_amp_simulator_0          = "amp-simulator";
const char *	Tag::_analyser_0               = "analyser";
const char *	Tag::_auto_mixing_0            = "auto-mixing";
const char *	Tag::_bass_0                   = "bass";
const char *	Tag::_channel_strip_0          = "channel-strip";
const char *	Tag::_delay_0                  = "delay";
const char *	Tag::_distortion_0             = "distortion";
const char *	Tag::_drum_machine_0           = "drum-machine";
const char *	Tag::_drum_replacement_0       = "drum-replacement";
const char *	Tag::_drums_0                  = "drums";
const char *	Tag::_dyn_compressor_0         = "dyn:compressor";
const char *	Tag::_dyn_de_esser_0           = "dyn:de-esser";
const char *	Tag::_dyn_gate_expander_0      = "dyn:gate/expander";
const char *	Tag::_dyn_limiter_0            = "dyn:limiter";
const char *	Tag::_dyn_transient_designer_0 = "dyn:transient-designer";
const char *	Tag::_eq_dynamic_0             = "eq:dynamic";
const char *	Tag::_eq_filter_0              = "eq:filter";
const char *	Tag::_eq_graphic_0             = "eq:graphic";
const char *	Tag::_eq_linear_phase_0        = "eq:linear-phase";
const char *	Tag::_eq_parametric_0          = "eq:parametric";
const char *	Tag::_eq_tilt_0                = "eq:tilt";
const char *	Tag::_example_0                = "example";
const char *	Tag::_freeform_0               = "freeform";
const char *	Tag::_guitar_0                 = "guitar";
const char *	Tag::_inline_ui_0              = "inline-ui";
const char *	Tag::_instrument_0             = "instrument";
const char *	Tag::_keys_0                   = "keys";
const char *	Tag::_mastering_0              = "mastering";
const char *	Tag::_microphone_0             = "microphone";
const char *	Tag::_mid_side_0               = "mid/side";
const char *	Tag::_midi_effect_0            = "midi-effect";
const char *	Tag::_modulation_0             = "modulation";
const char *	Tag::_multiband_0              = "multiband";
const char *	Tag::_noise_reduction_0        = "noise-reduction";
const char *	Tag::_pitch_shift_0            = "pitch-shift";
const char *	Tag::_pre_amp_0                = "pre-amp";
const char *	Tag::_reverb_0                 = "reverb";
const char *	Tag::_sampler_0                = "sampler";
const char *	Tag::_spatial_0                = "spatial";
const char *	Tag::_stomp_box_0              = "stomp-box";
const char *	Tag::_subharmonic_0            = "subharmonic";
const char *	Tag::_surround_0               = "surround";
const char *	Tag::_synth_0                  = "synth";
const char *	Tag::_tape_simulator_0         = "tape-simulator";
const char *	Tag::_time_align_0             = "time-align";
const char *	Tag::_tuner_0                  = "tuner";
const char *	Tag::_utility_0                = "utility";
const char *	Tag::_vocal_0                  = "vocal";

const char *	Tag::_control_gen_0            = "control-gen";
const char *	Tag::_mixing_0                 = "mixing";
const char *	Tag::_spectral_0               = "spectral";
const char *	Tag::_granular_0               = "granular";



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace piapi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
