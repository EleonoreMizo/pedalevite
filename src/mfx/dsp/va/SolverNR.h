/*****************************************************************************

        SolverNR.h
        Author: Laurent de Soras, 2020

Newton-Raphson equation solver.

Template parameters:

- FNC: function f(x) = 0, where x is the unknown. Requires:
   FNC::FNC (FNC &&) noexcept;
	FNC::DataType;
		The type of handled data
	FNC::DataType FNC::estimate () noexcept;
		Guess a first value x for the iteration, so f(x) is as close as
		possible to 0.
	void FNC::eval (FNC::DataType &y, FNC::DataType &dy, FNC::DataType x) noexcept;
		y and dy (never 0) are the function value and its derivative at x.

- MAXIT: maximum number of iterations. >= 1

- PVF: when the convergence cannot be obained, reuses the previous value.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_SolverNR_HEADER_INCLUDED)
#define mfx_dsp_va_SolverNR_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace va
{



template <typename FNC, int MAXIT = 8, bool PVF = true>
class SolverNR
{
	static_assert (MAXIT > 0, "MAXIT should be > 0.");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  _nbr_max_it = MAXIT;
	static const bool _prev_val_flag = PVF;

	typedef FNC FuncType;
	typedef typename FuncType::DataType DataType;

	               SolverNR ()                                          = default;
	               SolverNR (const SolverNR <FNC, MAXIT, PVF> &other)   = default;
	               SolverNR (SolverNR <FNC, MAXIT, PVF> &&other)        = default;
	SolverNR <FNC, MAXIT, PVF> &
	               operator = (const SolverNR <FNC, MAXIT, PVF> &other) = default;
	SolverNR <FNC, MAXIT, PVF> &
	               operator = (SolverNR <FNC, MAXIT, PVF> &&other)      = default;
	               ~SolverNR () = default;

	explicit       SolverNR (const FuncType &fnc, DataType prec, DataType max_dif = 0.125) noexcept;
	explicit       SolverNR (FuncType &&fnc, DataType prec, DataType max_dif = 0.125) noexcept;
	inline FuncType &
	               use_fnc () noexcept;
	DataType       slove () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	FuncType       _fnc;
	DataType       _prec    = DataType (1e-6);
	DataType       _max_dif = DataType (0.125); // Limits f(x)/f'(x) so the solver don't go too far in case of abrupt slope change.
	DataType       _prev_x  = DataType (0);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const SolverNR <FNC, MAXIT, PVF> &other) const = delete;
	bool           operator != (const SolverNR <FNC, MAXIT, PVF> &other) const = delete;

}; // class SolverNR



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/va/SolverNR.hpp"



#endif   // mfx_dsp_va_SolverNR_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
