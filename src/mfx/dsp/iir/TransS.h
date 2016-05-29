/*****************************************************************************

        TransS.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_iir_TransS_HEADER_INCLUDED)
#define mfx_dsp_iir_TransS_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <complex>



namespace mfx
{
namespace dsp
{
namespace iir
{



class TransS
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef std::complex <double> Cplx;

	static void    conv_roots_real_to_poly_1 (double poly [2], double k, double root);
	static void    conv_roots_real_to_poly_2 (double poly [3], double k, double root_1, double root_2);
	static void    conv_roots_cplx_to_poly_2 (double poly [3], double k, const Cplx &root);

	static void    conv_lp_pz_to_pb_pz (Cplx &pz_bp_1, Cplx &pz_bp_2, const Cplx &pz_lp, double bw);
	static void    conv_lp_ap_to_pb_biq (double b_1 [3], double a_1 [3], double b_2 [3], double a_2 [3], const Cplx &lp_pole, double bw);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TransS ()                               = delete;
	               TransS (const TransS &other)            = delete;
	virtual        ~TransS ()                              = delete;
	TransS &       operator = (const TransS &other)        = delete;
	bool           operator == (const TransS &other) const = delete;
	bool           operator != (const TransS &other) const = delete;

}; // class TransS



}  // namespace iir
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/iir/TransS.hpp"



#endif   // mfx_dsp_iir_TransS_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
