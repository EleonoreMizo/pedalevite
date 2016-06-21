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

	               View ()  = default;
	virtual        ~View () = default;

	void           add_observer (ModelObserverInterface &obs);
	void           remove_observer (ModelObserverInterface &obs);

	bool           is_editing () const;
	bool           is_tuner_active () const;
	float          get_tuner_freq () const;
	const doc::Setup &
	               use_setup () const;
	const doc::Preset &
	               use_preset_cur () const;
	const SlotInfoList &
	               use_slot_info_list () const;
	int            get_bank_index () const;
	int            get_preset_index () const;

	static void    update_parameter (doc::Preset &preset, int slot_index, PiType type, int index, float val);
	static float   get_param_val (const doc::Preset &preset, int slot_index, PiType type, int index);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::ModelObserverInterface
	virtual void   do_set_edit_mode (bool edit_flag);
	virtual void   do_set_pedalboard_layout (const doc::PedalboardLayout &layout);
	virtual void   do_set_bank (int index, const doc::Bank &bank);
	virtual void   do_select_bank (int index);
	virtual void   do_set_bank_name (std::string name);
	virtual void   do_set_preset_name (std::string name);
	virtual void   do_activate_preset (int index);
	virtual void   do_store_preset (int index);
	virtual void   do_set_tuner (bool active_flag);
	virtual void   do_set_tuner_freq (float freq);
	virtual void   do_set_slot_info_for_current_preset (const SlotInfoList &info_list);
	virtual void   do_set_param (int pi_id, int index, float val, int slot_index, PiType type);
	virtual void   do_set_nbr_slots (int nbr_slots);
	virtual void   do_set_plugin (int slot_index, const PluginInitData &pi_data);
	virtual void   do_remove_plugin (int slot_index);
	virtual void   do_set_param_ctrl (int slot_index, PiType type, int index, const doc::CtrlLinkSet &cls);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::set <ModelObserverInterface *> ObsSet;
	typedef std::set <int> PluginList;

	ObsSet         _obs_set;

	// Cached data
	bool           _edit_flag    = true;
	bool           _tuner_flag   = false;
	float          _tuner_freq   = 0;
	int            _bank_index   = 0;
	int            _preset_index = 0;
	doc::Setup     _setup;
	doc::Preset    _preset_cur;
	SlotInfoList   _slot_info_list;

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
