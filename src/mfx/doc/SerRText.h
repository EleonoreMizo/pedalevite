/*****************************************************************************

        SerRText.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_doc_SerRText_HEADER_INCLUDED)
#define mfx_doc_SerRText_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/doc/SerRInterface.h"

#include <vector>



namespace mfx
{
namespace doc
{



class SerRText
:	public SerRInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               SerRText ()  = default;
	virtual        ~SerRText () = default;

	void           start (std::string content);
	int            terminate ();

	bool           has_error () const;
	std::string    get_error_msg () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::doc::SerRInterface
	virtual int    do_begin_list (int &nbr_elt);
	virtual int    do_end_list ();
	virtual int    do_read (double &x);
	virtual int    do_read (std::string &s);

	virtual void   do_set_doc_version (int vers);
	virtual int    do_get_doc_version () const;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class ListInfo
	{
	public:
		size_t         _pos;
		int            _nbr_elt;
	};
	class ScanPos
	{
	public:
		size_t         _pos  = 0;
		int            _line = 0;
	};

	enum StrState
	{
		StrState_BEG = 0,
		StrState_CONTENT,
		StrState_ESC,
		StrState_ESC_HEX,
		StrState_END
	};

	void           scan_list_content (ScanPos &spos, int info_index);
	void           skip_white (ScanPos &spos);
	void           scan_flt (ScanPos &spos);
	void           scan_str (ScanPos &spos);
	void           set_error (const ScanPos &spos, const char *msg_0);

	std::string    _content;
	std::string    _err_msg;
	bool           _err_flag = false;

	std::vector <ListInfo>
	               _list_info_arr;

	ScanPos        _spos;
	int            _info_index = 0;
	int            _list_level = 0;

	int            _doc_ver    = -1;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               SerRText (const SerRText &other)          = delete;
	SerRText &     operator = (const SerRText &other)        = delete;
	bool           operator == (const SerRText &other) const = delete;
	bool           operator != (const SerRText &other) const = delete;

}; // class SerRText



}  // namespace doc
}  // namespace mfx



//#include "mfx/doc/SerRText.hpp"



#endif   // mfx_doc_SerRText_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
