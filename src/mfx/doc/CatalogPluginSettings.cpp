/*****************************************************************************

        CatalogPluginSettings.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/CatalogPluginSettings.h"
#include "mfx/doc/SerRInterface.h"
#include "mfx/doc/SerWInterface.h"

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	CatalogPluginSettings::is_empty () const
{
	for (const auto &cell_sptr : _cell_arr)
	{
		if (cell_sptr.get () != 0)
		{
			return false;
		}
	}

	return true;
}



bool	CatalogPluginSettings::is_preset_existing (int index) const
{
	return (
		   index >= 0
		&& index < int (_cell_arr.size ())
		&& _cell_arr [index].get () != 0
	);
}



void	CatalogPluginSettings::ser_write (SerWInterface &ser) const
{
	ser.begin_list ();

	// We need a second level for future extensions (sorting the catalog)
	ser.begin_list ();
	const int      nbr_settings = int (_cell_arr.size ());
	for (int index = 0; index < nbr_settings; ++index)
	{
		if (is_preset_existing (index))
		{
			const Cell &   cell = *(_cell_arr [index]);

			ser.begin_list ();

			ser.write (index);
			ser.write (cell._name);
			cell._main.ser_write (ser);
			cell._mixer.ser_write (ser);
			
			ser.end_list ();
		}
	}
	ser.end_list ();

	ser.end_list ();
}



void	CatalogPluginSettings::ser_read (SerRInterface &ser, std::string pi_model)
{
	ser.begin_list ();

	int            nbr_elt;
	ser.begin_list (nbr_elt);
	_cell_arr.clear ();
	_cell_arr.reserve (nbr_elt);
	for (int pos = 0; pos < nbr_elt; ++pos)
	{
		ser.begin_list ();

		int            index = -1;
		ser.read (index);
		assert (index >= 0);
		if (index >= int (_cell_arr.size ()))
		{
			_cell_arr.resize (index + 1);
		}
		_cell_arr [index] = CellSPtr (new Cell);
		Cell &         cell = *(_cell_arr [index]);
		ser.read (cell._name);
		cell._main.ser_read (ser, pi_model);
		cell._mixer.ser_read (ser, Cst::_plugin_dwm);

		ser.end_list ();
	}
	trim_array ();
	ser.end_list ();

	ser.end_list ();
}



void	CatalogPluginSettings::trim_array ()
{
	while (! _cell_arr.empty () && _cell_arr.back ().get () == 0)
	{
		_cell_arr.pop_back ();
	}
}



void	CatalogPluginSettings::add_settings (int index, const Cell &cell)
{
	assert (index >= 0);

	if (index >= int (_cell_arr.size ()))
	{
		_cell_arr.resize (index + 1);
	}

	if (_cell_arr [index].get () == 0)
	{
		_cell_arr [index] = CellSPtr (new Cell (cell));
	}
	else
	{
		*(_cell_arr [index]) = cell;
	}
}



void	CatalogPluginSettings::remove_settings (int index)
{
	assert (index >= 0);

	if (index < int (_cell_arr.size ()))
	{
		_cell_arr [index].reset ();
		trim_array ();
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
