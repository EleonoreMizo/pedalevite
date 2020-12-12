/*****************************************************************************

        Tag.h
        Author: Laurent de Soras, 2020

Reference list of plug-in category tags.
Most of the tags are taken from Ardour:
https://github.com/Ardour/ardour/blob/master/share/plugin_metadata/plugin_tags

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_piapi_Tag_HEADER_INCLUDED)
#define mfx_piapi_Tag_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace piapi
{



class Tag
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	// Ardour tags
	static const char *  _amp_simulator_0;
	static const char *  _analyser_0;
	static const char *  _auto_mixing_0;
	static const char *  _bass_0;
	static const char *  _channel_strip_0;
	static const char *  _delay_0;
	static const char *  _distortion_0;
	static const char *  _drum_machine_0;
	static const char *  _drum_replacement_0;
	static const char *  _drums_0;
	static const char *  _dyn_compressor_0;
	static const char *  _dyn_de_esser_0;
	static const char *  _dyn_gate_expander_0;
	static const char *  _dyn_limiter_0;
	static const char *  _dyn_transient_designer_0;
	static const char *  _eq_dynamic_0;
	static const char *  _eq_filter_0;
	static const char *  _eq_graphic_0;
	static const char *  _eq_linear_phase_0;
	static const char *  _eq_parametric_0;
	static const char *  _eq_tilt_0;
	static const char *  _example_0;
	static const char *  _freeform_0;
	static const char *  _guitar_0;
	static const char *  _inline_ui_0;
	static const char *  _instrument_0;
	static const char *  _keys_0;
	static const char *  _mastering_0;
	static const char *  _microphone_0;
	static const char *  _mid_side_0;
	static const char *  _midi_effect_0;
	static const char *  _modulation_0;
	static const char *  _multiband_0;
	static const char *  _noise_reduction_0;
	static const char *  _pitch_shift_0;
	static const char *  _pre_amp_0;
	static const char *  _reverb_0;
	static const char *  _sampler_0;
	static const char *  _spatial_0;
	static const char *  _stomp_box_0;
	static const char *  _subharmonic_0;
	static const char *  _surround_0;
	static const char *  _synth_0;
	static const char *  _tape_simulator_0;
	static const char *  _time_align_0;
	static const char *  _tuner_0;
	static const char *  _utility_0;
	static const char *  _vocal_0;

	// Other tags
	static const char *  _control_gen_0;
	static const char *  _mixing_0;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Tag ()                               = delete;
	               Tag (const Tag &other)               = delete;
	               Tag (Tag &&other)                    = delete;
	               ~Tag ()                              = delete;
	Tag &          operator = (const Tag &other)        = delete;
	Tag &          operator = (Tag &&other)             = delete;
	bool           operator == (const Tag &other) const = delete;
	bool           operator != (const Tag &other) const = delete;

}; // class Tag



}  // namespace piapi
}  // namespace mfx



//#include "mfx/piapi/Tag.hpp"



#endif   // mfx_piapi_Tag_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
