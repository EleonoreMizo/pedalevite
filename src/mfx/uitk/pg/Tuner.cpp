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

#include "fstb/fnc.h"
#include "mfx/uitk/pg/Tuner.h"
#include "mfx/uitk/NodeEvt.h"
#include "mfx/uitk/PageMgrInterface.h"
#include "mfx/uitk/PageSwitcher.h"
#include "mfx/ui/Font.h"
#include "mfx/ui/LedInterface.h"
#include "mfx/View.h"

#include <cassert>
#include <cmath>
#include <climits>



namespace mfx
{
namespace uitk
{
namespace pg
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



Tuner::Tuner (PageSwitcher &page_switcher, ui::LedInterface &led)
:	_page_switcher (page_switcher)
,	_led (led)
,	_leg_beg ((led.get_nbr_led () - _nbr_led) >> 1)
,	_view_ptr (0)
,	_page_ptr (0)
,	_page_size ()
,	_scale (Scale_GUITAR_EBGDAE)
,	_note_sptr ( new NText (0))
,	_scale_sptr (new NText (1))
,	_fnt_l_ptr (0)
{
	_note_sptr->set_justification (0.5f, 0.5f, false);
	_scale_sptr->set_justification (0.5f, 0.5f, false);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Tuner::do_connect (Model &model, const View &view, PageMgrInterface &page, Vec2d page_size, void *usr_ptr, const ui::Font &fnt_s, const ui::Font &fnt_m, const ui::Font &fnt_l)
{
	_view_ptr  = &view;
	_page_ptr  = &page;
	_page_size = page_size;
	_fnt_l_ptr = &fnt_l;

	_note_sptr->set_font (fnt_l);
	_scale_sptr->set_font (fnt_l);

	const int      x_mid = _page_size [0] >> 1;
	const int      y_mid = _page_size [1] >> 1;

	_note_sptr->set_coord (Vec2d (x_mid, y_mid));
	_scale_sptr->set_coord (Vec2d (x_mid, y_mid));

	_page_ptr->push_back (_note_sptr);
	_page_ptr->push_back (_scale_sptr);

	i_set_freq (_view_ptr->get_tuner_freq ());
}



void	Tuner::do_disconnect ()
{
	for (int index = 0; index < _nbr_led; ++index)
	{
		_led.set_led (_leg_beg + index, 0);
	}
}



MsgHandlerInterface::EvtProp	Tuner::do_handle_evt (const NodeEvt &evt)
{
	EvtProp        ret_val = EvtProp_PASS;

	if (evt.get_type () == NodeEvt::Type_BUTTON)
	{
		const Button   but = evt.get_button ();
		switch (but)
		{
		case Button_U:
		case Button_L:
			_scale = Scale ((int (_scale) + Scale_NBR_ELT - 1) % Scale_NBR_ELT);
			ret_val = EvtProp_CATCH;
			break;
		case Button_D:
		case Button_R:
			_scale = Scale ((int (_scale)                 + 1) % Scale_NBR_ELT);
			ret_val = EvtProp_CATCH;
			break;
		default:
			// Nothing
			break;
		}
		i_set_freq (_view_ptr->get_tuner_freq ());
	}

	return ret_val;
}



void	Tuner::do_set_tuner (bool active_flag)
{
	if (! active_flag)
	{
		_page_switcher.return_page ();
	}
}



void	Tuner::do_set_tuner_freq (float freq)
{
	i_set_freq (freq);
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Tuner::i_set_freq (float freq)
{
	std::array <float, _nbr_led>  lum_arr = {{ 0, 0, 0 }};

	if (freq <= 0)
	{
		std::string       scale_name;
		switch (_scale)
		{
		case Scale_CHROMA:		  scale_name = "CHROMATIC"; break;
		case Scale_GUITAR_EBGDAE: scale_name = "GUITAR";    break;
		case Scale_BASS_GDAE:	  scale_name = "BASS";      break;
		default:
			assert (false);
			break;
		}

		_note_sptr->set_text ("");
		_scale_sptr->set_text (scale_name);
	}

	else
	{
		_scale_sptr->set_text ("");

		/*** To do: take the selected scale into account ***/

		const float    midi_pitch = log2 (freq / 220) * 12 - 3 + 60;

		// LEDs
		const float    target     = find_closest_note (midi_pitch, _scale);
		const int      tg_midi    = fstb::round_int (target);
		const int      tg_octave  = tg_midi / 12;
		const int      tg_note    = tg_midi - tg_octave * 12;
		const float    dist_cent  = (midi_pitch - target) * 100;
		const float    cents_abs  = fabs (dist_cent);
		lum_arr [1] = std::max (5 - cents_abs, 0.0f) * (1.0f / 5);

		const float    lum        = fstb::limit (cents_abs * (1.0f / 25), 0.0f, 1.0f);
		lum_arr [0] = (dist_cent < 0) ? lum : 0;
		lum_arr [2] = (dist_cent > 0) ? lum : 0;

		// Note
		char           note3_0 [3+1] = "-";
		if (tg_octave >= 0 && tg_octave <= 9)
		{
			fstb::snprintf4all (
				note3_0, sizeof (note3_0),
				"%s%1d",
				_note_0_arr [tg_note], tg_octave
			);
		}
		const std::string note_str (note3_0);
		const int      len        = int (note_str.length ());
		const int      w_l        = _fnt_l_ptr->get_char_w ();
		const int      h_l        = _fnt_l_ptr->get_char_h ();
		const int      mag_x      = _page_size [0] / (w_l * len);
		const int      mag_y      = _page_size [1] / h_l;
		_note_sptr->set_mag (mag_x, mag_y);
		_note_sptr->set_text (note3_0);

		// Frequency and finetune, currently unused
		const int      midi_note  = fstb::round_int (midi_pitch);
		const float    cents_flt  = (midi_pitch - midi_note) * 100;
		const int      cents      = fstb::round_int (cents_flt);
		const int      octave     = midi_note / 12;
		const int      note       = midi_note - octave * 12;
		char           freq_0 [127+1] = "---- ---- ------- Hz";
		fstb::snprintf4all (
			freq_0, sizeof (freq_0),
			"%2s%-2d %+4d %7.3lf Hz",
			_note_0_arr [note], octave, cents, freq
		);
	}

	for (int index = 0; index < _nbr_led; ++index)
	{
		float           val = lum_arr [index];
		val = val * val;  // Gamma 2.0
		_led.set_led (_leg_beg + index, val);
	}
}



float	Tuner::find_closest_note (float note, Scale sc)
{
	if (sc == Scale_CHROMA)
	{
		note = float (fstb::round (note));
	}
	else
	{
		const std::vector <float> note_set = _scale_data [sc];
		assert (! note_set.empty ());
		float       best_dist = 1e9f;
		float       best_note = -1;
		for (auto tst : note_set)
		{
			const float    dist = fabs (tst - note);
			if (dist < best_dist)
			{
				best_note = tst;
				best_dist = dist;
			}
		}
		if (best_note >= 0)
		{
			note = best_note;
		}
	}

	return note;
}



const char * const	Tuner::_note_0_arr [12] =
{
	"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};

const std::array <std::vector <float>, Tuner::Scale_NBR_ELT>	Tuner::_scale_data =
{{
	{ },
	{ 36+4, 36+9, 48+2, 48+7, 48+11, 60+4 }, // Scale_GUITAR_EBGDAE
	{ 24+4, 24+9, 36+2, 36+7 }               // Scale_BASS_GDAE
}};


}  // namespace pg
}  // namespace uitk
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
