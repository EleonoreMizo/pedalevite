/*****************************************************************************

        Setup.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_Setup_HEADER_INCLUDED)
#define mfx_doc_Setup_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/Bank.h"
#include "mfx/doc/PedalboardLayout.h"
#include "mfx/Cst.h"

#include <array>
#include <string>



namespace mfx
{
namespace doc
{



class SerRInterface;
class SerWInterface;

class Setup
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum SaveMode
	{
		SaveMode_MANUAL = 0,
		SaveMode_PARTIAL,
		SaveMode_AUTO,

		SaveMode_NBR_ELT
	};

	typedef std::array <Bank, Cst::_nbr_banks> BankArray;

	               Setup ()                               = default;
	               Setup (const Setup &other)             = default;
	virtual        ~Setup ()                              = default;

	Setup &        operator = (const Setup &other)        = default;

	void           ser_write (SerWInterface &ser) const;
	void           ser_read (SerRInterface &ser);

	std::string    _name;
	SaveMode       _save_mode = SaveMode_PARTIAL;
	PedalboardLayout
	               _layout;
	BankArray      _bank_arr;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Setup &other) const = delete;
	bool           operator != (const Setup &other) const = delete;

}; // class Setup



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/Setup.hpp"



#endif   // mfx_doc_Setup_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
