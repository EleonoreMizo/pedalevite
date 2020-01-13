/*****************************************************************************

        Remez.h
        Author: Laurent de Soras, 2019

Based on the Jake Janovetz' implementation

Algorithm may not converge if there are too much coefficients for too loose
specification. Ex: Half-band LPF with 129 coefs / 0.1 TBW

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_fir_Remez_HEADER_INCLUDED)
#define mfx_dsp_fir_Remez_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/fir/RemezPoint.h"

#include <vector>



namespace mfx
{
namespace dsp
{
namespace fir
{



class Remez
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Type
	{
		Type_BANDPASS = 0,
		Type_DIFFERENTIATOR,
		Type_HILBERT,

		Type_NBR_ELT
	};

	typedef	std::vector <RemezPoint>	RemezSpec;
	typedef	std::vector <double>	CoefList;

	int            compute_coefs (double coef_list_ptr [], int len, const RemezSpec &spec, Type type);
	int            compute_coefs (CoefList &coef_list, int len, const RemezSpec &spec, Type type);
	void           free_resources ();

	static int     estimate_order (double ripple_db, double atten_db, double trans_band);
	static double  compute_ripple_ratio (double ripple_db, double atten_db);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  GRID_DENSITY = 16;
	static const int  MAX_NBR_IT   = 40;

	enum Symmetry
	{
		Symmetry_NEG = 0,
		Symmetry_POS,

		Symmetry_NBR_ELT
	};

	typedef	std::vector <double>	FltArray;
	typedef	std::vector <int>	IntArray;

	void           create_dense_grid (int r, int nbr_coefs, const RemezSpec &spec, int gridsize, Symmetry symmetry);
	void           initial_guess (int r, int gridsize);
	void           calc_params (int r);
	double         compute_a (double freq, int r) const;
	void           calc_error (int r, int gridsize);
	void           search (int r, int gridsize);
	void           freq_sample (int N, double h [], int symm);
	bool           is_done (int r) const;

	FltArray       _grid;
	FltArray       _w;
	FltArray       _d;
	FltArray       _e;
	IntArray       _ext;
	FltArray       _taps;
	FltArray       _x;
	FltArray       _y;
	FltArray       _ad;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const Remez &other) const = delete;
	bool           operator != (const Remez &other) const = delete;

}; // class Remez



}  // namespace fir
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/fir/Remez.hpp"



#endif   // mfx_dsp_fir_Remez_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
