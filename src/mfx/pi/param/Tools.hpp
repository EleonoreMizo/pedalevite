/*****************************************************************************

        Tools.hpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_pi_param_Tools_CODEHEADER_INCLUDED)
#define mfx_pi_param_Tools_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/txt/utf8/Codec8.h"

#include <cassert>



namespace mfx
{
namespace pi
{
namespace param
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T, class U>
std::string	Tools::print_name_bestfit (size_t max_len, const char src_list_0 [], T &metric_obj, U metric_fnc)
{
	size_t         pos_utf8;
	size_t         len_utf8;
	size_t         len_metric;

	cut_str_bestfit (
		pos_utf8, len_utf8, len_metric,
		max_len, src_list_0, '\n',
		metric_obj, metric_fnc
	);

	return (std::string (src_list_0 + pos_utf8, len_utf8));
}



/*
==============================================================================
Name: cut_str_bestfit
Description:
	Cut a label given in different lengthes at the biggest length (in Unicode
	characters) fitting in a specified size.
	Strings are assumed valid UTF-8 sequences.
	The length of each Unicode character is evaluated by a user-provided
	function.
Input parameters:
	- max_len_metric: Maximum length of the string, in the custom metric. > 0.
	- src_list_0: List of labels, as a single string, separated with the
		delimiter character. No delimiter is needed for the last label.
	- delimiter: character delimiting the labels
	- metric_obj: object supporting the member function for size evaluation.
	- metric_fnc: user function taking a Unicode character as argument and
		returning its evaluated size.
		Note: characters are evaluated in order, but each label may not be
		evaluated completely.
Output parameters:
	- pos_utf8: Position of the selected label within the input string.
	- len_utf8: Length of the selected label, in UTF-8 characters. If no
		fitting labels were found, the length correspond to a valid truncated
		label.
	- len_metric: Evaluated size of the (possibly truncated) selected label.
Returns: The best string. The label may be truncated if all the provided ones
	are too long.
Throws: depends on the metric function
==============================================================================
*/

template <class T, class U>
void	Tools::cut_str_bestfit (size_t &pos_utf8, size_t &len_utf8, size_t &len_metric, size_t max_len_metric, const char src_list_0 [], char delimiter, T &metric_obj, U metric_fnc)
{
	assert (max_len_metric > 0);
	assert (src_list_0 != 0);

	pos_utf8   = 0;
	len_utf8   = 0;
	len_metric = 0;

	size_t         sel_label_len_ucs = 0;
	size_t         sel_label_len_utf = 0;
	size_t         sel_label_len_met = 0;
	size_t         sel_label_len_brk = 0; // Metric
	size_t         sel_label_pos     = 0;
	size_t         cur_label_len_ucs = 0;
	size_t         cur_label_len_utf = 0;
	size_t         cur_label_len_met = 0;
	size_t         cur_label_len_brk = 0; // Metric
	size_t         cur_label_pos     = 0;
	size_t         pos               = 0;
	bool           exit_flag         = false;
	do
	{
		const char		c = src_list_0 [pos];
		int            c_len;
		fstb::txt::utf8::Codec8::get_char_seq_len_utf (c_len, c);
		if (c == delimiter || c == '\0')
		{
			if (cur_label_len_ucs > 0)
			{
				if (   (   cur_label_len_ucs > sel_label_len_ucs
				        && cur_label_len_met <= max_len_metric)
				    || (   cur_label_len_ucs < sel_label_len_ucs
				        && sel_label_len_met > max_len_metric))
				{
					sel_label_len_ucs = cur_label_len_ucs;
					sel_label_len_utf = cur_label_len_utf;
					sel_label_len_met = cur_label_len_met;
					sel_label_len_brk = cur_label_len_brk;
					sel_label_pos     = cur_label_pos;
				}
			}

			cur_label_len_ucs = 0;
			cur_label_len_utf = 0;
			cur_label_len_met = 0;
			cur_label_len_brk = 0;
			cur_label_pos     = pos + c_len;

			if (c == '\0')
			{
				exit_flag = true;
			}
		}

		else
		{
			if (cur_label_len_met <= max_len_metric)
			{
				char32_t       c_ucs4;
				fstb::txt::utf8::Codec8::decode_char (c_ucs4, src_list_0 + pos);
				const size_t   met_len = size_t ((metric_obj.*metric_fnc) (c_ucs4));

				if (cur_label_len_met + met_len <= max_len_metric)
				{
					cur_label_len_utf += c_len;
					cur_label_len_brk += met_len;
				}
				cur_label_len_met += met_len;
			}
			++ cur_label_len_ucs;
		}

		pos += c_len;
	}
	while (! exit_flag);

	assert (sel_label_len_utf <= max_len_metric);

	pos_utf8   = sel_label_pos;
	len_utf8   = sel_label_len_utf;
	len_metric = sel_label_len_brk;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace param
}  // namespace pi
}  // namespace mfx



#endif   // mfx_pi_param_Tools_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
