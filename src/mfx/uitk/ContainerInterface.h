/*****************************************************************************

        ContainerInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_uitk_ContainerInterface_HEADER_INCLUDED)
#define mfx_uitk_ContainerInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/uitk/NodeInterface.h"
#include "mfx/uitk/ParentInterface.h"

#include <memory>



namespace mfx
{
namespace uitk
{



class ContainerInterface
:	public ParentInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef std::shared_ptr <NodeInterface> NodeSPtr;

	void           push_back (NodeSPtr node_sptr);
	void           set_node (int pos, NodeSPtr node_sptr);
	void           insert (int pos, NodeSPtr node_sptr);
	void           erase (int pos);
	int            get_nbr_nodes () const;
	NodeSPtr       use_node (int pos);

	// Convenience function
	void           clear_all_nodes ();
	void           erase (NodeSPtr node_sptr);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void   do_push_back (NodeSPtr node_sptr) = 0;
	virtual void   do_set_node (int pos, NodeSPtr node_sptr) = 0;
	virtual void   do_insert (int pos, NodeSPtr node_sptr) = 0;
	virtual void   do_erase (int pos) = 0;
	virtual int    do_get_nbr_nodes () const = 0;
	virtual NodeSPtr
	               do_use_node (int pos) = 0;



}; // class ContainerInterface



}  // namespace uitk
}  // namespace mfx



//#include "mfx/uitk/ContainerInterface.hpp"



#endif   // mfx_uitk_ContainerInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
