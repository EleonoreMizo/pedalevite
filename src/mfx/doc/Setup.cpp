/*****************************************************************************

        Setup.cpp
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

#include "mfx/doc/SerRInterface.h"
#include "mfx/doc/SerWInterface.h"
#include "mfx/doc/Setup.h"
#include "mfx/Cst.h"

#include <cassert>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Setup::ser_write (SerWInterface &ser) const
{
	const int      version = Cst::_format_version;

	ser.begin_list ();
	ser.write (version);
	ser.write (_name);
	ser.write (_save_mode);
	ser.write (_chn_mode);
	ser.write (_master_vol);
	_layout.ser_write (ser);

	ser.begin_list ();
	for (const auto &b : _bank_arr)
	{
		b.ser_write (ser);
	}
	ser.end_list ();

	if (version >= 5)
	{
		ser.begin_list ();

		for (const auto &m : _map_plugin_settings)
		{
			if (! m.second.is_empty ())
			{
				ser.begin_list ();
				ser.write (m.first);
				m.second.ser_write (ser);
				ser.end_list ();
			}
		}

		ser.end_list ();
	}

	ser.end_list ();
}



void	Setup::ser_read (SerRInterface &ser)
{
	int            version = -1;

	ser.begin_list ();
	ser.read (version);

	ser.set_doc_version (version);

	ser.read (_name);
	ser.read (_save_mode);
	ser.read (_chn_mode);
	ser.read (_master_vol);
	_layout.ser_read (ser);

	int            nbr_elt;
	ser.begin_list (nbr_elt);
	assert (nbr_elt == int (_bank_arr.size ()));
	for (auto &b : _bank_arr)
	{
		b.ser_read (ser);
	}
	ser.end_list ();

	_map_plugin_settings.clear ();
	if (version >= 5)
	{
		ser.begin_list (nbr_elt);
		for (int pos = 0; pos < nbr_elt; ++pos)
		{
			ser.begin_list ();
			std::string    pi_model;
			ser.read (pi_model);
			CatalogPluginSettings & cat = _map_plugin_settings [pi_model];
			cat.ser_read (ser, pi_model);
			ser.end_list ();
		}

		ser.end_list ();
	}

	ser.end_list ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
