/*****************************************************************************

        CatalogPluginSettings.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_CatalogPluginSettings_HEADER_INCLUDED)
#define mfx_doc_CatalogPluginSettings_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/PluginSettings.h"

#include <memory>
#include <vector>



namespace mfx
{
namespace doc
{



class CatalogPluginSettings
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class Cell
	{
	public:
		std::string    _name;
		PluginSettings _main;
		PluginSettings _mixer;
	};
	
	typedef std::shared_ptr <Cell> CellSPtr;
	typedef std::vector <CellSPtr> CellArray;

	virtual        ~CatalogPluginSettings () = default;
	               CatalogPluginSettings ()  = default;
	               CatalogPluginSettings (const CatalogPluginSettings &other) = default;
	CatalogPluginSettings &
	               operator = (const CatalogPluginSettings &other)            = default;

	bool           is_empty () const;
	bool           is_preset_existing (int index) const;
	void           ser_write (SerWInterface &ser) const;
	void           ser_read (SerRInterface &ser);
	void           trim_array ();
	void           add_settings (int index, const Cell &cell);
	void           remove_settings (int index);

	CellArray      _cell_arr;    // Shared pointers can be 0



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const CatalogPluginSettings &other) const = delete;
	bool           operator != (const CatalogPluginSettings &other) const = delete;

}; // class CatalogPluginSettings



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/CatalogPluginSettings.hpp"



#endif   // mfx_doc_CatalogPluginSettings_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
