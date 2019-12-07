/*****************************************************************************

        CnxEnd.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_cmd_CnxEnd_CODEHEADER_INCLUDED)
#define mfx_cmd_CnxEnd_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace cmd
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	CnxEnd::operator < (const CnxEnd &other) const
{
	bool           inf_flag = false;

	if (_slot_type < other._slot_type)
	{
		inf_flag = true;
	}
	else if (_slot_type == other._slot_type)
	{
		if (_slot_pos < other._slot_pos)
		{
			inf_flag = true;
		}
		else if (_slot_pos == other._slot_pos)
		{
			if (_pin < other._pin)
			{
				inf_flag = true;
			}
		}
	}

	return inf_flag;
}



bool	CnxEnd::operator == (const CnxEnd &other) const
{
	return (
		   _slot_type == other._slot_type
		&& _slot_pos  == other._slot_pos
		&& _pin       == other._pin
	);
}



bool	CnxEnd::operator != (const CnxEnd &other) const
{
	return (! (*this == other));
}



bool	CnxEnd::is_valid () const
{
	return (_slot_type >= 0);
}



void	CnxEnd::invalidate ()
{
	_slot_type = SlotType_INVALID;
	_slot_pos  = -1;
	_pin       = -1;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace cmd
}  // namespace mfx



#endif   // mfx_cmd_CnxEnd_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
