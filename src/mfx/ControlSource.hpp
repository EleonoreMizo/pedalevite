/*****************************************************************************

        ControlSource.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_ControlSource_CODEHEADER_INCLUDED)
#define mfx_ControlSource_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	ControlSource::is_valid () const
{
	return (_type >= 0 && _index >= 0);
}



bool	ControlSource::is_relative () const
{
	return (_type == ControllerType (ui::UserInputType_ROTENC));
}



bool	ControlSource::operator == (const ControlSource &other) const
{
	return (_type == other._type && _index == other._index);
}



bool	ControlSource::operator != (const ControlSource &other) const
{
	return ! (*this == other);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ GLOBAL OPERATORS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	operator < (const ControlSource &lhs, const ControlSource &rhs)
{
	return (    lhs._type <  rhs._type
	        || (lhs._type == rhs._type && lhs._index < rhs._index));
}



}  // namespace mfx



#endif   // mfx_ControlSource_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
