/*****************************************************************************

        StereoLevel.h
        Author: Laurent de Soras, 2002

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_StereoLevel_HEADER_INCLUDED)
#define mfx_dsp_StereoLevel_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{



class StereoLevel
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               StereoLevel ()                         = default;
	               StereoLevel (const StereoLevel &other) = default;
	inline         StereoLevel (float l2l, float l2r, float r2l, float r2r);

	StereoLevel &  operator = (const StereoLevel &other)  = default;

	void inline    set (float l2l, float l2r, float r2l, float r2r);

	inline void    set_l2l (float val);
	inline void    set_l2r (float val);
	inline void    set_r2l (float val);
	inline void    set_r2r (float val);

	float const inline
	               get_l2l () const;
	float const inline
	               get_l2r () const;
	float const inline
	               get_r2l () const;
	float const inline
	               get_r2r () const;

	void inline    mat_mul (const StereoLevel &other);

	StereoLevel inline &
	               operator += (const StereoLevel &other);
	StereoLevel inline &
	               operator += (float val);
	StereoLevel inline &
	               operator -= (const StereoLevel &other);
	StereoLevel inline &
	               operator -= (float val);
	StereoLevel inline &
	               operator *= (const StereoLevel &other);
	StereoLevel inline &
	               operator *= (float val);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	// Please keep this exact order for members (used in BasicMixingSseAlign).
	float          _l2l;    // Left to Left
	float          _l2r;    // Left to Right
	float          _r2l;    // Right to Left
	float          _r2r;    // Right to Right



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const StereoLevel &other) = delete;
	bool           operator != (const StereoLevel &other) = delete;

}; // class StereoLevel



inline bool	operator == (const StereoLevel &lhs, const StereoLevel &rhs);
inline bool	operator != (const StereoLevel &lhs, const StereoLevel &rhs);



}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/StereoLevel.hpp"



#endif   // mfx_dsp_StereoLevel_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
