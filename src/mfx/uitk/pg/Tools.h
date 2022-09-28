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

#include "mfx/doc/CnxEnd.h"
#include "mfx/piapi/Dir.h"
#include "mfx/uitk/pg/PedalConf.h"
#include "mfx/uitk/MsgHandlerInterface.h"
#include "mfx/uitk/PageMgrInterface.h"
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
	class ActionToggleFx;
	class FxId;
	class PedalActionGroup;
	class PedalActionSingleInterface;
	class PedalboardLayout;
	class PluginSettings;
	class Program;
}

namespace piapi
{
	class ParamDescInterface;
}

namespace uitk
{

class ContainerInterface;
class NText;

namespace pg
{



class CtrlSrcNamed;

class Tools
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef std::shared_ptr <NText> TxtSPtr;
	typedef std::vector <TxtSPtr> TxtArray;

	class NodeEntry
	{
	public:
		int            _slot_id      = -1;
		int            _instance_nbr = -1;  // -1 = unique instance
		std::string    _type;               // Empty string: empty slot
		std::string    _name_multilabel { "<Empty>" };
	};

	static double  get_param_nat (const doc::PluginSettings &settings, const piapi::PluginDescInterface &desc_pi, int index);
	static void    set_param_text (const Model &model, const View &view, int width, int index, float val, int slot_id, PiType type, NText *param_name_ptr, NText &param_val, NText *param_unit_ptr, NText *fx_name_ptr, bool group_unit_val_flag);
	static void    print_param_with_pres (std::string &val_s, std::string &unit, const Model &model, const View &view, const doc::Program &prog, int slot_id, PiType type, int index, float val);
	static MsgHandlerInterface::EvtProp
	               change_param (Model &model, const View &view, int slot_id, PiType type, int index, double step, int step_index, int dir);
	static double  change_param (double val_nrm, const Model &model, const View &view, int slot_id, PiType type, int index, double step, int step_index, int dir);
	static double  change_param (double val_nrm, const View &view, const piapi::PluginDescInterface &desc_pi, const doc::PluginSettings &settings, int index, double step, int step_index, int dir);
	static double  change_param (double val_nrm, const piapi::PluginDescInterface &desc_pi, int index, double step, int dir);
	static int     find_ctrl_index (const ControlSource &src, const std::vector <CtrlSrcNamed> &ctrl_list);
	static std::string
	               find_ctrl_name (const ControlSource &src, const std::vector <CtrlSrcNamed> &ctrl_list);
	static std::vector <CtrlSrcNamed>
	               make_port_list (const Model &model, const View &view);
	static int     change_plugin (Model &model, const View &view, int slot_id, int dir, const std::vector <std::string> &fx_list, bool chain_flag);
	static int     change_plugin (Model &model, const View &view, int slot_id, std::string model_id, bool chain_flag);
	static void    assign_default_rotenc_mapping (Model &model, const View &view, int slot_id, int page);
	static bool    get_physical_io (int &nbr_i, int &nbr_o, int &nbr_s, int slot_id, const doc::Program &prog, const Model &model);

	static std::string
	               conv_pedal_conf_to_short_txt (PedalConf &conf, const doc::PedalActionGroup &group, const Model &model, const View &view);
	static std::string
	               conv_pedal_conf_to_short_txt (PedalConf &conf, const doc::PedalboardLayout &layout, int index, const Model &model, const View &view);
	static std::string
	               conv_pedal_action_to_short_txt (const doc::PedalActionSingleInterface &action, const Model &model, const View &view);

	static int     extract_slot_list (std::vector <NodeEntry> &slot_list, const doc::Program &prog, const Model &model);
	static std::string
	               build_slot_name_with_index (const NodeEntry &entry);
	static int     find_linear_index_audio_graph (const View &view, int slot_id);
	static std::string
	               find_fx_type (const doc::FxId &fx_id, const View &view);
	static std::string
	               find_fx_type_in_prog (const std::string &label, const doc::Program &prog);
	static void    print_action_toggle_fx (std::string &model_name, const doc::ActionToggleFx &action, const Model &model, const View &view);
	static void    print_action_param (std::string &model_name, std::string &param_name, const doc::ActionParam &action, const Model &model, const View &view);
	static void    print_cnx_name (NText &txtbox, int width, const std::vector <Tools::NodeEntry> &entry_list, piapi::Dir dir, const doc::CnxEnd &cnx_end, const char prefix_0 [], int nbr_pins);

	static void    create_bank_list (TxtArray &bank_list, ContainerInterface &menu, PageMgrInterface::NavLocList &nav_list, const View &view, const ui::Font &fnt, int y, int w, bool chk_cur_flag);
	static void    create_prog_list (TxtArray &prog_list, ContainerInterface &menu, PageMgrInterface::NavLocList &nav_list, const View &view, const ui::Font &fnt, int y, int w);

	static void    draw_curve (std::vector <int32_t> y_arr, uint8_t *disp_ptr, int height, int stride, uint8_t c);
	static void    complete_v_seg (uint8_t *disp_ptr, int x, int y, int yn, int height, int stride, uint8_t c);

	static std::array <const char *, piapi::Dir_NBR_ELT>
	               _dir_txt_arr;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static void    print_param_with_pres (std::string &val_s, std::string &unit, const doc::Program &prog, int slot_id, PiType type, int index, float val, const piapi::ParamDescInterface &desc, double tempo);
	static bool    is_pedal_empty (const doc::PedalActionGroup &group);
	static bool    is_pedal_simple_action (const doc::PedalActionGroup &group, const Model &model, const View &view, std::string &name);
	static bool    is_pedal_momentary_button (const doc::PedalActionGroup &group, const Model &model, const View &view, std::string &name);
	static bool    is_pedal_toggle (const doc::PedalActionGroup &group, const Model &model, const View &view, std::string &name);
	static std::string
	               print_action_toggle_fx (const doc::ActionToggleFx &action, const Model &model, const View &view);
	static std::string
	               print_action_param (const doc::ActionParam &action, const Model &model, const View &view);
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
