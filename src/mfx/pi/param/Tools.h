/*****************************************************************************

        Tools.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_param_Tools_HEADER_INCLUDED)
#define mfx_pi_param_Tools_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <string>



namespace mfx
{
namespace pi
{
namespace param
{



class Tools
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static std::string
	               print_name_bestfit (size_t max_len, const char src_list_0 []);
	static std::string
	               extract_longest_str (const char src_list_0 [], char delimiter);
	static std::string
	               join_strings_multi (const char src_list_0 [], char delimiter, std::string pre, std::string post);

	template <class T, class U>
	static std::string
	               print_name_bestfit (size_t max_len, const char src_list_0 [], T &metric_obj, U metric_fnc);

	template <class T, class U>
	static void    cut_str_bestfit (size_t &pos_utf8, size_t &len_utf8, size_t &len_metric, size_t max_len_metric, const char src_list_0 [], char delimiter, T &metric_obj, U metric_fnc);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	virtual        ~Tools ()                              = delete;
	               Tools ()                               = delete;
	               Tools (const Tools &other)             = delete;
	Tools &        operator = (const Tools &other)        = delete;
	bool           operator == (const Tools &other) const = delete;
	bool           operator != (const Tools &other) const = delete;

}; // class Tools



}  // namespace param
}  // namespace pi
}  // namespace mfx



#include "mfx/pi/param/Tools.hpp"



#endif   // mfx_pi_param_Tools_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
