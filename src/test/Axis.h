/*****************************************************************************

        Axis.h
        Copyright (c) 2003 Laurent de Soras

--- Legal stuff ---

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*Tab=3***********************************************************************/



#if ! defined (Axis_HEADER_INCLUDED)
#define	Axis_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	<string>



class Axis
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

						Axis ();
	virtual			~Axis () {}

	void				set_scale (double min_val, double max_val);
	void				set_log_scale (bool log_flag);
	void				activate_tick (bool flag);
	void				set_tick_org (double org);
	void				set_tick_dist (double dist);
	void				set_name (const std::string &name);

	void				get_scale (double &min_val, double &max_val) const;
	bool				is_log_scale () const;
	bool				is_tick_active () const;
	double			get_tick_org () const;
	double			get_tick_dist () const;
	std::string		get_name () const;

	double			conv_val_to_pos (double val) const;

	// Tick iteration:
	// for (long t = a.tick_begin (); t != a.tick_end (); ++t)
	long				tick_begin () const;
	long				tick_end () const;
	double			get_tick_val (long it) const;
	double			get_tick_pos (long it) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void				keep_coherent ();

	double			_min_val;	// Minimum value (left or bottom)
	double			_max_val;	// Maximum value (right or top), > _min_val
	double			_tick_org;	// Ref value for one tick
	double			_tick_dist;	// Value between 2 ticks, only for for linear
	std::string		_name;
	bool				_log_flag;	// Log scale
	bool				_tick_flag;	// Tick display



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						Axis (const Axis &other);
	Axis &			operator = (const Axis &other);
	bool				operator == (const Axis &other);
	bool				operator != (const Axis &other);

};	// class Axis



//#include	"Axis.hpp"



#endif	// Axis_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
