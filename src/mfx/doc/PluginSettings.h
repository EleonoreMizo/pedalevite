/*****************************************************************************

        PluginSettings.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_PluginSettings_HEADER_INCLUDED)
#define mfx_doc_PluginSettings_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/CtrlLinkSet.h"
#include "mfx/doc/ParamPresentation.h"

#include <map>
#include <vector>



namespace mfx
{
namespace doc
{



class SerRInterface;
class SerWInterface;

class PluginSettings
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef std::map <int, CtrlLinkSet> MapParamCtrl;
	typedef std::map <int, ParamPresentation> MapPres;
	typedef std::vector <float> ParamList;

	               PluginSettings ()                            = default;
	               PluginSettings (const PluginSettings &other) = default;
	virtual        ~PluginSettings ()                           = default;
	PluginSettings &
	               operator = (const PluginSettings &other)     = default;

	CtrlLinkSet &  use_ctrl_link_set (int index);
	const CtrlLinkSet &
	               use_ctrl_link_set (int index) const;
	bool           has_ctrl (int index) const;
	bool           has_any_ctrl () const;

	const std::set <float> *
	               find_notch_list (int index) const;
	const ParamPresentation *
	               use_pres_if_tempo_ctrl (int index) const;
	ParamPresentation *
	               use_pres_if_tempo_ctrl (int index);

	void           ser_write (SerWInterface &ser) const;
	void           ser_read (SerRInterface &ser);

	bool           _force_mono_flag  = false;
	bool           _force_reset_flag = false;
	ParamList      _param_list;
	MapParamCtrl   _map_param_ctrl;     // Parameter index -> controller list
	MapPres        _map_param_pres;     // Parameter index -> presentation



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const PluginSettings &other) const = delete;
	bool           operator != (const PluginSettings &other) const = delete;

}; // class PluginSettings



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/PluginSettings.hpp"



#endif   // mfx_doc_PluginSettings_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
