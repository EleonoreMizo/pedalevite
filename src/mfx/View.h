/*****************************************************************************

        View.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_View_HEADER_INCLUDED)
#define mfx_View_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/Setup.h"
#include "mfx/ModelObserverInterface.h"

#include <set>



namespace mfx
{



class View
:	public ModelObserverInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class OverrideLoc
	{
	public:
		bool           operator < (const OverrideLoc &rhs) const;
		int            _slot_id;
		PiType         _type;
		int            _index;
	};
	typedef std::map <OverrideLoc, int> OverrideMap;

	               View ()  = default;
	virtual        ~View () = default;

	void           add_observer (ModelObserverInterface &obs);
	void           remove_observer (ModelObserverInterface &obs);

	double         get_tempo () const;
	bool           is_editing () const;
	bool           is_tuner_active () const;
	float          get_tuner_freq () const;
	const doc::Setup &
	               use_setup () const;
	const doc::Preset &
	               use_preset_cur () const;
	const SlotInfoMap &
	               use_slot_info_map () const;
	int            get_bank_index () const;
	int            get_preset_index () const;
	const OverrideMap &
	               use_param_ctrl_override_map () const;

	int            conv_slot_index_to_id (int slot_index) const;

	static void    update_parameter (doc::Preset &preset, int slot_index, PiType type, int index, float val);
	static float   get_param_val (const doc::Preset &preset, int slot_id, PiType type, int index);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::ModelObserverInterface
	virtual void   do_set_tempo (double bpm);
	virtual void   do_set_edit_mode (bool edit_flag);
	virtual void   do_set_save_mode (doc::Setup::SaveMode mode);
	virtual void   do_set_setup_name (std::string name);
	virtual void   do_set_pedalboard_layout (const doc::PedalboardLayout &layout);
	virtual void   do_set_pedal (const PedalLoc &loc, const doc::PedalActionGroup &content);
	virtual void   do_set_bank (int index, const doc::Bank &bank);
	virtual void   do_select_bank (int index);
	virtual void   do_set_bank_name (std::string name);
	virtual void   do_set_preset_name (std::string name);
	virtual void   do_activate_preset (int index);
	virtual void   do_store_preset (int preset_index, int bank_index);
	virtual void   do_set_chn_mode (ChnMode mode);
	virtual void   do_set_master_vol (float vol);
	virtual void   do_set_tuner (bool active_flag);
	virtual void   do_set_tuner_freq (float freq);
	virtual void   do_set_slot_info_for_current_preset (const SlotInfoMap &info_map);
	virtual void   do_set_param (int pi_id, int index, float val, int slot_id, PiType type);
	virtual void   do_set_param_beats (int slot_id, int index, float beats);
	virtual void   do_insert_slot (int slot_index, int slot_id);
	virtual void   do_erase_slot (int slot_index);
	virtual void   do_set_slot_label (int slot_id, std::string name);
	virtual void   do_set_plugin (int slot_id, const PluginInitData &pi_data);
	virtual void   do_remove_plugin (int slot_id);
	virtual void   do_set_plugin_mono (int slot_id, bool mono_flag);
	virtual void   do_set_param_ctrl (int slot_id, PiType type, int index, const doc::CtrlLinkSet &cls);
	virtual void   do_override_param_ctrl (int slot_id, PiType type, int index, int rotenc_index);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::set <ModelObserverInterface *> ObsSet;
	typedef std::set <int> PluginList;

	ObsSet         _obs_set;

	// Cached data
	double         _tempo        = Cst::_tempo_ref;
	bool           _edit_flag    = false;
	bool           _tuner_flag   = false;
	float          _tuner_freq   = 0;
	int            _bank_index   = 0;
	int            _preset_index = 0;
	doc::Setup     _setup;
	doc::Preset    _preset_cur;
	SlotInfoMap    _slot_info_map;
	OverrideMap    _override_map;

	PluginList     _lookup_list; // Id of the plug-ins we need to collect data after instantiation (number of parameters and other specs)



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               View (const View &other)              = delete;
	View &         operator = (const View &other)        = delete;
	bool           operator == (const View &other) const = delete;
	bool           operator != (const View &other) const = delete;

}; // class View



}  // namespace mfx



//#include "mfx/View.hpp"



#endif   // mfx_View_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
