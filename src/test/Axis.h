/*****************************************************************************

        Axis.h
        Author: Laurent de Soras, 2003

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (Axis_HEADER_INCLUDED)
#define	Axis_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <string>



class Axis
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Axis ();
	virtual        ~Axis () {}

	void           set_scale (double min_val, double max_val);
	void           set_log_scale (bool log_flag);
	void           activate_tick (bool flag);
	void           set_tick_org (double org);
	void           set_tick_dist (double dist);
	void           set_name (const std::string &name);

	void           get_scale (double &min_val, double &max_val) const;
	bool           is_log_scale () const;
	bool           is_tick_active () const;
	double         get_tick_org () const;
	double         get_tick_dist () const;
	std::string    get_name () const;

	double         conv_val_to_pos (double val) const;

	// Tick iteration:
	// for (long t = a.tick_begin (); t != a.tick_end (); ++t)
	long           tick_begin () const;
	long           tick_end () const;
	double         get_tick_val (long it) const;
	double         get_tick_pos (long it) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void				keep_consistent ();

	double         _min_val;   // Minimum value (left or bottom)
	double         _max_val;   // Maximum value (right or top), > _min_val
	double         _tick_org;  // Ref value for one tick
	double         _tick_dist; // Value between 2 ticks, only for for linear
	std::string    _name;
	bool           _log_flag;  // Log scale
	bool           _tick_flag; // Tick display



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Axis (const Axis &other);
	Axis &         operator = (const Axis &other);
	bool           operator == (const Axis &other);
	bool           operator != (const Axis &other);

};	// class Axis



//#include	"Axis.hpp"



#endif	// Axis_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
