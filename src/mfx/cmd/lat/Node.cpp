/*****************************************************************************

        Node.cpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/cmd/lat/Node.h"
#include "mfx/cmd/lat/Tools.h"

#include <cassert>



namespace mfx
{
namespace cmd
{
namespace lat
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	Node::set_nature (Nature nature)
{
	assert (nature >= 0);
	assert (nature < Nature_NBR_ELT);

	_nature = nature;
}



Node::Nature	Node::get_nature () const
{
	return _nature;
}



// All previous connections are lost
void	Node::set_nbr_cnx (piapi::Dir dir, int nbr_cnx)
{
	assert (dir >= 0);
	assert (dir < piapi::Dir_NBR_ELT);
	assert (nbr_cnx >= 0);

	CnxList &      cnx_list = _end_list [dir];
	cnx_list.assign (nbr_cnx, -1);
}



int	Node::get_nbr_cnx (piapi::Dir dir) const
{
	assert (dir >= 0);
	assert (dir < piapi::Dir_NBR_ELT);

	return int (_end_list [dir].size ());
}



void	Node::set_cnx (piapi::Dir dir, int ref_index, int cnx_index)
{
	assert (dir >= 0);
	assert (dir < piapi::Dir_NBR_ELT);
	assert (ref_index >= 0);
	assert (ref_index < get_nbr_cnx (dir));
	assert (cnx_index >= 0);

	_end_list [dir] [ref_index] = cnx_index;
}



void	Node::add_cnx (piapi::Dir dir, int cnx_index)
{
	assert (dir >= 0);
	assert (dir < piapi::Dir_NBR_ELT);
	assert (cnx_index >= 0);

	_end_list [dir].push_back (cnx_index);
}



int	Node::get_cnx (piapi::Dir dir, int ref_index) const
{
	assert (dir >= 0);
   assert (dir < piapi::Dir_NBR_ELT);
	assert (ref_index >= 0);
   assert (ref_index < get_nbr_cnx (dir));

   const int      cnx_index = _end_list [dir] [ref_index];
   assert (cnx_index >= 0);

	return cnx_index;
}



bool	Node::is_pure (piapi::Dir dir) const
{
	assert (dir >= 0);
   assert (dir < piapi::Dir_NBR_ELT);

   const piapi::Dir  inv_dir = Tools::invert_dir (dir);

	return (get_nbr_cnx (inv_dir) == 0);
}



bool	Node::is_tree_set () const
{
	return (_tree >= 0);
}



void	Node::set_tree (int tree)
{
	assert (tree >= 0);

	_tree = tree;
}



int	Node::get_tree () const
{
   assert (is_tree_set ());

	return _tree;
}



bool	Node::is_timestamp_set () const
{
	return _timestamp_flag;
}



void	Node::set_timestamp (int timestamp)
{
	_timestamp      = timestamp;
   _timestamp_flag = true;
}



int	Node::get_timestamp () const
{
   assert (_timestamp_flag);

	return _timestamp;
}



void	Node::set_latency (int latency)
{
	assert (latency >= 0);

	_latency = latency;
}



int	Node::get_latency () const
{
	return _latency;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace lat
}  // namespace cmd
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
