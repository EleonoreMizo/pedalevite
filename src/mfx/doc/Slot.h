/*****************************************************************************

        Slot.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_Slot_HEADER_INCLUDED)
#define mfx_doc_Slot_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/PluginSettings.h"
#include "mfx/PiType.h"

#include <map>
#include <string>



namespace mfx
{
namespace doc
{



class FxId;
class SerRInterface;
class SerWInterface;

class Slot
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef std::map <std::string, PluginSettings> SettingHistory;

	               Slot ();
	               Slot (const Slot &other)       = default;
	               Slot (Slot &&other)            = default;
	               ~Slot ()                       = default;

	Slot &         operator = (const Slot &other) = default;
	Slot &         operator = (Slot &&other)      = default;

	bool           operator == (const Slot &other) const;
	bool           operator != (const Slot &other) const;
	bool           is_similar (const Slot &other) const;

	bool           is_empty () const;
	PluginSettings &
	               use_settings (PiType type);
	const PluginSettings &
	               use_settings (PiType type) const;
	PluginSettings *
	               test_and_get_settings (PiType type);
	const PluginSettings *
	               test_and_get_settings (PiType type) const;
	bool           has_ctrl () const;
	bool           is_referenced_by (const FxId &fx_id) const;

	void           ser_write (SerWInterface &ser) const;
	void           ser_read (SerRInterface &ser);

	std::string    _pi_model; // Empty string is legal here and means the slot is empty.
	SettingHistory _settings_all;
	PluginSettings _settings_mixer;
	std::string    _label;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

}; // class Slot



/*\\\ GLOBAL OPERATORS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	operator < (const Slot &lhs, const Slot &rhs);



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/Slot.hpp"



#endif   // mfx_doc_Slot_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
