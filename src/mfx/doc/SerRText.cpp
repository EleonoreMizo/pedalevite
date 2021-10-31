/*****************************************************************************

        SerRText.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/txt/neutral/ConvDigit.h"
#include "fstb/fnc.h"
#include "mfx/doc/SerRText.h"

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cstdlib>



namespace mfx
{
namespace doc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SerRText::start (std::string content)
{
	_content    = content;
	_err_msg.clear ();
	_err_flag   = false;
	_doc_ver    = -1;

	// Structural parsing: find the number of elements for every list
	_list_info_arr.clear ();
	ScanPos        spos;
	scan_list_content (spos, -1);

	_spos._pos  = 0;
	_spos._line = 0;
	_info_index = 0;
	_list_level = 0;
}



int	SerRText::terminate ()
{
	skip_white (_spos);
	if (! _err_flag)
	{
		if (   _spos._pos < _content.length ()
		    || _list_level > 0
		    || _info_index < int (_list_info_arr.size ()))
		{
			set_error (_spos, "Terminated before the end of the stream");
		}
	}

	return (_err_flag) ? -1 : 0;
}



bool	SerRText::has_error () const
{
	return _err_flag;
}



std::string	SerRText::get_error_msg () const
{
	return _err_msg;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	SerRText::do_begin_list (int &nbr_elt)
{
	nbr_elt = 0;

	skip_white (_spos);
	if (! _err_flag)
	{
		if (_spos._pos >= _content.length ())
		{
			set_error (_spos, "Unexpected end of stream");
		}
		else if (_info_index >= int (_list_info_arr.size ()))
		{
			set_error (_spos, "No more list found");
		}
		else if (   _spos._pos != _list_info_arr [_info_index]._pos
		         || _content [_spos._pos] != '{')
		{
			set_error (_spos, "Not a list");
		}
		else
		{
			nbr_elt = _list_info_arr [_info_index]._nbr_elt;
			++ _spos._pos;
			++ _info_index;
			++ _list_level;
		}
	}

	return (_err_flag) ? -1 : 0;
}



int	SerRText::do_end_list ()
{
	skip_white (_spos);
	if (! _err_flag)
	{
		if (_spos._pos >= _content.length ())
		{
			set_error (_spos, "Unexpected end of stream");
		}
		else if (_list_level <= 0)
		{
			set_error (_spos, "Not in a list");
		}
		else if (_content [_spos._pos] != '}')
		{
			set_error (_spos, "Unexpected request to end the list");
		}
		/*** To do: check that we read the expected number of elements ***/
		else
		{
			++ _spos._pos;
			-- _list_level;
		}
	}

	return (_err_flag) ? -1 : 0;
}



int	SerRText::do_read (double &x)
{
	x = 0;
	skip_white (_spos);
	if (! _err_flag)
	{
		const char     c = _content [_spos._pos];
		if (! isdigit (c) && c != '+' && c != '-' && c != '.' && c != ',')
		{
			set_error (_spos, "Not in a number");
		}
	}
	if (! _err_flag)
	{
		const char *   beg_0 = _content.c_str () + _spos._pos;
		char       *   end_0 = const_cast <char *> (beg_0);
		errno = 0;
		x = strtod (beg_0, &end_0);
		if (end_0 <= beg_0)
		{
			set_error (_spos, "Conversion error");
		}
		else if (errno == ERANGE)
		{
			set_error (_spos, "Number out of range");
		}
		else
		{
			_spos._pos += end_0 - beg_0;
		}
	}

	return (_err_flag) ? -1 : 0;
}



int	SerRText::do_read (std::string &s)
{
	s.clear ();
	skip_white (_spos);
	StrState       state   = StrState_BEG;
	int            esc_len = 0;
	char           esc_c   = '\0';
	while (_spos._pos < _content.length () && ! _err_flag && state != StrState_END)
	{
		const char     c = _content [_spos._pos];
		switch (state)
		{
		case StrState_BEG:
			if (c != '\"')
			{
				set_error (_spos, "Not a string");
			}
			else
			{
				state = StrState_CONTENT;
			}
			break;
		case StrState_CONTENT:
			if (c == '\"')
			{
				state = StrState_END;
			}
			else if (c == '\\')
			{
				state = StrState_ESC;
			}
			else
			{
				s += c;
			}
			break;
		case StrState_ESC:
			if (tolower (c) == 'x')
			{
				state   = StrState_ESC_HEX;
				esc_len = 2;
				esc_c   = '\0';
			}
			else if (c < 32 || c >= 127)
			{
				set_error (_spos, "Wrong escaped character");
			}
			else
			{
				s += c;
				state = StrState_CONTENT;
			}
			break;
		case StrState_ESC_HEX:
			{
				const int      d =
					fstb::txt::neutral::ConvDigit::conv_char_to_digit (c);
				if (d >= 16)
				{
					set_error (_spos, "Wrong hex sequence");
				}
				esc_c = static_cast <char> ((esc_c << 4) + d);
				-- esc_len;
				if (esc_len <= 0 && ! _err_flag)
				{
					s += esc_c;
					state = StrState_CONTENT;
				}
			}
			break;
		case StrState_END:
			// Nothing
			break;
		default:
			assert (false);
			break;
		}

		++ _spos._pos;
	}

	if (! _err_flag && state != StrState_END)
	{
		set_error (_spos, "Unexpected end of string");
	}

	return (_err_flag) ? -1 : 0;
}



void	SerRText::do_set_doc_version (int vers)
{
	assert (_doc_ver < 0);

	_doc_ver = vers;
}



int	SerRText::do_get_doc_version () const
{
	return _doc_ver;
}



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	SerRText::scan_list_content (ScanPos &spos, int info_index)
{
	skip_white (spos);

	int            nbr_elt   = 0;
	bool           cont_flag = true;
	while (spos._pos < _content.length () && cont_flag && ! _err_flag)
	{
		const char     c = _content [spos._pos];
		if (c == '\"')
		{
			scan_str (spos);
			++ nbr_elt;
		}
		else if (isdigit (c) || c == '+' || c == '-' || c == '.' || c == ',')
		{
			scan_flt (spos);
			++ nbr_elt;
		}
		else if (c == '{')
		{
			const int      index_new = int (_list_info_arr.size ());
			_list_info_arr.push_back ({ spos._pos, 0 });
			++ spos._pos;
			scan_list_content (spos, index_new);
			++ nbr_elt;
		}
		else if (c == '}')
		{
			if (info_index < 0)
			{
				set_error (spos, "Unexpected closing curly brace");
			}
			else
			{
				++ spos._pos;
				cont_flag = false;
			}
		}
		else
		{
			set_error (spos, "Unexpected character");
		}

		skip_white (spos);
	}

	if (! _err_flag && info_index >= 0)
	{
		_list_info_arr [info_index]._nbr_elt = nbr_elt;
	}
}



void	SerRText::skip_white (ScanPos &spos)
{
	while (spos._pos < _content.length () && isspace (_content [spos._pos]))
	{
		if (_content [spos._pos] == '\n')
		{
			++ spos._line;
		}
		++ spos._pos;
	}
}



void	SerRText::scan_flt (ScanPos &spos)
{
	// Not a strong check, but enough for our needs
	while (   spos._pos < _content.length ()
	       && (   isdigit (_content [spos._pos])
	           || tolower (_content [spos._pos]) == 'e'
	           || _content [spos._pos] == '.'
	           || _content [spos._pos] == ',' // Not sure about the locale
	           || _content [spos._pos] == '+'
	           || _content [spos._pos] == '-'))
	{
		++ spos._pos;
	}
}



// Points on the opening double quote
// Exits after the closing double quote
void	SerRText::scan_str (ScanPos &spos)
{
	StrState       state = StrState_BEG;
	while (spos._pos < _content.length () && ! _err_flag && state != StrState_END)
	{
		const char     c = _content [spos._pos];
		switch (state)
		{
		case StrState_BEG:
			if (c != '\"')
			{
				set_error (_spos, "Not a string");
			}
			else
			{
				state = StrState_CONTENT;
			}
			break;
		case StrState_CONTENT:
			if (c == '\"')
			{
				state = StrState_END;
			}
			else if (c == '\\')
			{
				state = StrState_ESC;
			}
			break;
		case StrState_ESC:
			if (c < 32 || c >= 127)
			{
				set_error (_spos, "Wrong escaped character");
			}
			else
			{
				state = StrState_CONTENT;
			}
			break;
		case StrState_END:
			// Nothing
			break;
		default:
			assert (false);
			break;
		}

		++ spos._pos;
	}

	if (! _err_flag && state != StrState_END)
	{
		set_error (spos, "Unexpected end of string");
	}
}



void	SerRText::set_error (const ScanPos &spos, const char *msg_0)
{
	char           txt_0 [255+1];
	fstb::snprintf4all (txt_0, sizeof (txt_0),
		"%s at line %d (stream position %ld)",
		msg_0,
		spos._line + 1,
		long (spos._pos)
	);
	_err_flag = true;
	_err_msg  = txt_0;
}



}  // namespace doc
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
