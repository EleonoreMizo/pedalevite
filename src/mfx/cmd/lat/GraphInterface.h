/*****************************************************************************

        GraphInterface.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_cmd_lat_GraphInterface_HEADER_INCLUDED)
#define mfx_cmd_lat_GraphInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace cmd
{
namespace lat
{



class Cnx;
class Node;

class GraphInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	virtual        ~GraphInterface () = default;

	int            get_nbr_nodes () const;
	Node &         use_node (int index);
	const Node &   use_node (int index) const;

	int            get_nbr_cnx () const;
	Cnx &          use_cnx (int index);
	const Cnx &    use_cnx (int index) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual int    do_get_nbr_nodes () const = 0;
	virtual Node & do_use_node (int index) = 0;
	virtual const Node &
	               do_use_node (int index) const = 0;
	
	virtual int    do_get_nbr_cnx () const = 0;
	virtual Cnx &  do_use_cnx (int index) = 0;
	virtual const Cnx &
	               do_use_cnx (int index) const = 0;


}; // class GraphInterface



}  // namespace lat
}  // namespace cmd
}  // namespace mfx



//#include "mfx/cmd/lat/GraphInterface.hpp"



#endif   // mfx_cmd_lat_GraphInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
