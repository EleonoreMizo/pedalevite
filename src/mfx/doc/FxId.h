/*****************************************************************************

        FxId.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_FxId_HEADER_INCLUDED)
#define mfx_doc_FxId_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/PiType.h"

#include <string>



namespace mfx
{
namespace doc
{



class SerRInterface;
class SerWInterface;

class FxId
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum LocType
	{
		LocType_INVALID = -1,

		LocType_LABEL = 0,
		LocType_CATEGORY,

		LocType_NBR_ELT
	};

	explicit       FxId (LocType loc_type, std::string label, PiType type);
	               FxId (const FxId &other)       = default;
	virtual        ~FxId ()                       = default;

	FxId &         operator = (const FxId &other) = default;

	bool           operator == (const FxId &other) const;
	bool           operator != (const FxId &other) const;

	void           ser_write (SerWInterface &ser) const;
	void           ser_read (SerRInterface &ser);

	LocType        _location_type = LocType_INVALID;
	std::string    _label_or_model;
	PiType         _type          = PiType_MAIN;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               FxId () = delete;

}; // class FxId



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/FxId.hpp"



#endif   // mfx_doc_FxId_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
