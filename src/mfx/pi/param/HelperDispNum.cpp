/*****************************************************************************

        HelperDispNum.cpp
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/txt/utf8/fnc.h"
#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/pi/param/HelperDispNum.h"

#include <algorithm>

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>



namespace mfx
{
namespace pi
{
namespace param
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	HelperDispNum::set_type (Type type)
{
	assert (type >= 0);
	assert (type < Type_NBR_ELT);

	_type = type;
}



// Log scale allows null values (displayed as minus infinity)
void	HelperDispNum::set_log (bool flag)
{
	_log_flag = flag;
}



void	HelperDispNum::set_preset (Preset preset)
{
	assert (preset >= 0);
	assert (preset < Preset_NBR_ELT);

	switch (preset)
	{
	case Preset_FLOAT_STD:
		_type         = Type_FLOAT;
		_log_flag     = false;
		_scale        = 1;
		_offset       = 0;
		break;

	case Preset_FLOAT_PERCENT:
		_type         = Type_FLOAT;
		_log_flag     = false;
		_scale        = 100;
		_offset       = 0;
		break;

	case Preset_FLOAT_MILLI:
		_type         = Type_FLOAT;
		_log_flag     = false;
		_scale        = 1000;
		_offset       = 0;
		break;

	case Preset_FLOAT_MICRO:
		_type         = Type_FLOAT;
		_log_flag     = false;
		_scale        = 1'000'000;
		_offset       = 0;
		break;

	case Preset_DB:
		_type         = Type_FLOAT;
		_log_flag     = true;
		_scale        = 20 / fstb::LN10;
		_offset       = 0;
		break;

	case Preset_DB_FROM_LOG2:
		_type         = Type_FLOAT;
		_log_flag     = false;
		_scale        = 20 * fstb::LOG10_2;
		_offset       = 0;
		break;

	case Preset_INT_BASE_0:
		_type         = Type_INT;
		_log_flag     = false;
		_scale        = 1;
		_offset       = 0;
		break;

	case Preset_INT_BASE_1:
		_type         = Type_INT;
		_log_flag     = false;
		_scale        = 1;
		_offset       = 1;
		break;

	case Preset_MIDI_NOTE:
		_type         = Type_NOTE;
		_log_flag     = false;
		_scale        = 1;
		_offset       = 0;
		break;

	case Preset_MULT_SEMITONES:
		_type         = Type_FLOAT;
		_log_flag     = true;
		_scale        = 12 / fstb::LN2;
		_offset       = 0;
		break;

	case Preset_MULT_CENTS:
		_type         = Type_FLOAT;
		_log_flag     = true;
		_scale        = 1200 / fstb::LN2;
		_offset       = 0;
		break;

	case Preset_FREQ_NOTE:
		_type         = Type_NOTE;
		_log_flag     = true;
		_scale        = 12 / fstb::LN2;
		_offset       = 69 - log (440.0) * _scale;	// MIDI note 69 is A440
		break;

	case Preset_FRAC_STD:
		_type         = Type_FRAC;
		_log_flag     = false;
		_scale        = 1;
		_offset       = 0;
		break;

	case Preset_RATIO_STD:
		_type         = Type_RATIO;
		_log_flag     = false;
		_scale        = 1;
		_offset       = 0;
		break;

	case Preset_RATIO_INV:
		_type         = Type_RATIO_INV;
		_log_flag     = false;
		_scale        = 1;
		_offset       = 0;
		break;

	default:
		assert (false);
		break;
	}
}


void	HelperDispNum::set_print_format (const char *format_0)
{
	assert (format_0 != nullptr);
	assert (format_0 [0] != '\0');

	_print_format = format_0;
}



void	HelperDispNum::set_range (double val_min, double val_max)
{
	assert (val_min < val_max);

	_val_min = val_min;
	_val_max = val_max;
}



void	HelperDispNum::set_scale (double scale)
{
	assert (scale != 0);

	_scale = scale;
}



void	HelperDispNum::set_offset (double offset)
{
	assert (offset != 0);

	_offset = offset;
}



bool	HelperDispNum::is_ok () const
{
	return (
		   _val_min < _val_max
		&& (! _log_flag || _val_min >= 0)
	);
}



double	HelperDispNum::get_val_min () const
{
	assert (is_ok ());

	return _val_min;
}



double	HelperDispNum::get_val_max () const
{
	assert (is_ok ());

	return _val_max;
}



int	HelperDispNum::conv_to_str (double val, char txt_0 [], long max_len) const
{
	assert (is_ok ());
	assert (val >= _val_min);
	assert (val <= _val_max);
	assert (txt_0 != nullptr);
	assert (max_len > 0);

	char           tmp_0 [1023+1];
	char           tmp2_0 [1023+1];
	bool           log_zero_flag = false;

	double         val_p = val;
	if (_log_flag)
	{
		if (val == 0)
		{
			log_zero_flag = true;
		}

		else
		{
			assert (val > 0);
			val_p = log (val);
		}
	}

	if (log_zero_flag)
	{
		const char *   sign_0 = (_scale < 0) ? "+" : "-";
		fstb::txt::utf8::strncpy_0 (txt_0, sign_0, max_len + 1);
		fstb::txt::utf8::strncat_0 (txt_0, _inf_0_list [0], max_len + 1);
	}

	else if (   (_type == Type_RATIO || _type == Type_RATIO_INV)
	         && val_p == 0)
	{
		const char *   val_0 = "0";
		if (_type == Type_RATIO_INV)
		{
			val_0 = _inf_0_list [0];
		}
		fstb::txt::utf8::strncpy_0 (txt_0, val_0, max_len + 1);
	}

	else
	{
		val_p *= _scale;
		val_p += _offset;

		tmp_0 [0] = '\0';

		bool           ratio_l_flag = false;
		if (_type == Type_RATIO || _type == Type_RATIO_INV)
		{
			ratio_l_flag = (val_p < 1);
			if (ratio_l_flag ^ (_type == Type_RATIO_INV))
			{
				fstb::txt::utf8::strncat_0 (tmp_0, "1:", sizeof (tmp_0));
			}
		}

		tmp2_0 [0] = '\0';

		switch (_type)
		{
		case	Type_FLOAT:
			fstb::snprintf4all (
				tmp2_0, sizeof (tmp2_0),
				_print_format.c_str (), val_p
			);
			break;

		case	Type_INT:
			fstb::snprintf4all (
				tmp2_0, sizeof (tmp2_0),
				_print_format.c_str (), double (fstb::round_int64 (val_p))
			);
			break;

		case	Type_NOTE:
			{
				const auto     cents_tot = fstb::round_int (val_p * 100);
				const auto     midi_both = fstb::divmod_floor (cents_tot, 100);
				auto           midi_note = midi_both [0];
				auto           cents     = midi_both [1];
				// > and not >= because we prefer displaying "+50" instead of
				// "-50" for pitches close to a quarter-tone.
				if (cents > +50)
				{
					cents -= 100;
					++ midi_note;
				}
				const auto     noct_both = fstb::divmod_floor (midi_note, 12);
				const auto     octave    = noct_both [0];
				const auto     note      = noct_both [1];
				char           tmp3_0 [1023+1];
				tmp3_0 [0] = '\0';
				if (cents != 0)
				{
					assert (std::abs (cents) < 100);
					fstb::snprintf4all (tmp3_0, sizeof (tmp3_0), "%+03d", cents);
				}
				fstb::snprintf4all (
					tmp2_0, sizeof (tmp2_0), "%2s%d%s",
					_note_0_list [note], octave, tmp3_0
				);
			}
			break;

		case	Type_FRAC:
			{
				const std::string tmp = print_frac (val_p, max_len);
				fstb::txt::utf8::strncpy_0 (tmp2_0, tmp.c_str (), sizeof (tmp2_0));
			}
			break;

		case	Type_RATIO:
		case	Type_RATIO_INV:
			{
				double         ratio = fabs (val_p);
				if (ratio_l_flag)
				{
					assert (ratio > 0);
					ratio = 1 / ratio;
				}
				assert (ratio >= 1);
				fstb::snprintf4all (
					tmp2_0, sizeof (tmp2_0),
					_print_format.c_str (), ratio
				);
			}
			break;

		default:
			assert (false);
			break;
		}

		fstb::txt::utf8::strncat_0 (tmp_0, tmp2_0, sizeof (tmp_0));

		if (_type == Type_RATIO || _type == Type_RATIO_INV)
		{
			if ((! ratio_l_flag) ^ (_type == Type_RATIO_INV))
			{
				fstb::txt::utf8::strncat_0 (tmp_0, ":1", sizeof (tmp_0));
			}
		}

		fstb::txt::utf8::strncpy_0 (txt_0, tmp_0, max_len + 1);
	}

	return Err_OK;
}



int	HelperDispNum::conv_from_str (const char txt_0 [], double &val) const
{
	assert (is_ok ());
	assert (txt_0 != nullptr);

	int            ret_val = Err_OK;

	bool           log_zero_flag = false;
	if (_log_flag)
	{
		log_zero_flag = find_infinite (txt_0);
	}

	if (log_zero_flag)
	{
		// We are too lazy to check the infinity sign...
		val = 0;
	}

	else
	{
		double         val_p = 0;

		switch (_type)
		{
		case Type_FLOAT:
			{
				char *         end_ptr;
				val_p = strtod (txt_0, &end_ptr);
				if (end_ptr == txt_0 || fabs (val_p) == HUGE_VAL)
				{
					ret_val = Err_CANNOT_CONVERT_VALUE;
				}
			}
			break;

		case Type_INT:
			{
				char *            end_0   = nullptr;
				const long long   val_int = strtoll (txt_0, &end_0, 10);
				if (   end_0 == txt_0
				    || ((   val_int == LLONG_MAX
				         || val_int == LLONG_MIN) && errno == ERANGE))
				{
					ret_val = Err_CANNOT_CONVERT_VALUE;
				}
				else
				{
					val_p = double (val_int);
				}
			}
			break;

		case Type_NOTE:
			{
				const char *   octave_0;
				const int      note = detect_note (txt_0, octave_0);
				if (note < 0)
				{
					ret_val = Err_CANNOT_CONVERT_VALUE;
				}

				else
				{
					char *         end_0;
					const long     octave = strtol (octave_0, &end_0, 0);
					if (end_0 == octave_0 || octave == LONG_MIN || octave == LONG_MAX)
					{
						ret_val = Err_CANNOT_CONVERT_VALUE;
					}
					else
					{
						const long           midi_note = octave * 12 + note;
						val_p = double (midi_note);
						const char * const   cents_0   = end_0;
						long                 cents     = strtol (cents_0, &end_0, 0);
						if (end_0 == cents_0)
						{
							cents = 0;
						}
						else if (octave == LONG_MIN || octave == LONG_MAX)
						{
							ret_val = Err_CANNOT_CONVERT_VALUE;
						}
						val_p += double (cents) / 100.f;
					}

					// Checks if there is garbage between end_0 and the actual
					// end of the string
					if (ret_val == Err_OK)
					{
						const auto     c = *end_0;
						if (c != '\0' && ! isspace (c))
						{
							ret_val = Err_CANNOT_CONVERT_VALUE;
						}
					}
				}
			}
			break;

		case Type_FRAC:
			if (! scan_frac (txt_0, val_p))
			{
				ret_val = Err_CANNOT_CONVERT_VALUE;
			}
			break;

		case Type_RATIO:
		case Type_RATIO_INV:
			if (! scan_ratio (txt_0, val_p, _type))
			{
				ret_val = Err_CANNOT_CONVERT_VALUE;
			}
			break;

		default:
			assert (false);
			ret_val = Err_CANNOT_CONVERT_VALUE;
			break;
		}

		if (ret_val == Err_OK)
		{
			val_p -= _offset;
			val_p /= _scale;

			val = val_p;
			if (_log_flag)
			{
				val = exp (val_p);
			}
		}
	}

	if (ret_val == Err_OK)
	{
		val = fstb::limit (val, _val_min, _val_max);
	}

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



// Returns -1 if the note cannot be found.
int	HelperDispNum::detect_note (const char *txt_0, const char * &stop_0) const
{
	assert (txt_0 != nullptr);

	int            note = -1;

	// Goes backward to make sure we catch the notes with a sharp.
	for (int pos = 12-1; pos >= 0 && note < 0; --pos)
	{
		const char *   test_0 = _note_0_list [pos];
		stop_0 = strstr (txt_0, test_0);
		if (stop_0 != nullptr)
		{
			stop_0 += strlen (test_0);
			note = pos;
		}
	}

	return note;
}



std::string	HelperDispNum::print_frac (double val, long max_len) const
{
	const char *   delim_0 = "/";
	const long     delim_len = long (strlen (delim_0));

	// Separates int and frac values
	const double   val_abs = fabs (val);
	long 	         val_abs_int  = fstb::floor_int_accurate (val);
	double         val_abs_frac = val_abs - val_abs_int;

	// Finds the fraction
	int            num        = 0;
	int            den        = 1;
	bool           retry_flag = false;
	do
	{
		long           frac_len = max_len;
		if (val_abs_int > 0)
		{
			const int      nbr_dgt_int =
				1 + fstb::floor_int_accurate (log10 (double (val_abs_int)));
			frac_len -= nbr_dgt_int;
		}
		frac_len -= delim_len;

		find_frac (num, den, val_abs_frac, frac_len);

		if (num < den)
		{
			retry_flag = false;
		}
		else
		{
			++ val_abs_int;
			val_abs_frac = 0;
			retry_flag = true;
		}
	}
	while (retry_flag);

	std::string    result;
	char           tmp_0 [1023+1];

	// Prints the integer part
	bool           int_flag = false;
	if (val_abs_int > 0 || num == 0)
	{
		const long     val_int = (val < 0) ? -val_abs_int : val_abs_int;
		fstb::snprintf4all (tmp_0, sizeof (tmp_0), "%d", val_int);
		result  += tmp_0;
		int_flag = true;
	}

	// Prints the fraction
	if (num > 0)
	{
		if (int_flag)
		{
			result += " ";
		}
		fstb::snprintf4all (tmp_0, sizeof (tmp_0), "%d%s%d", num, delim_0, den);
		result += tmp_0;
	}

	return result;
}



bool	HelperDispNum::find_infinite (const char *txt_0)
{
	bool           inf_flag = false;
	const int      nbr_inf  = sizeof (_inf_0_list) / sizeof (_inf_0_list [0]);
	for (int k = 0; k < nbr_inf && ! inf_flag; ++k)
	{
		const char *   test_0 = strstr (txt_0, _inf_0_list [k]);
		inf_flag = (test_0 != nullptr);
	}

	return inf_flag;
}



bool	HelperDispNum::find_frac (int &num, int &den, double frac_val, long max_len)
{
	assert (frac_val >= 0);
	assert (frac_val <= 1);	// Should be < 1, but we have to tolerate rounding errors.
	assert (max_len > 0);

	bool           fit_flag = true;

	num = 1;
	den = 0;

	// Numerator is always lower than denominator.
	// Logically rounded towards minus infinity.
	const long     max_len_num = max_len / 2;
	const long     max_num = fstb::ipowp (
		10L,
		fstb::limit (max_len_num, 1L, 4L)
	) - 1;
	const long     max_den = fstb::ipowp (
		10L,
		fstb::limit (max_len - max_len_num, 1L, 4L)
	) - 1;

	// We have no room enough
	if (max_len_num < 1)
	{
		fit_flag = false;
	}

	int            best_num = 0;
	int            best_den = 1;
	double         best_err = 1.0;

	static const int  den_list [] =
	{
		2, 3,
		4, 5, 6, 7,
		8, 9, 10, 12, 14,
		16, 20, 24,
		32, 48,
		64, 96,
		128, 192,
		256
	};
	const int      nbr_den = sizeof (den_list) / sizeof (den_list [0]);

	bool           cont_flag = true;
	for (int den_index = 0; den_index < nbr_den && cont_flag; ++den_index)
	{
		const int      tst_den = den_list [den_index];
		const int      tst_num = fstb::round_int (frac_val * tst_den);

		// We take this solution only if it is displayable.
		// We can tolerate that the numerator gives a digit to the
		// denominator to display small values.
		if (   (tst_num      <= max_num && tst_den <= max_den         )
		    || (tst_num * 10 <= max_num && tst_den <= max_den * 10 + 9))
		{
			const double   exact_tst = double (tst_num) / double (tst_den);
			const double   err = fabs (frac_val - exact_tst);
			if (err < best_err)
			{
				// Avoids both big numerator and denominator
				if (max_num > 9 && tst_den >= 16 && tst_num > 8)
				{
					cont_flag = false;
				}
				else
				{
					best_num = tst_num;
					best_den = tst_den;
					best_err = err;	
				}
			}
		}
		else
		{
			cont_flag = false;
		}
	}

	assert (best_den > 1);
	num = best_num;
	den = best_den;

	return fit_flag;
}



bool	HelperDispNum::scan_frac (const char txt_0 [], double &val)
{
	assert (txt_0 != nullptr);

	bool           ok_flag = true;

	const int      nbr_val = 3;
	double         res [nbr_val];
	const char *   base_0 = txt_0;
	int            nbr_conv = 0;
	for (int i = 0; i < nbr_val && *base_0 != '\0'; ++i)
	{
		const char *      end_0;
		res [i] = strtod (base_0, const_cast <char **> (&end_0));
		if (end_0 != base_0)
		{
			++ nbr_conv;
			base_0 = end_0;
			while (*base_0 != '\0' && (*base_0 == ' ' || *base_0 == '/'))
			{
				++ base_0;
			}
		}
	}

	switch (nbr_conv)
	{
	//	Integer only
	case 1:
	case 2:
		val = res [0];
		break;

	// Fraction only
	case 3:
	case 4:
		if (fstb::is_null (res [1]))
		{
			ok_flag = false;
		}
		val = res [0] / res [1];
		break;

	// Integer + fraction
	case 5:
		if (fstb::is_null (res [2]))
		{
			ok_flag = false;
		}
		val = res [0] + res [1] / res [2];
		break;

	default:
		ok_flag = false;
		break;
	}

	return ok_flag;
}



bool	HelperDispNum::scan_ratio (const char txt_0 [], double &val, Type type)
{
	bool           ok_flag = true;

	char           part_0 [2] [63+1];
	const char *   colon_0 = strchr (txt_0, ':');
	int            nbr_conv = 0;
	if (colon_0 == nullptr)
	{
		fstb::txt::utf8::strncpy_0 (part_0 [0], txt_0, sizeof (part_0 [0]));
		nbr_conv = (part_0 [0] [0] != '\0') ? 1 : 0;
	}
	else
	{
		memcpy (
			part_0 [0],
			txt_0,
			std::min (strlen (txt_0), sizeof (part_0 [0]))
		);
		fstb::txt::utf8::strncpy_0 (part_0 [1], colon_0 + 1, sizeof (part_0 [1]));
		nbr_conv  = 3;
	}
	part_0 [0] [sizeof (part_0 [0]) - 1] = '\0';
	part_0 [1] [sizeof (part_0 [1]) - 1] = '\0';

	int            nbr_parts = 0;
	if (nbr_conv == 0)
	{
		ok_flag = false;
	}
	else
	{
		nbr_parts = (nbr_conv == 3) ? 2 : 1;
	}

	double         part [2] = { 1, 1 };
	for (int cnt = 0; cnt < nbr_parts && ok_flag; ++cnt)
	{
		if (find_infinite (part_0 [cnt]))
		{
			part [1 - cnt] = 0;
		}

		else
		{
			char *         end_ptr = nullptr;
			const char *   start_0 = part_0 [cnt];
			const double   x       = strtod (start_0, &end_ptr);
			if (end_ptr == start_0 || fabs (x) == HUGE_VAL)
			{
				ok_flag = false;
			}
			else
			{
				part [cnt] = x;
			}
		}
	}

	if (ok_flag)
	{
		if (part [0] == 0 && part [1] == 0)
		{
			part [0] = 1;
			part [1] = 1;
		}

		if (type == Type_RATIO_INV)
		{
			std::swap (part [0], part [1]);
		}

		if (part [1] == 0)
		{
			ok_flag = false;
		}
		else
		{
			val = part [0] / part [1];
		}
	}

	return ok_flag;
}



const char *	HelperDispNum::_note_0_list [12] =
{
	"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};



const char *	HelperDispNum::_inf_0_list [2] =
{
	"\xE2\x88\x9E",	// UTF-8 encoding for Unicode U+221E: INFINITY sign.
	"oo"					// ASCII emulation
};



}  // namespace param
}  // namespace pi
}  // namespace mfx



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
