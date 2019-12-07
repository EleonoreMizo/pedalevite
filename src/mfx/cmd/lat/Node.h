/*****************************************************************************

        Node.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_cmd_lat_Node_HEADER_INCLUDED)
#define mfx_cmd_lat_Node_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/piapi/Dir.h"

#include <array>
#include <vector>



namespace mfx
{
namespace cmd
{
namespace lat
{



class Node
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Nature
	{
		Nature_NORMAL = 0,
		Nature_SOURCE,
		Nature_SINK,

		Nature_NBR_ELT
	};

	               Node ()                        = default;
	               Node (const Node &other)       = default;
	virtual        ~Node ()                       = default;
	Node &         operator = (const Node &other) = default;

	void           set_nature (Nature nature);
	Nature         get_nature () const;

	void           set_nbr_cnx (piapi::Dir dir, int nbr_cnx);
	int            get_nbr_cnx (piapi::Dir dir) const;

	void           set_cnx (piapi::Dir dir, int ref_index, int cnx_index);
	void           add_cnx (piapi::Dir dir, int cnx_index);
	int            get_cnx (piapi::Dir dir, int ref_index) const;

	bool           is_pure (piapi::Dir dir) const;

	bool           is_tree_set () const;
	void           set_tree (int tree);
	int            get_tree () const;

	bool           is_timestamp_set () const;
	void           set_timestamp (int timestamp);
	int            get_timestamp () const;

	void				set_latency (int latency);
	int				get_latency () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <int> CnxList;    // -1 = not initialised
	typedef std::array <CnxList, piapi::Dir_NBR_ELT> EndList;

	// Indexes of connexions for each end
	EndList        _end_list;

	Nature         _nature         = Nature_NORMAL;

	// Between inputs and outputs, samples, >= 0. -1 = not initialised
	int            _latency        = -1;

	// Global output latency, samples
	int            _timestamp      = 0;

	// Set if timestamp is known.
	bool           _timestamp_flag = false;

	// Tree index. -1 = not initialised
	int            _tree           = -1;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Node &other) const = delete;
	bool           operator != (const Node &other) const = delete;

}; // class Node



}  // namespace lat
}  // namespace cmd
}  // namespace mfx



//#include "mfx/cmd/lat/Node.hpp"



#endif   // mfx_cmd_lat_Node_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
