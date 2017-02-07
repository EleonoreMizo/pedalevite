/*****************************************************************************

        HelperDispNum.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_param_HelperDispNum_HEADER_INCLUDED)
#define mfx_pi_param_HelperDispNum_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	<string>



namespace mfx
{
namespace pi
{
namespace param
{



class HelperDispNum
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum Type
	{
		Type_FLOAT = 0,
		Type_INT,
		Type_NOTE,
		Type_FRAC,
		Type_RATIO,
		Type_RATIO_INV,

		Type_NBR_ELT
	};

	enum Preset
	{
		Preset_FLOAT_STD,
		Preset_FLOAT_PERCENT,
		Preset_FLOAT_MILLI,
		Preset_FLOAT_MICRO,
		Preset_DB,
		Preset_DB_FROM_LOG2,
		Preset_INT_BASE_0,
		Preset_INT_BASE_1,
		Preset_MIDI_NOTE,
		Preset_MULT_SEMITONES,
		Preset_MULT_CENTS,
		Preset_FREQ_NOTE,
		Preset_FRAC_STD,
		Preset_RATIO_STD,
		Preset_RATIO_INV,

		Preset_NBR_ELT
	};

	enum Err
	{
		Err_OK = 0,
		Err_CANNOT_CONVERT_VALUE = -1000
	};

	               HelperDispNum ()  = default;
	virtual        ~HelperDispNum () = default;

	void           set_type (Type type);
	void           set_log (bool flag);
	void           set_preset (Preset preset);

	void           set_print_format (const char *format_0);
	void           set_range (double val_min, double val_max);
	void           set_scale (double scale);
	void           set_offset (double offset);
	bool           is_ok () const;

	double         get_val_min () const;
	double         get_val_max () const;

	int            conv_to_str (double val, char txt_0 [], long max_len) const;
	int            conv_from_str (const char txt_0 [], double &val) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	int            detect_note (const char *txt_0, const char * &stop_0) const;
	std::string    print_frac (double val, long max_len) const;

	static bool    find_infinite (const char *txt_0);
	static bool    find_frac (int &num, int &den, double frac_val, long max_len);
	static bool    scan_frac (const char txt_0 [], double &val);
	static bool    scan_ratio (const char txt_0 [], double &val, Type type);

	double         _scale        = 1;
	double         _offset       = 0;
	double         _val_max      = 0;
	double         _val_min      = 0;
	std::string    _print_format = "%f";
	Type           _type         = Type_FLOAT;
	bool           _log_flag     = false;

	static const char *
	               _note_0_list [12];
	static const char *
	               _inf_0_list [2];



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               HelperDispNum (const HelperDispNum &other)     = delete;
	HelperDispNum &
	               operator = (const HelperDispNum &other)        = delete;
	bool           operator == (const HelperDispNum &other) const = delete;
	bool           operator != (const HelperDispNum &other) const = delete;

}; // class HelperDispNum



}  // namespace param
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/param/HelperDispNum.hpp"



#endif   // mfx_pi_param_HelperDispNum_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
