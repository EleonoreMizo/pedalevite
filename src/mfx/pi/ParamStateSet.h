/*****************************************************************************

        ParamStateSet.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_ParamStateSet_HEADER_INCLUDED)
#define mfx_pi_ParamStateSet_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/BitFieldSparse.h"
#include "mfx/piapi/ParamCateg.h"
#include "mfx/pi/ParamState.h"



namespace fstb
{
namespace util
{
class ObserverInterface;
}
}

namespace mfx
{
namespace pi
{



class ParamDescSet;

class ParamStateSet
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           init (piapi::ParamCateg categ, const ParamDescSet &param_desc_arr);
	void           set_sample_freq (double fs);
	void           clear_buffers ();
	void           set_steady_state ();
	void           set_ramp_time (int index, double t);
	void           set_val (int index, double val);
	void           set_val_nat (const ParamDescSet &param_desc_arr, int index, double val);
	const ParamState &
	               use_state (int index) const;
	ParamState &   use_state (int index);
	void           process_block (int nbr_spl);

	// Convenience functions
	void           add_observer (int index, fstb::util::ObserverInterface &observer);
	double         get_val_beg_nat (int index) const;
	double         get_val_end_nat (int index) const;
	double         get_val_tgt_nat (int index) const;
	bool           check_change (int index);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <ParamState> ParamStateArray;

	piapi::ParamCateg                   // Negative: not initialised yet
	               _categ = piapi::ParamCateg_UNDEFINED;
	ParamStateArray
	               _state_arr;
	fstb::BitFieldSparse
	               _change_flag_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const ParamStateSet &other) const = delete;
	bool           operator != (const ParamStateSet &other) const = delete;

}; // class ParamStateSet



}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/ParamStateSet.hpp"



#endif   // mfx_pi_ParamStateSet_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
