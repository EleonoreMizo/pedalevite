/*****************************************************************************

        TreeList.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_cmd_lat_TreeList_HEADER_INCLUDED)
#define mfx_cmd_lat_TreeList_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



namespace mfx
{
namespace cmd
{
namespace lat
{



class GraphInterface;

class TreeList
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               TreeList ()                        = default;
	               TreeList (const TreeList &other)   = default;
	virtual        ~TreeList ()                       = default;
	TreeList &     operator = (const TreeList &other) = default;

	void           init (GraphInterface &graph);
	void				restore ();

	int            get_nbr_trees () const;
	int            get_nbr_nodes (int tree) const;
	int            get_node (int tree, int index) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class TreeInfo
	{
	public:
		explicit       TreeInfo (int start_index);
		               TreeInfo ()                        = default;
		               TreeInfo (const TreeInfo &other)   = default;
		virtual        ~TreeInfo ()                       = default;
		TreeInfo &     operator = (const TreeInfo &other) = default;

		int            _nbr_nodes   =  0;
		int            _start_index = -1;   // Within _node_list. -1 = not initialised
	};

	typedef std::vector <TreeInfo> InfoList;
	typedef std::vector <int>      NodeList;

	void           build_rec (GraphInterface &graph, int node_index, int cur_tree);

	int            _nbr_trees = -1;  // -1 = not initialised
	InfoList       _info_list;
	NodeList       _node_list;       // One array for all trees



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const TreeList &other) const = delete;
	bool           operator != (const TreeList &other) const = delete;

}; // class TreeList



}  // namespace lat
}  // namespace cmd
}  // namespace mfx



//#include "mfx/cmd/lat/TreeList.hpp"



#endif   // mfx_cmd_lat_TreeList_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
