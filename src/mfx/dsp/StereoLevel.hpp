/*****************************************************************************

        StereoLevel.hpp
        Author: Laurent de Soras, 2002

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_StereoLevel_CODEHEADER_INCLUDED)
#define mfx_dsp_StereoLevel_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



StereoLevel::StereoLevel (float l2l, float l2r, float r2l, float r2r) noexcept
:	_l2l (l2l)
,	_l2r (l2r)
,	_r2l (r2l)
,	_r2r (r2r)
{
	// Nothing
}



void	StereoLevel::set (float l2l, float l2r, float r2l, float r2r) noexcept
{
	_l2l = l2l;
	_l2r = l2r;
	_r2l = r2l;
	_r2r = r2r;
}



void	StereoLevel::set_l2l (float val) noexcept
{
	_l2l = val;
}



void	StereoLevel::set_l2r (float val) noexcept
{
	_l2r = val;
}



void	StereoLevel::set_r2l (float val) noexcept
{
	_r2l = val;
}



void	StereoLevel::set_r2r (float val) noexcept
{
	_r2r = val;
}



float	StereoLevel::get_l2l () const noexcept
{
	return _l2l;
}



float	StereoLevel::get_l2r () const noexcept
{
	return _l2r;
}



float	StereoLevel::get_r2l () const noexcept
{
	return _r2l;
}



float	StereoLevel::get_r2r () const noexcept
{
	return _r2r;
}



/*
==============================================================================
Name: mat_mul
Description:
	Multiplie *this avec un autre objet StereoLevel, de facon a n'en former
	plus qu'un qui servirait a effectuer un mixage equivalent a la cascade de
	deux effectuer un mixage equivalent a la cascade de deux mixages bases sur
	ces niveaux.
Input parameters:
	- other: Matrice de niveaux correspondant au premier des mixages.
Throws: Nothing
==============================================================================
*/

void	StereoLevel::mat_mul (const StereoLevel &other) noexcept
{
	const float	t_l2r = _l2r * other._l2l + _r2r * other._l2r;
	const float	t_r2l = _l2l * other._r2l + _r2l * other._r2r;
	_l2l = _l2l * other._l2l + _r2l * other._l2r;
	_r2r = _l2r * other._r2l + _r2r * other._r2r;
	_l2r = t_l2r;
	_r2l = t_r2l;
}



StereoLevel &	StereoLevel::operator += (const StereoLevel &other) noexcept
{
	_l2l += other._l2l;
	_l2r += other._l2r;
	_r2l += other._r2l;
	_r2r += other._r2r;

	return *this;
}



StereoLevel &	StereoLevel::operator += (float val) noexcept
{
	_l2l += val;
	_l2r += val;
	_r2l += val;
	_r2r += val;

	return *this;
}



StereoLevel &	StereoLevel::operator -= (const StereoLevel &other) noexcept
{
	_l2l -= other._l2l;
	_l2r -= other._l2r;
	_r2l -= other._r2l;
	_r2r -= other._r2r;

	return *this;
}



StereoLevel &	StereoLevel::operator -= (float val) noexcept
{
	_l2l -= val;
	_l2r -= val;
	_r2l -= val;
	_r2r -= val;

	return *this;
}



StereoLevel &	StereoLevel::operator *= (const StereoLevel &other) noexcept
{
	_l2l *= other._l2l;
	_l2r *= other._l2r;
	_r2l *= other._r2l;
	_r2r *= other._r2r;

	return *this;
}



StereoLevel &	StereoLevel::operator *= (float val) noexcept
{
	_l2l *= val;
	_l2r *= val;
	_r2l *= val;
	_r2r *= val;

	return *this;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ GLOBAL OPERATOR DEFINITIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	operator == (const StereoLevel &lhs, const StereoLevel &rhs) noexcept
{
	return (
		   lhs.get_l2l () == rhs.get_l2l ()
		&& lhs.get_l2r () == rhs.get_l2r ()
		&& lhs.get_r2l () == rhs.get_r2l ()
		&& lhs.get_r2r () == rhs.get_r2r ()
	);
}



bool	operator != (const StereoLevel &lhs, const StereoLevel &rhs) noexcept
{
	return (! (lhs == rhs));
}



}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_StereoLevel_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
