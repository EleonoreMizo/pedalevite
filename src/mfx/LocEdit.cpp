/*****************************************************************************

        LocEdit.cpp
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

#include "mfx/LocEdit.h"
#include "mfx/Model.h"
#include "mfx/ToolsRouting.h"
#include "mfx/View.h"

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include <cassert>



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// When _slot_id becomes valid again, the audio flag must be updated.
void	LocEdit::fix_audio_flag (const View &view, const Model &model)
{
	if (_slot_id >= 0)
	{
		const doc::Preset &  prog    = view.use_preset_cur ();
		const auto           it_slot = prog._slot_map.find (_slot_id);
		if (it_slot != prog._slot_map.end ())
		{
			ToolsRouting::NodeMap   graph;
			ToolsRouting::build_node_graph (
				graph, prog.use_routing ()._cnx_audio_set
			);
			const std::vector <std::string> &   aud_pi_list =
				model.use_aud_pi_list ();
			std::set <std::string> aud_pi_set;
			aud_pi_set.insert (aud_pi_list.begin (), aud_pi_list.end ());
			std::vector <int> list_aud;
			std::vector <int> list_sig;
			ToolsRouting::build_ordered_node_lists (
				list_aud, list_sig, prog, graph, aud_pi_set
			);
			const auto     it = std::find (
				list_aud.begin (),
				list_aud.end (),
				_slot_id
			);
			_audio_flag = (it != list_aud.end ());
		}
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
