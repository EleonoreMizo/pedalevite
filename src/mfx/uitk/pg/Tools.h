/*****************************************************************************

        Tools.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_pg_Tools_HEADER_INCLUDED)
#define mfx_uitk_pg_Tools_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/pg/PedalConf.h"
#include "mfx/uitk/MsgHandlerInterface.h"
#include "mfx/PiType.h"

#include <string>
#include <vector>



namespace mfx
{

class ControlSource;
class Model;
class View;

namespace doc
{
	class ActionParam;
	class FxId;
	class PedalActionGroup;
	class PedalboardLayout;
	class PluginSettings;
	class Preset;
}

namespace piapi
{
	class ParamDescInterface;
}

namespace uitk
{

class NText;

namespace pg
{



class CtrlSrcNamed;

class Tools
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class NodeEntry
	{
	public:
		int            _slot_id      = -1;
		int            _instance_nbr = -1;  // -1 = unique instance
		std::string    _type;               // Empty string: empty slot
		std::string    _name_multilabel;
	};

	static void    set_param_text (const Model &model, const View &view, int width, int index, float val, int slot_id, PiType type, NText *param_name_ptr, NText &param_val, NText *param_unit_ptr, NText *fx_name_ptr, bool group_unit_val_flag);
	static MsgHandlerInterface::EvtProp
	               change_param (Model &model, const View &view, int slot_id, PiType type, int index, float step, int step_index, int dir);
	static double  change_param (double val_nrm, const Model &model, const View &view, int slot_id, PiType type, int index, float step, int step_index, int dir);
	static int     find_ctrl_index (const ControlSource &src, const std::vector <CtrlSrcNamed> &ctrl_list);
	static std::string
	               find_ctrl_name (const ControlSource &src, const std::vector <CtrlSrcNamed> &ctrl_list);
	static std::vector <CtrlSrcNamed>
	               make_port_list (const Model &model, const View &view);
	static int     change_plugin (Model &model, const View &view, int slot_id, int dir, const std::vector <std::string> &fx_list, bool chain_flag);
	static void    assign_default_rotenc_mapping (Model &model, const View &view, int slot_id, int page);

	static std::string
	               conv_pedal_conf_to_short_txt (PedalConf &conf, const doc::PedalboardLayout &layout, int index, const Model &model, const View &view);

	static std::vector <NodeEntry>
	               extract_slot_list (const doc::Preset &preset, const Model &model);
	static int     find_chain_index (const doc::Preset &preset, int slot_id);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static void    print_param_with_pres (std::string &val_s, std::string &unit, const doc::Preset &preset, int slot_id, PiType type, int index, float val, const piapi::ParamDescInterface &desc, double tempo);
	static bool    is_pedal_empty (const doc::PedalActionGroup &group);
	static bool    is_pedal_simple_action (const doc::PedalActionGroup &group, const Model &model, const View &view, std::string &name);
	static bool    is_pedal_momentary_button (const doc::PedalActionGroup &group, const Model &model, const View &view, std::string &name);
	static bool    is_pedal_toggle (const doc::PedalActionGroup &group, const Model &model, const View &view, std::string &name);
	static std::string
	               find_fx_type (const doc::FxId &fx_id, const View &view);
	static std::string
	               print_param_action (const doc::ActionParam &param, const Model &model, const View &view);
	static void    create_missing_signal_ports (Model &model, const View &view, int slot_id);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Tools ()                               = delete;
	               Tools (const Tools &other)             = delete;
	virtual        ~Tools () = default;
	Tools &        operator = (const Tools &other)        = delete;
	bool           operator == (const Tools &other) const = delete;
	bool           operator != (const Tools &other) const = delete;

}; // class Tools



}  // namespace pg
}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/pg/Tools.hpp"



#endif   // mfx_uitk_pg_Tools_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
