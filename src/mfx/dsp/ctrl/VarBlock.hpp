/*****************************************************************************

        VarBlock.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_ctrl_VarBlock_CODEHEADER_INCLUDED)
#define mfx_dsp_ctrl_VarBlock_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace ctrl
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



VarBlock::VarBlock (float val) noexcept
:	_val_tgt (val)
,	_val_beg (val)
,	_val_end (val)
,	_step (0)
{
	// Nothing
}



void	VarBlock::set_val (float val) noexcept
{
	_val_tgt = val;
}



void	VarBlock::force_val (float val) noexcept
{
	set_val (val);
	clear_buffers ();
}



void	VarBlock::tick (int nbr_spl) noexcept
{
	assert (nbr_spl > 0);

	_val_beg = _val_end;
	_val_end = _val_tgt;
	_step    = (_val_end - _val_beg) * fstb::rcp_uint <float> (nbr_spl);
}



float	VarBlock::get_beg () const noexcept
{
	return _val_beg;
}



float	VarBlock::get_end () const noexcept
{
	return _val_end;
}



float	VarBlock::get_tgt () const noexcept
{
	return _val_tgt;
}



float	VarBlock::get_step () const noexcept
{
	return _step;
}



void	VarBlock::clear_buffers () noexcept
{
	_val_beg = _val_tgt;
	_val_end = _val_tgt;
	_step    = 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_ctrl_VarBlock_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
