/*****************************************************************************

        SampleDataTplPar.h
        Author: Laurent de Soras, 2019

Template parameter object for SampleData

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_osc_SampleDataTplPar_HEADER_INCLUDED)
#define mfx_dsp_osc_SampleDataTplPar_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace osc
{



template <int NBRT, typename DT = float, long UPRE = 1, long UPOST = 3>
class SampleDataTplPar
{
	static_assert (NBRT > 0, "");
	static_assert (UPRE >= 0, "");
	static_assert (UPOST >= 0, "");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef	DT	DataType;

	static const int  NBR_TABLES  = NBRT;
	static const int  UNROLL_PRE  = UPRE;
	static const int  UNROLL_POST = UPOST;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               SampleDataTplPar ()                               = delete;
	               SampleDataTplPar (const SampleDataTplPar &other)  = delete;
	virtual        ~SampleDataTplPar ()                              = delete;
	SampleDataTplPar &
	               operator = (const SampleDataTplPar &other)        = delete;
	bool           operator == (const SampleDataTplPar &other) const = delete;
	bool           operator != (const SampleDataTplPar &other) const = delete;

}; // class SampleDataTplPar



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/osc/SampleDataTplPar.hpp"



#endif   // mfx_dsp_osc_SampleDataTplPar_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
