/*****************************************************************************

        Document.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_cmd_Document_HEADER_INCLUDED)
#define mfx_cmd_Document_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/cmd/Slot.h"
#include "mfx/ChnMode.h"
#include "mfx/PiType.h"
#include "mfx/ProcessingContext.h"

#include <map>
#include <memory>
#include <vector>



namespace mfx
{
namespace cmd
{



class Document
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Document ()                      = default;
	               Document (const Document &other) = default;
	virtual        ~Document ()                     = default;

	typedef std::map <int, bool> InstanceMap; // [Id] = use

	class PluginLoc
	{
	public:
		int            _slot_pos;
		PiType         _type;
	};

	typedef std::shared_ptr <ProcessingContext> ContextSPtr;

	Plugin &       find_plugin (int pi_id);

	std::vector <Slot>
	               _slot_list;
	ContextSPtr    _ctx_sptr;
	std::map <std::string, InstanceMap> // All the existing plug-ins, ordered by model.
	               _map_model_id;
	std::map <int, PluginLoc>
	               _map_id_loc;
	ChnMode        _chn_mode               = ChnMode_1M_1M;
	float          _master_vol             = 1;
	bool           _smooth_transition_flag = false;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	Document &     operator = (const Document &other)        = delete;
	bool           operator == (const Document &other) const = delete;
	bool           operator != (const Document &other) const = delete;

}; // class Document



}  // namespace cmd
}  // namespace mfx



//#include "mfx/cmd/Document.hpp"



#endif   // mfx_cmd_Document_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
