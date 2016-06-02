/*****************************************************************************

        MapPiecewiseLinLog.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_param_MapPiecewiseLinLog_HEADER_INCLUDED)
#define mfx_pi_param_MapPiecewiseLinLog_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



namespace mfx
{
namespace pi
{
namespace param
{



class MapPiecewiseLinLog
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               MapPiecewiseLinLog ();
	virtual        ~MapPiecewiseLinLog () = default;

	void           config (double val_min, double val_max);
	double         conv_norm_to_nat (double norm) const;
	double         conv_nat_to_norm (double nat) const;

	void           set_first_value (double nat);
	void           add_segment (double norm, double nat, bool log_flag);
	bool				is_ok () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class Segment
	{
	public:
		double         _val_l_nrm;
		double         _val_l_nat;
		double         _val_u_nrm; // 0 = segment is just the first value.
		double         _val_u_nat;
		double         _a_no2na;
		double         _b_no2na;
		double         _a_na2no;
		bool           _log_flag;
		bool           _neg_flag;  // For log segments
	};

	typedef std::vector <Segment> SegList;

	SegList        _seg_list;
	double         _val_min;
	double         _val_max;




/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               MapPiecewiseLinLog (const MapPiecewiseLinLog &other) = delete;
	MapPiecewiseLinLog &
	               operator = (const MapPiecewiseLinLog &other)         = delete;
	bool           operator == (const MapPiecewiseLinLog &other) const  = delete;
	bool           operator != (const MapPiecewiseLinLog &other) const  = delete;

}; // class MapPiecewiseLinLog



}  // namespace param
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/param/MapPiecewiseLinLog.hpp"



#endif   // mfx_pi_param_MapPiecewiseLinLog_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
