/*****************************************************************************

        VarBlock.h
        Author: Laurent de Soras, 2020

Call order within a processing block:

set_val (), force_val (), clear_buffers ()
tick ()
get_* ()


--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_ctrl_VarBlock_HEADER_INCLUDED)
#define mfx_dsp_ctrl_VarBlock_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace ctrl
{



class VarBlock
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               VarBlock ()                        = default;
	inline explicit
	               VarBlock (float val);
	               VarBlock (const VarBlock &other)   = default;
	               VarBlock (VarBlock &&other)        = default;
	               ~VarBlock ()                       = default;
	VarBlock &     operator = (const VarBlock &other) = default;
	VarBlock &     operator = (VarBlock &&other)      = default;

	inline void    set_val (float val);
	inline void    force_val (float val);
	inline void    tick (int nbr_spl);
	inline float   get_beg () const;
	inline float   get_end () const;
	inline float   get_tgt () const;
	inline float   get_step () const;
	inline void    clear_buffers ();
	


/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	float          _val_tgt = 0;
	float          _val_beg = 0;
	float          _val_end = 0;
	float          _step    = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const VarBlock &other) const = delete;
	bool           operator != (const VarBlock &other) const = delete;

}; // class VarBlock



}  // namespace ctrl
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/ctrl/VarBlock.hpp"



#endif   // mfx_dsp_ctrl_VarBlock_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
