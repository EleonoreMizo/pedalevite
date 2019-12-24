/*****************************************************************************

        Cnx.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_cmd_Cnx_CODEHEADER_INCLUDED)
#define mfx_cmd_Cnx_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace cmd
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	Cnx::operator < (const Cnx &other) const
{
	bool           inf_flag = false;

	if (_src < other._src)
	{
		inf_flag = true;
	}
	else if (_src == other._src)
	{
		if (_dst < other._dst)
		{
			inf_flag = true;
		}
	}


	return inf_flag;
}



bool	Cnx::operator == (const Cnx &other) const
{
	return (
		   _src == other._src
		&& _dst == other._dst
	);
}



bool	Cnx::operator != (const Cnx &other) const
{
	return (! (*this == other));
}



bool	Cnx::is_valid () const
{
	return (_src.is_valid () && _dst.is_valid ());
}



void	Cnx::invalidate ()
{
	_src.invalidate ();
	_dst.invalidate ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace cmd
}  // namespace mfx



#endif   // mfx_cmd_Cnx_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
