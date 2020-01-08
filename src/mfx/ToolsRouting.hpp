/*****************************************************************************

        ToolsRouting.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_ToolsRouting_CODEHEADER_INCLUDED)
#define mfx_ToolsRouting_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	ToolsRouting::Node::operator == (const Node &other) const
{
   return (_type == other._type && _slot_id == other._slot_id);
}



bool	ToolsRouting::Node::operator != (const Node &other) const
{
   return ! (*this == other);
}


bool	ToolsRouting::Node::is_valid () const
{
   return (_type >= 0);
}



doc::CnxEnd::Type	ToolsRouting::Node::get_type () const
{
   return _type;
}



int	ToolsRouting::Node::get_slot_id () const
{
   return _slot_id;
}


bool	operator < (const ToolsRouting::Node &lhs, const ToolsRouting::Node &rhs)
{
   if (lhs.get_type () < rhs.get_type ())
   {
      return true;
   }
   else if (lhs.get_type () == rhs.get_type ())
   {
      return (lhs.get_slot_id () < rhs.get_slot_id ());
   }

   return false;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace mfx



#endif   // mfx_ToolsRouting_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

