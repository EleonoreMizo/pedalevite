/*****************************************************************************

        Cnx.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_cmd_lat_Cnx_HEADER_INCLUDED)
#define mfx_cmd_lat_Cnx_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/piapi/Dir.h"

#include <array>



namespace mfx
{
namespace cmd
{
namespace lat
{



class Cnx
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Cnx ()                               = default;
	               Cnx (const Cnx &other)               = default;
	virtual        ~Cnx ()                              = default;
	Cnx &          operator = (const Cnx &other)        = default;

	void           set_node (piapi::Dir dir, int node_index);
	int            get_node (piapi::Dir dir) const;

	void           set_comp_delay (int dly_spl);
	int            get_comp_delay () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::array <int, piapi::Dir_NBR_ELT> EndArray;

	// Reference to end nodes. -1 = not set
	// Dir_IN  = connected to an input (destination)
	// Dir_OUT = connected to an output (source)
	EndArray       _end_arr    = {{ -1, -1 }};

	// Compensation delay (result of the algorithm), in samples
	int            _comp_delay = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Cnx &other) const = delete;
	bool           operator != (const Cnx &other) const = delete;

}; // class Cnx



}  // namespace lat
}  // namespace cmd
}  // namespace mfx



//#include "mfx/cmd/lat/Cnx.hpp"



#endif   // mfx_cmd_lat_Cnx_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
