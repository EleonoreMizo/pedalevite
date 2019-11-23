/*****************************************************************************

        Algo.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_cmd_lat_Algo_HEADER_INCLUDED)
#define mfx_cmd_lat_Algo_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/cmd/lat/Cnx.h"
#include "mfx/cmd/lat/GraphInterface.h"
#include "mfx/cmd/lat/Node.h"
#include "mfx/cmd/lat/TreeList.h"

#include <array>
#include <vector>



namespace mfx
{
namespace cmd
{
namespace lat
{



class Algo
:	public GraphInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Algo ()                        = default;
	               Algo (const Algo &other);
	virtual        ~Algo ()                       = default;
	Algo &         operator = (const Algo &other);

	void           reset ();
   void           set_nbr_elt (int nbr_nodes, int nbr_cnx);
   void           run ();

	int            get_bm_nbr_rec_calls () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

   // GraphInterface
	virtual int    do_get_nbr_nodes () const;
	virtual Node & do_use_node (int index);
	virtual const Node &
	               do_use_node (int index) const;
	
	virtual int    do_get_nbr_cnx () const;
	virtual Cnx &  do_use_cnx (int index);
	virtual const Cnx &
	               do_use_cnx (int index) const;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class TermNodeList
	{
	public:
		typedef std::vector <int> IndexList;

		IndexList      _list;      // Contains node indexes in _node_list
		int            _nbr_nodes = 0;
	};

	typedef std::array <TermNodeList, piapi::Dir_NBR_ELT> TermNodeListArray;
	typedef std::vector <Cnx> CnxList;
	typedef std::vector <Node> NodeList;
	typedef std::vector <bool> FlagList;

	void           build_term_node_list (int tree_cnt);
	void           set_initial_timestamps ();
	void           compute_remaining_timestamps (int tree_cnt);
	bool           skip_term_nodes_known_timestamps (int &pos, const TermNodeList &tnl);
	void           reset_visit_list ();
	bool           compute_timestamp_rec (int node_index, piapi::Dir dir);
	bool           compute_timestamp_bck_rec (int node_index);

	static int     get_timestamp_at (const Node &node, piapi::Dir dir);
	static int     get_biggest_timestamp (long ts_1, long ts_2, piapi::Dir dir);
	static int     compute_delay_between (const Node &node_1, const Node &node_2, piapi::Dir node_1_end);

	CnxList        _cnx_list;
	NodeList       _node_list;

	int            _bm_nbr_rec_calls = 0;   // For benchmarking

	// Temporary data
	TreeList       _tree_list;
	TermNodeListArray                       // Source or terminal nodes, for current tree
	               _term_node_list_arr;
	FlagList			_visited_flag_list;




/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Algo &other) const = delete;
	bool           operator != (const Algo &other) const = delete;

}; // class Algo



}  // namespace lat
}  // namespace cmd
}  // namespace mfx



//#include "mfx/cmd/lat/Algo.hpp"



#endif   // mfx_cmd_lat_Algo_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
