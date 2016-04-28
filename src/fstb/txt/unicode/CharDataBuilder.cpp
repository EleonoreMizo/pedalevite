/*****************************************************************************

        CharDataBuilder.cpp
        Author: Laurent de Soras, 2008

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

#include	"fstb/txt/neutral/fnc.h"
#include	"fstb/txt/unicode/Categ.h"
#include	"fstb/txt/unicode/CharData.h"
#include	"fstb/txt/unicode/CharDataInfo.h"
#include	"fstb/txt/unicode/CharDataBuilder.h"
#include	"fstb/Err.h"
#include	"fstb/fnc.h"

#include	<cassert>
#include	<cstdio>



namespace fstb
{
namespace txt
{
namespace unicode
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



CharDataBuilder::CharDataBuilder ()
{
	// Nothing
}



int	CharDataBuilder::build_tables_from_unicode_ref_data (std::string &source, const char unicode_data_txt_filename_0 [], const char grapheme_break_property_txt_filename_0 [], const char line_break_txt_filename_0 [], const char word_break_property_txt_filename_0 [])
{
	assert (&source != 0);
	assert (unicode_data_txt_filename_0 != 0);
	assert (grapheme_break_property_txt_filename_0 != 0);
	assert (line_break_txt_filename_0 != 0);
	assert (word_break_property_txt_filename_0 != 0);

	int				ret_val = Err_OK;

	std::string		data_main;
	if (ret_val == Err_OK)
	{
		ret_val = read_file_as_string (data_main, unicode_data_txt_filename_0);
	}

	std::string		data_gbp;
	if (ret_val == Err_OK)
	{
		ret_val = read_file_as_string (data_gbp, grapheme_break_property_txt_filename_0);
	}

	std::string		data_lbp;
	if (ret_val == Err_OK)
	{
		ret_val = read_file_as_string (data_lbp, line_break_txt_filename_0);
	}

	std::string		data_wbp;
	if (ret_val == Err_OK)
	{
		ret_val = read_file_as_string (data_wbp, word_break_property_txt_filename_0);
	}

	if (ret_val == Err_OK)
	{
		Sheet				sheet_main;
		conv_semicolon_file_as_string_to_sheet (sheet_main, data_main);

		InfoList			info_list;
		extract_main_data (info_list, sheet_main);

		Sheet				sheet_gbp;
		conv_semicolon_file_as_string_to_sheet (sheet_gbp, data_gbp);
		remove_comments (sheet_gbp);
		remove_empty_lines (sheet_gbp);
		extract_grapheme_break_data (info_list, sheet_gbp);

		Sheet				sheet_lbp;
		conv_semicolon_file_as_string_to_sheet (sheet_lbp, data_lbp);
		remove_comments (sheet_lbp);
		remove_empty_lines (sheet_lbp);
		extract_line_break_data (info_list, sheet_lbp);

		Sheet				sheet_wbp;
		conv_semicolon_file_as_string_to_sheet (sheet_wbp, data_wbp);
		remove_comments (sheet_wbp);
		remove_empty_lines (sheet_wbp);
		extract_word_break_data (info_list, sheet_wbp);

		RefList			ref_list;
		shrink_char_info_set (info_list, ref_list);

		std::vector <long>	index_page_list;
		std::vector <long>	index_info_list;
		compact_data (index_page_list, index_info_list, info_list, ref_list);

		TableBreakGrapheme	table_break_grapheme;
		build_table_break_grapheme (table_break_grapheme);

		TableBreakWord	table_break_word;
		build_table_break_word (table_break_word);

		print_begin (source);
		print_info_unique_list (source, info_list);
		print_index_info_list (source, index_info_list);
		print_index_page_list (source, index_page_list);
		print_table_break_grapheme (source, table_break_grapheme);
		print_table_break_word (source, table_break_word);
		print_end (source);
	}

	return (ret_val);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int	CharDataBuilder::read_file_as_string (std::string &data, const char unicode_data_txt_filename_0 [])
{
	assert (&data != 0);
	assert (unicode_data_txt_filename_0 != 0);

	int				ret_val = Err_OK;

	using namespace std;

	FILE *			f_ptr = fopen (unicode_data_txt_filename_0, "r");
	if (f_ptr == 0)
	{
		ret_val = Err_CANNOT_OPEN_FILE;
	}

   if (ret_val == Err_OK)
   {
		data.erase (data.begin (), data.end ());

		bool				cont_flag = true;
		const long		tmp_len = 65536;
		char				tmp [tmp_len];
		do
		{
			const long		work_len = long (fread (&tmp [0], 1, tmp_len, f_ptr));
			cont_flag = (work_len == tmp_len);
			std::string	tmp_str (&tmp [0], work_len);
			data += tmp_str;
		}
		while (cont_flag);
   }

	if (f_ptr != 0)
	{
		fclose (f_ptr);
	}

	return (ret_val);
}



void	CharDataBuilder::conv_semicolon_file_as_string_to_sheet (Sheet &sheet, const std::string &data)
{
	assert (&sheet != 0);
	assert (&data != 0);

	sheet.clear ();
   sheet.reserve (20000);	// Reserves memory for 20000 lines

	std::string::size_type	pos = 0;
	std::string		acc_field;
	Line				acc_line;
   do
   {
		const char		c = data [pos];
		if (c == ';' || c == '\n')
		{
			acc_line.push_back (acc_field);
			acc_field.erase (acc_field.begin (), acc_field.end ());
			if (c == '\n')
			{
				sheet.push_back (acc_line);
				acc_line.clear ();
			}
		}
		else
		{
			acc_field += c;
		}

		++ pos;
   }
   while (pos < data.length ());

	if (acc_field.length () > 0)
	{
		acc_line.push_back (acc_field);
	}
	if (acc_line.size () > 0)
	{
		sheet.push_back (acc_line);
	}
}



void	CharDataBuilder::remove_comments (Sheet &sheet)
{
	assert (&sheet != 0);

	const long		nbr_lines = long (sheet.size ());
	for (long line_pos = 0; line_pos < nbr_lines; ++line_pos)
	{
		Line &			line = sheet [line_pos];
		bool				trunc_flag = false;
		const long		nbr_fields = long (line.size ());
		for (long field_pos = 0
		;	field_pos < nbr_fields && ! trunc_flag
		;	++field_pos)
		{
			std::string &	field = line [field_pos];
			const std::string::size_type	sharp_pos = field.find ('#');
			if (sharp_pos != std::string::npos)
			{
				long				erase_pos = field_pos;
				if (sharp_pos > 0)
				{
					field.erase (field.begin () + sharp_pos, field.end ());
					++ erase_pos;
				}

				line.erase (line.begin () + erase_pos, line.end ());
				trunc_flag = true;
			}
		}
	}
}



void	CharDataBuilder::remove_empty_lines (Sheet &sheet)
{
	assert (&sheet != 0);

	Sheet::size_type	line_pos = 0;
	while (line_pos < sheet.size ())
	{
		Line &			line = sheet [line_pos];
		if (   line.empty ()
			 || (line.size () == 1 && line [0].length () == 0))
		{
			sheet.erase (sheet.begin () + line_pos);
		}
		else
		{
			++ line_pos;
		}
	}
}



void	CharDataBuilder::extract_main_data (InfoList &info_list, const Sheet &sheet)
{
	assert (&info_list != 0);
	assert (&sheet != 0);

	info_list.clear ();
	info_list.resize (CharData::MAX_NBR_CODE_POINTS, _not_assigned);

	char32_t       range_beg = -1;	// -1 = range not started

	const long     nbr_lines = long (sheet.size ());
	for (long line_cnt = 0; line_cnt < nbr_lines; ++line_cnt)
	{
		const Line &	line = sheet [line_cnt];
		assert (line.size () >= 15);

		char *			dummy_ptr;

		// Decodes the code point
		const char32_t code_point =
			strtol (&line [Field_CODE_POINT] [0], &dummy_ptr, 16);
		assert (code_point >= 0);
		assert (code_point < CharData::MAX_NBR_CODE_POINTS);

		// Finds the range, if any
		if (line [Field_NAME].find ("First>") != std::string::npos)
		{
			range_beg = code_point;
		}
		else
		{
			if (range_beg >= 0)
			{
				assert (line [Field_NAME].find ("Last>") != std::string::npos);
			}
			else
			{
				range_beg = code_point;
			}

			CharDataInfo	info = _not_assigned;

			// Category
			bool				found_flag = false;
			for (int categ_cnt = 0
			;	_categ_list [categ_cnt]._name_0 != 0 && ! found_flag
			;	++ categ_cnt)
			{
				if (line [Field_CATEG] == _categ_list [categ_cnt]._name_0)
				{
					info._categ = _categ_list [categ_cnt]._value;
					found_flag = true;
				}
			}

			// Bidi class
			found_flag = false;
			for (int bidi_cnt = 0
			;	_bidi_class_list [bidi_cnt]._name_0 != 0 && ! found_flag
			;	++ bidi_cnt)
			{
				if (line [Field_BIDI_CLASS] == _bidi_class_list [bidi_cnt]._name_0)
				{
					info._bidi_class = _bidi_class_list [bidi_cnt]._value;
					found_flag = true;
				}
			}

			// Digit
			info._digit = static_cast <uint8_t> (
				strtol (&line [Field_DECIMAL_DIGIT] [0], &dummy_ptr, 10)
			);

			// Upper case
			info._upper = strtol (&line [Field_UPPER_CASE] [0], &dummy_ptr, 16);

			// Lower case
			info._lower = strtol (&line [Field_LOWER_CASE] [0], &dummy_ptr, 16);

			// Title case
			info._title = strtol (&line [Field_TITLE_CASE] [0], &dummy_ptr, 16);

			// From range_beg to code_point (included)
			for (char32_t pos = range_beg; pos <= code_point; ++pos)
			{
				info_list [pos] = info;
			}

			range_beg = -1;
		}
	}
}



void	CharDataBuilder::extract_grapheme_break_data (InfoList &info_list, const Sheet &sheet)
{
	assert (&info_list != 0);
	assert (&sheet != 0);
	assert (info_list.size () == CharData::MAX_NBR_CODE_POINTS);

	for (Sheet::size_type line_pos = 0; line_pos < sheet.size (); ++line_pos)
	{
		const Line &	line = sheet [line_pos];
		assert (line.size () >= 2);	// 2 fields

		// Range
		char32_t       range_first;
		char32_t       range_last;
		detect_range (range_first, range_last, line [0].c_str ());

		// Category
		const std::string	name  = neutral::trim_spaces (line [1].c_str ());
		bool				found_flag = false;
		GraphemeBreakProp	gbp;
		for (int categ_cnt = 0
		;	_grapheme_break_prop_list [categ_cnt]._name_0 != 0 && ! found_flag
		;	++ categ_cnt)
		{
			if (name == _grapheme_break_prop_list [categ_cnt]._name_0)
			{
				gbp = _grapheme_break_prop_list [categ_cnt]._value;
				found_flag = true;
			}
		}
		assert (found_flag);

		// Assigns values
		for (char32_t code_point = range_first
		;	code_point <= range_last
		;	++code_point)
		{
			CharDataInfo &	info = info_list [code_point];
			info._grapheme_break_prop = gbp;
		}
	}
}



void	CharDataBuilder::extract_line_break_data (InfoList &info_list, const Sheet &sheet)
{
	assert (&info_list != 0);
	assert (&sheet != 0);
	assert (info_list.size () == CharData::MAX_NBR_CODE_POINTS);

	for (Sheet::size_type line_pos = 0; line_pos < sheet.size (); ++line_pos)
	{
		const Line &	line = sheet [line_pos];
		assert (line.size () >= 2);	// 2 fields

		// Range
		char32_t       range_first;
		char32_t       range_last;
		detect_range (range_first, range_last, line [0].c_str ());

		// Category
		const std::string	name  = neutral::trim_spaces (line [1].c_str ());
		bool				found_flag = false;
		LineBreakProp	lbp;
		for (int categ_cnt = 0
		;	_line_break_prop_list [categ_cnt]._name_0 != 0 && ! found_flag
		;	++ categ_cnt)
		{
			if (name == _line_break_prop_list [categ_cnt]._name_0)
			{
				lbp = _line_break_prop_list [categ_cnt]._value;
				found_flag = true;
			}
		}
		assert (found_flag);

		// Assigns values
		for (char32_t code_point = range_first
		;	code_point <= range_last
		;	++code_point)
		{
			CharDataInfo &	info = info_list [code_point];
			info._line_break_prop = lbp;
		}
	}
}



void	CharDataBuilder::extract_word_break_data (InfoList &info_list, const Sheet &sheet)
{
	assert (&info_list != 0);
	assert (&sheet != 0);
	assert (info_list.size () == CharData::MAX_NBR_CODE_POINTS);

	for (Sheet::size_type line_pos = 0; line_pos < sheet.size (); ++line_pos)
	{
		const Line &	line = sheet [line_pos];
		assert (line.size () >= 2);	// 2 fields

		// Range
		char32_t       range_first;
		char32_t       range_last;
		detect_range (range_first, range_last, line [0].c_str ());

		// Category
		const std::string	name  = neutral::trim_spaces (line [1].c_str ());
		bool				found_flag = false;
		WordBreakProp	wbp;
		for (int categ_cnt = 0
		;	_word_break_prop_list [categ_cnt]._name_0 != 0 && ! found_flag
		;	++ categ_cnt)
		{
			if (name == _word_break_prop_list [categ_cnt]._name_0)
			{
				wbp = _word_break_prop_list [categ_cnt]._value;
				found_flag = true;
			}
		}
		assert (found_flag);

		// Assigns values
		for (char32_t code_point = range_first
		;	code_point <= range_last
		;	++code_point)
		{
			CharDataInfo &	info = info_list [code_point];
			info._word_break_prop = wbp;
		}
	}
}



void	CharDataBuilder::shrink_char_info_set (InfoList &info_list, RefList &ref_list)
{
	assert (&info_list != 0);
	assert (&ref_list != 0);
	assert (info_list.size () == CharData::MAX_NBR_CODE_POINTS);

	InfoList		shrunk_info_list;
	MapInfoIndex	info_map;		// Maps unique info to NEW position in (shrunk_)info_list

	// Forces the "not assigned" element to be always first, God wants it.
	shrunk_info_list.push_back (_not_assigned);
	info_map [_not_assigned] = 0;

	ref_list.resize (info_list.size ());

	for (char32_t code_point = 0
	;	code_point < char32_t (info_list.size ())
	;	++ code_point)
	{
		const CharDataInfo &	info = info_list [code_point];

		InfoList::size_type	list_pos;
		const MapInfoIndex::const_iterator	it = info_map.find (info);
		if (it == info_map.end ())
		{
			list_pos = shrunk_info_list.size ();
			shrunk_info_list.push_back (info);
			info_map [info] = list_pos;
		}
		else
		{
			list_pos = it->second;
		}

		ref_list [code_point] = list_pos;
	}

	info_list.swap (shrunk_info_list);
}



void	CharDataBuilder::compact_data (std::vector <long> &index_page_list, std::vector <long> &index_info_list, const InfoList &info_list, const RefList &ref_list)
{
	assert (&info_list != 0);
	assert (&ref_list != 0);

	const long		nbr_pages =
		CharData::MAX_NBR_CODE_POINTS >> CharData::PAGE_SIZE_L2;
	index_page_list.resize (nbr_pages);
	index_info_list.clear ();
	long				nbr_zones = 0;

	ZoneMapIndex	zone_map;

	for (long page_cnt = 0; page_cnt < nbr_pages; ++page_cnt)
	{
		// Checks if page is uniform
		const long		first_index = page_cnt << CharData::PAGE_SIZE_L2;
		const long		ref = long (ref_list [first_index]);
		bool				uniform_flag = true;
		for (long cnt = 1; cnt < PAGE_SIZE_X && uniform_flag; ++cnt)
		{
			if (ref_list [first_index + cnt] != ref_list [first_index])
			{
				uniform_flag = false;
			}
		}

		if (uniform_flag)
		{
			index_page_list [page_cnt] = ref | (1 << CharData::DIRECT_L2);
		}

		else
		{
			long				cnt;
			assert (nbr_zones < 1L << CharData::DIRECT_L2);

			ZoneContent		zone_content;
			for (cnt = 0; cnt < PAGE_SIZE_X ; ++cnt)
			{
				zone_content._val_arr [cnt] = long (ref_list [first_index + cnt]);
			}

			long				zone_index = -1;
			ZoneMapIndex::const_iterator	it = zone_map.find (zone_content);
			if (it == zone_map.end ())
			{
				zone_index = nbr_zones;
				zone_map [zone_content] = zone_index;
				for (cnt = 0; cnt < PAGE_SIZE_X ; ++cnt)
				{
					index_info_list.push_back (zone_content._val_arr [cnt]);
				}
				++ nbr_zones;
			}
			else
			{
				zone_index = it->second;
			}

			index_page_list [page_cnt] = zone_index;
		}
	}
}



void	CharDataBuilder::build_table_break_grapheme (TableBreakGrapheme &table)
{
	assert (&table != 0);

	for (int prev = 0; prev < GraphemeBreakProp_NBR_ELT; ++prev)
	{
		for (int next = 0; next < GraphemeBreakProp_NBR_ELT; ++next)
		{
			bool				break_flag = true;	// Default: break OK

			// GB3
			if (prev == GraphemeBreakProp_CR && next == GraphemeBreakProp_LF)
			{
				break_flag = false;
			}

			// GB4
			else if (   prev == GraphemeBreakProp_CONTROL
			         || prev == GraphemeBreakProp_CR
			         || prev == GraphemeBreakProp_LF)
			{
				break_flag = true;
			}

			// GB5
			else if (   next == GraphemeBreakProp_CONTROL
			         || next == GraphemeBreakProp_CR
			         || next == GraphemeBreakProp_LF)
			{
				break_flag = true;
			}

			// GB6
			else if (   prev == GraphemeBreakProp_L
			         && (   next == GraphemeBreakProp_L
			             || next == GraphemeBreakProp_V
			             || next == GraphemeBreakProp_LV
			             || next == GraphemeBreakProp_LVT))
			{
				break_flag = false;
			}

			// GB7
			else if (   (   prev == GraphemeBreakProp_LV
			             || prev == GraphemeBreakProp_V)
			         && (   next == GraphemeBreakProp_V
			             || next == GraphemeBreakProp_T))
			{
				break_flag = false;
			}

			// GB8
			else if (   (   prev == GraphemeBreakProp_LVT
			             || prev == GraphemeBreakProp_T)
			         && next == GraphemeBreakProp_T)
			{
				break_flag = false;
			}

			// GB9
			else if (next == GraphemeBreakProp_EXTEND)
			{
				break_flag = false;
			}

			// GB9a
			else if (next == GraphemeBreakProp_SP_MARK)
			{
				break_flag = false;
			}

			// GB9b
			else if (prev == GraphemeBreakProp_PREPEND)
			{
				break_flag = false;
			}

			table [prev] [next] = break_flag;
		}
	}
}



void	CharDataBuilder::build_table_break_word (TableBreakWord &table)
{
	assert (&table != 0);

	for (int prev = 0; prev < WordBreakPropTablePrev_NBR_ELT; ++prev)
	{
		for (int next = 0; next < WordBreakPropTableNext_NBR_ELT; ++next)
		{
			bool				break_flag = true;	// WB14 - Default: break OK

			// WB5
			if (   prev == WordBreakProp_ALETTER
			    && next == WordBreakProp_ALETTER)
			{
				break_flag = false;
			}

			// WB6
			else if (   prev == WordBreakProp_ALETTER
			         && (   next == WordBreakPropTableNext_MIDLETTER_ALETTER
			             || next == WordBreakPropTableNext_MIDNUMLET_ALETTER))
			{
				break_flag = false;
			}

			// WB7
			else if (   (   prev == WordBreakPropTablePrev_ALETTER_MIDLETTER
			             || prev == WordBreakPropTablePrev_ALETTER_MIDNUMLET)
			         && next == WordBreakProp_ALETTER)
			{
				break_flag = false;
			}

			// WB8
			else if (   prev == WordBreakProp_NUMERIC
			         && next == WordBreakProp_NUMERIC)
			{
				break_flag = false;
			}

			// WB9
			else if (   prev == WordBreakProp_ALETTER
			         && next == WordBreakProp_NUMERIC)
			{
				break_flag = false;
			}

			// WB10
			else if (   prev == WordBreakProp_NUMERIC
			         && next == WordBreakProp_ALETTER)
			{
				break_flag = false;
			}

			// WB11
			else if (   (   prev == WordBreakPropTablePrev_NUMERIC_MIDNUM
			             || prev == WordBreakPropTablePrev_NUMERIC_MIDNUMLET)
			         && next == WordBreakProp_NUMERIC)
			{
				break_flag = false;
			}

			// WB12
			else if (   prev == WordBreakProp_NUMERIC
			         && (   next == WordBreakPropTableNext_MIDNUM_NUMERIC
			             || next == WordBreakPropTableNext_MIDNUMLET_NUMERIC))
			{
				break_flag = false;
			}

			// WB13
			else if (   prev == WordBreakProp_KATAKANA
			         && next == WordBreakProp_KATAKANA)
			{
				break_flag = false;
			}

			// WB13a
			else if (   (   prev == WordBreakProp_ALETTER
			             || prev == WordBreakProp_NUMERIC
			             || prev == WordBreakProp_KATAKANA
			             || prev == WordBreakProp_EXTENDNUMLET)
			         && next == WordBreakProp_EXTENDNUMLET)
			{
				break_flag = false;
			}

			// WB13b
			else if (   prev == WordBreakProp_EXTENDNUMLET
			         && (   next == WordBreakProp_ALETTER
			             || next == WordBreakProp_NUMERIC
			             || next == WordBreakProp_KATAKANA))
			{
				break_flag = false;
			}

			table [prev] [next] = break_flag;
		}
	}
}



void	CharDataBuilder::print_begin (std::string &source)
{
	assert (&source != 0);

	source = "// Begin: automatically generated data\n\n\n\n";
}



void	CharDataBuilder::print_info_unique_list (std::string &source, const InfoList &info_list)
{
	assert (&source != 0);
	assert (&info_list != 0);

	ValueNameMap	cat_name_map;
	cat_name_map [Categ_LETTER_UPPERCASE         ] = "Categ_LETTER_UPPERCASE";
	cat_name_map [Categ_LETTER_LOWERCASE         ] = "Categ_LETTER_LOWERCASE";
	cat_name_map [Categ_LETTER_TITLECASE         ] = "Categ_LETTER_TITLECASE";
	cat_name_map [Categ_LETTER_MODIFIER          ] = "Categ_LETTER_MODIFIER";
	cat_name_map [Categ_LETTER_OTHER             ] = "Categ_LETTER_OTHER";
	cat_name_map [Categ_MARK_NONSPACING          ] = "Categ_MARK_NONSPACING";
	cat_name_map [Categ_MARK_SPACING_COMBINING   ] = "Categ_MARK_SPACING_COMBINING";
	cat_name_map [Categ_MARK_ENCLOSING           ] = "Categ_MARK_ENCLOSING";
	cat_name_map [Categ_NUMBER_DECIMAL_DIGIT     ] = "Categ_NUMBER_DECIMAL_DIGIT";
	cat_name_map [Categ_NUMBER_LETTER            ] = "Categ_NUMBER_LETTER";
	cat_name_map [Categ_NUMBER_OTHER             ] = "Categ_NUMBER_OTHER";
	cat_name_map [Categ_PUNCTUATION_CONNECTOR    ] = "Categ_PUNCTUATION_CONNECTOR";
	cat_name_map [Categ_PUNCTUATION_DASH         ] = "Categ_PUNCTUATION_DASH";
	cat_name_map [Categ_PUNCTUATION_OPEN         ] = "Categ_PUNCTUATION_OPEN";
	cat_name_map [Categ_PUNCTUATION_CLOSE        ] = "Categ_PUNCTUATION_CLOSE";
	cat_name_map [Categ_PUNCTUATION_INITIAL_QUOTE] = "Categ_PUNCTUATION_INITIAL_QUOTE";
	cat_name_map [Categ_PUNCTUATION_FINAL_QUOTE  ] = "Categ_PUNCTUATION_FINAL_QUOTE";
	cat_name_map [Categ_PUNCTUATION_OTHER        ] = "Categ_PUNCTUATION_OTHER";
	cat_name_map [Categ_SYMBOL_MATH              ] = "Categ_SYMBOL_MATH";
	cat_name_map [Categ_SYMBOL_CURRENCY          ] = "Categ_SYMBOL_CURRENCY";
	cat_name_map [Categ_SYMBOL_MODIFIER          ] = "Categ_SYMBOL_MODIFIER";
	cat_name_map [Categ_SYMBOL_OTHER             ] = "Categ_SYMBOL_OTHER";
	cat_name_map [Categ_SEPARATOR_SPACE          ] = "Categ_SEPARATOR_SPACE";
	cat_name_map [Categ_SEPARATOR_LINE           ] = "Categ_SEPARATOR_LINE";
	cat_name_map [Categ_SEPARATOR_PARAGRAPH      ] = "Categ_SEPARATOR_PARAGRAPH";
	cat_name_map [Categ_OTHER_CONTROL            ] = "Categ_OTHER_CONTROL";
	cat_name_map [Categ_OTHER_FORMAT             ] = "Categ_OTHER_FORMAT";
	cat_name_map [Categ_OTHER_SURROGATE          ] = "Categ_OTHER_SURROGATE";
	cat_name_map [Categ_OTHER_PRIVATE_USE        ] = "Categ_OTHER_PRIVATE_USE";
	cat_name_map [Categ_OTHER_NOT_ASSIGNED       ] = "Categ_OTHER_NOT_ASSIGNED";

	ValueNameMap	gbp_name_map;
	gbp_name_map [GraphemeBreakProp_OTHER  ] = "GraphemeBreakProp_OTHER";
	gbp_name_map [GraphemeBreakProp_CR     ] = "GraphemeBreakProp_CR";
	gbp_name_map [GraphemeBreakProp_LF     ] = "GraphemeBreakProp_LF";
	gbp_name_map [GraphemeBreakProp_CONTROL] = "GraphemeBreakProp_CONTROL";
	gbp_name_map [GraphemeBreakProp_EXTEND ] = "GraphemeBreakProp_EXTEND";
	gbp_name_map [GraphemeBreakProp_PREPEND] = "GraphemeBreakProp_PREPEND";
	gbp_name_map [GraphemeBreakProp_SP_MARK] = "GraphemeBreakProp_SP_MARK";
	gbp_name_map [GraphemeBreakProp_L      ] = "GraphemeBreakProp_L";
	gbp_name_map [GraphemeBreakProp_V      ] = "GraphemeBreakProp_V";
	gbp_name_map [GraphemeBreakProp_T      ] = "GraphemeBreakProp_T";
	gbp_name_map [GraphemeBreakProp_LV     ] = "GraphemeBreakProp_LV";
	gbp_name_map [GraphemeBreakProp_LVT    ] = "GraphemeBreakProp_LVT";

	ValueNameMap	lbp_name_map;
	lbp_name_map [LineBreakProp_OP] = "LineBreakProp_OP";
	lbp_name_map [LineBreakProp_CL] = "LineBreakProp_CL";
	lbp_name_map [LineBreakProp_QU] = "LineBreakProp_QU";
	lbp_name_map [LineBreakProp_GL] = "LineBreakProp_GL";
	lbp_name_map [LineBreakProp_NS] = "LineBreakProp_NS";
	lbp_name_map [LineBreakProp_EX] = "LineBreakProp_EX";
	lbp_name_map [LineBreakProp_SY] = "LineBreakProp_SY";
	lbp_name_map [LineBreakProp_IS] = "LineBreakProp_IS";
	lbp_name_map [LineBreakProp_PR] = "LineBreakProp_PR";
	lbp_name_map [LineBreakProp_PO] = "LineBreakProp_PO";
	lbp_name_map [LineBreakProp_NU] = "LineBreakProp_NU";
	lbp_name_map [LineBreakProp_AL] = "LineBreakProp_AL";
	lbp_name_map [LineBreakProp_ID] = "LineBreakProp_ID";
	lbp_name_map [LineBreakProp_IN] = "LineBreakProp_IN";
	lbp_name_map [LineBreakProp_HY] = "LineBreakProp_HY";
	lbp_name_map [LineBreakProp_BA] = "LineBreakProp_BA";
	lbp_name_map [LineBreakProp_BB] = "LineBreakProp_BB";
	lbp_name_map [LineBreakProp_B2] = "LineBreakProp_B2";
	lbp_name_map [LineBreakProp_ZW] = "LineBreakProp_ZW";
	lbp_name_map [LineBreakProp_CM] = "LineBreakProp_CM";
	lbp_name_map [LineBreakProp_WJ] = "LineBreakProp_WJ";
	lbp_name_map [LineBreakProp_H2] = "LineBreakProp_H2";
	lbp_name_map [LineBreakProp_H3] = "LineBreakProp_H3";
	lbp_name_map [LineBreakProp_JL] = "LineBreakProp_JL";
	lbp_name_map [LineBreakProp_JV] = "LineBreakProp_JV";
	lbp_name_map [LineBreakProp_JT] = "LineBreakProp_JT";
	lbp_name_map [LineBreakProp_SA] = "LineBreakProp_SA";
	lbp_name_map [LineBreakProp_SP] = "LineBreakProp_SP";
	lbp_name_map [LineBreakProp_PS] = "LineBreakProp_PS";
	lbp_name_map [LineBreakProp_BK] = "LineBreakProp_BK";
	lbp_name_map [LineBreakProp_CR] = "LineBreakProp_CR";
	lbp_name_map [LineBreakProp_LF] = "LineBreakProp_LF";
	lbp_name_map [LineBreakProp_NL] = "LineBreakProp_NL";
	lbp_name_map [LineBreakProp_CB] = "LineBreakProp_CB";
	lbp_name_map [LineBreakProp_SG] = "LineBreakProp_SG";
	lbp_name_map [LineBreakProp_AI] = "LineBreakProp_AI";
	lbp_name_map [LineBreakProp_XX] = "LineBreakProp_XX";

	ValueNameMap	wbp_name_map;
	wbp_name_map [WordBreakProp_OTHER       ] = "WordBreakProp_OTHER";
	wbp_name_map [WordBreakProp_CR          ] = "WordBreakProp_CR";
	wbp_name_map [WordBreakProp_LF          ] = "WordBreakProp_LF";
	wbp_name_map [WordBreakProp_NEWLINE     ] = "WordBreakProp_NEWLINE";
	wbp_name_map [WordBreakProp_EXTEND      ] = "WordBreakProp_EXTEND";
	wbp_name_map [WordBreakProp_FORMAT      ] = "WordBreakProp_FORMAT";
	wbp_name_map [WordBreakProp_KATAKANA    ] = "WordBreakProp_KATAKANA";
	wbp_name_map [WordBreakProp_ALETTER     ] = "WordBreakProp_ALETTER";
	wbp_name_map [WordBreakProp_MIDLETTER   ] = "WordBreakProp_MIDLETTER";
	wbp_name_map [WordBreakProp_MIDNUM      ] = "WordBreakProp_MIDNUM";
	wbp_name_map [WordBreakProp_MIDNUMLET   ] = "WordBreakProp_MIDNUMLET";
	wbp_name_map [WordBreakProp_NUMERIC     ] = "WordBreakProp_NUMERIC";
	wbp_name_map [WordBreakProp_EXTENDNUMLET] = "WordBreakProp_EXTENDNUMLET";

	ValueNameMap	bdc_name_map;
	bdc_name_map [BidiClass_L  ] = "BidiClass_L";
	bdc_name_map [BidiClass_LRE] = "BidiClass_LRE";
	bdc_name_map [BidiClass_LRO] = "BidiClass_LRO";
	bdc_name_map [BidiClass_R  ] = "BidiClass_R";
	bdc_name_map [BidiClass_AL ] = "BidiClass_AL";
	bdc_name_map [BidiClass_RLE] = "BidiClass_RLE";
	bdc_name_map [BidiClass_RLO] = "BidiClass_RLO";
	bdc_name_map [BidiClass_PDF] = "BidiClass_PDF";
	bdc_name_map [BidiClass_EN ] = "BidiClass_EN";
	bdc_name_map [BidiClass_ES ] = "BidiClass_ES";
	bdc_name_map [BidiClass_ET ] = "BidiClass_ET";
	bdc_name_map [BidiClass_AN ] = "BidiClass_AN";
	bdc_name_map [BidiClass_CS ] = "BidiClass_CS";
	bdc_name_map [BidiClass_NSM] = "BidiClass_NSM";
	bdc_name_map [BidiClass_BN ] = "BidiClass_BN";
	bdc_name_map [BidiClass_B  ] = "BidiClass_B";
	bdc_name_map [BidiClass_S  ] = "BidiClass_S";
	bdc_name_map [BidiClass_WS ] = "BidiClass_WS";
	bdc_name_map [BidiClass_ON ] = "BidiClass_ON";

	source += "const CharDataInfo\tCharData::_info_unique_list [] = \n{\n";

	const InfoList::size_type	info_list_size = info_list.size ();
	for (InfoList::size_type index = 0; index < info_list_size; ++index)
	{
		const CharDataInfo &	info = info_list [index];
		const char *	cat_name_0 = cat_name_map [info._categ];
		const char *	gbp_name_0 = gbp_name_map [info._grapheme_break_prop];
		const char *	lbp_name_0 = lbp_name_map [info._line_break_prop];
		const char *	wbp_name_0 = wbp_name_map [info._word_break_prop];
		const char *	bdc_name_0 = bdc_name_map [info._bidi_class];

		char           txt_0 [1023+1];
		snprintf4all (
			txt_0, sizeof (txt_0),
#if defined (NDEBUG)
			"\t{ 0x%X, 0x%X, 0x%X, %d, %s, %d, "
			"%d, %s, %s, %s, %s }%s\n",
#else
			"\t{ 0x%06X, 0x%06X, 0x%06X, %d, %-31s, %3d, "
			"%3d, %-25s, %-16s, %-26s, %-13s }%s\n",
#endif
			info._upper,
			info._lower,
			info._title,
			info._digit,
			cat_name_0,
			info._kangxi_radical_stroke_cnt,
			info._residual_stroke_cnt,
			gbp_name_0,
			lbp_name_0,
			wbp_name_0,
			bdc_name_0,
			(index < info_list_size - 1) ? "," : ""
		);
		source += txt_0;
	}

	source += "};\n\n";
}



void	CharDataBuilder::print_index_info_list (std::string &source, const std::vector <long> &index_info_list)
{
	assert (&source != 0);
	assert (&index_info_list != 0);

	char           txt_0 [1023+1];

	const long		page_mask = PAGE_SIZE_X - 1;
	source += "const int16_t\tCharData::_index_info_list [] = \n{";
	const long		info_list_size = long (index_info_list.size ());
	for (long index = 0; index < info_list_size; ++index)
	{
		if (index > 0)
		{
			source += ",";
		}
		if ((index & 7) == 0)
		{
			source += "\n";
			if ((index & page_mask) == 0)
			{
				snprintf4all (
					txt_0,
					sizeof (txt_0),
					"// %ld\n",
					index >> CharData::PAGE_SIZE_L2
				);
				source += txt_0;
			}
			source += "\t";
		}
		else if (index > 0)
		{
			source += " ";
		}

		snprintf4all (
			txt_0,
			sizeof (txt_0),
#if defined (NDEBUG)
			"%ld",
#else
			"%5ld",
#endif
			index_info_list [index]
		);
		source += txt_0;
	}

	source += "\n};\n\n";
}



void	CharDataBuilder::print_index_page_list (std::string &source, const std::vector <long> &index_page_list)
{
	assert (&source != 0);
	assert (&index_page_list != 0);

	char           txt_0 [1023+1];

	source +=
		"const uint16_t\tCharData::_index_page_list "
		"[MAX_NBR_CODE_POINTS >> PAGE_SIZE_L2] = \n{";

	const long		index_page_list_size = long (index_page_list.size ());
	for (long index = 0; index < index_page_list_size; ++index)
	{
		if (index > 0)
		{
			source += ",";
		}
		if ((index & 7) == 0)
		{
			source += "\n";
			const long		code_point = index << CharData::PAGE_SIZE_L2;
			if ((code_point & 0xFFFF) == 0)
			{
				snprintf4all (
					txt_0,
					sizeof (txt_0),
					"\t// U+%06X\n",
					int (code_point)
				);
				source += txt_0;
			}
			source += "\t";
		}
		else if (index > 0)
		{
			source += " ";
		}

		const long		val = index_page_list [index];
		if ((val & (1L << CharData::DIRECT_L2)) != 0)
		{
			snprintf4all (
				txt_0,
				sizeof (txt_0),
#if defined (NDEBUG)
				"0x%X",
#else
				"0x%04X%",
#endif
				int (val)
			);
			source += txt_0;
		}
		else
		{
			snprintf4all (
				txt_0,
				sizeof (txt_0),
#if defined (NDEBUG)
				"%ld",
#else
				"%6ld%",
#endif
				val
			);
			source += txt_0;
		}
	}

	source += "\n};\n\n";
}



void	CharDataBuilder::print_table_break_grapheme (std::string &source, const TableBreakGrapheme table_break_grapheme)
{
	assert (&source != 0);
	assert (&table_break_grapheme != 0);

	char           txt_0 [1023+1];

	source +=
		"const bool\tCharData::_table_break_grapheme "
		"[GraphemeBreakProp_NBR_ELT] [GraphemeBreakProp_NBR_ELT] = \n{\n";

	for (int prev = 0; prev < GraphemeBreakProp_NBR_ELT; ++prev)
	{
		const bool	last_row_flag = (prev == GraphemeBreakProp_NBR_ELT - 1);
		source += "\t{";
		for (int next = 0; next < GraphemeBreakProp_NBR_ELT; ++next)
		{
			const bool	break_flag = table_break_grapheme [prev] [next];
			const bool	last_col_flag = (next == GraphemeBreakProp_NBR_ELT - 1);
			const bool	last_all_flag = (last_col_flag && last_row_flag);

			snprintf4all (
				txt_0,
				sizeof (txt_0),
				" %d%s%s",
				(break_flag   ) ?    1 : 0,
				(last_col_flag) ? " }" : "",
				(last_all_flag) ?   "" : ","
			);
			source += txt_0;
		}
		source += "\n";
	}

	source += "};\n\n";
}



void	CharDataBuilder::print_table_break_word (std::string &source, const TableBreakWord table_break_word)
{
	assert (&source != 0);
	assert (&table_break_word != 0);

	char           txt_0 [1023+1];

	source +=
		"const bool\tCharData::_table_break_word "
		"[WordBreakPropTablePrev_NBR_ELT] [WordBreakPropTableNext_NBR_ELT] = \n{\n";

	for (int prev = 0; prev < WordBreakPropTablePrev_NBR_ELT; ++prev)
	{
		const bool	last_row_flag = (prev == WordBreakPropTablePrev_NBR_ELT - 1);
		source += "\t{";
		for (int next = 0; next < WordBreakPropTableNext_NBR_ELT; ++next)
		{
			const bool	break_flag = table_break_word [prev] [next];
			const bool	last_col_flag = (next == WordBreakPropTableNext_NBR_ELT - 1);
			const bool	last_all_flag = (last_col_flag && last_row_flag);

			snprintf4all (
				txt_0,
				sizeof (txt_0),
				" %d%s%s",
				(break_flag   ) ?    1 : 0,
				(last_col_flag) ? " }" : "",
				(last_all_flag) ?   "" : ","
			);
			source += txt_0;
		}
		source += "\n";
	}

	source += "};\n\n";
}



void	CharDataBuilder::print_end (std::string &source)
{
	assert (&source != 0);

	source += "\n\n// End: automatically generated data\n\n\n\n";
}



void	CharDataBuilder::detect_range (char32_t &range_first, char32_t &range_last, const char field_0 [])
{
	assert (&range_first != 0);
	assert (&range_last != 0);
	assert (field_0 != 0);

	const std::string	range = neutral::trim_spaces (field_0);

	int64_t        first;
	long           stop_pos;
	const char *   range_0 = range.c_str ();
	neutral::conv_str_to_int64 (first, range_0, 16, &stop_pos);

	int64_t        last = first;
	if (   stop_pos + 2 < long (range.length ())
		 && range [stop_pos    ] == '.'
		 && range [stop_pos + 1] == '.')
	{
		neutral::conv_str_to_int64 (last, range_0 + stop_pos + 2, 16, 0);
	}
	assert (first <= last);

	range_first = char32_t (first);
	range_last  = char32_t (last);
}



bool	CharDataBuilder::ZoneContent::operator < (const ZoneContent &other) const
{
	assert (&other != 0);

	for (int cnt = 0; cnt < PAGE_SIZE_X ; ++cnt)
	{
		const long		val_lhs = _val_arr [cnt];
		const long		val_rhs = other._val_arr [cnt];

		if (val_lhs < val_rhs)
		{
			return (true);
		}
		else if (val_rhs < val_lhs)
		{
			return (false);
		}
	}

	return (false);
}



const CharDataBuilder::Elt <Categ>	CharDataBuilder::_categ_list [] =
{
	{ "Lu", Categ_LETTER_UPPERCASE },
	{ "Ll", Categ_LETTER_LOWERCASE },
	{ "Lt", Categ_LETTER_TITLECASE },
	{ "Lm", Categ_LETTER_MODIFIER },
	{ "Lo", Categ_LETTER_OTHER },
	{ "Mn", Categ_MARK_NONSPACING },
	{ "Mc", Categ_MARK_SPACING_COMBINING },
	{ "Me", Categ_MARK_ENCLOSING },
	{ "Nd", Categ_NUMBER_DECIMAL_DIGIT },
	{ "Nl", Categ_NUMBER_LETTER },
	{ "No", Categ_NUMBER_OTHER },
	{ "Pc", Categ_PUNCTUATION_CONNECTOR },
	{ "Pd", Categ_PUNCTUATION_DASH },
	{ "Ps", Categ_PUNCTUATION_OPEN },
	{ "Pe", Categ_PUNCTUATION_CLOSE },
	{ "Pi", Categ_PUNCTUATION_INITIAL_QUOTE },
	{ "Pf", Categ_PUNCTUATION_FINAL_QUOTE },
	{ "Po", Categ_PUNCTUATION_OTHER },
	{ "Sm", Categ_SYMBOL_MATH },
	{ "Sc", Categ_SYMBOL_CURRENCY },
	{ "Sk", Categ_SYMBOL_MODIFIER },
	{ "So", Categ_SYMBOL_OTHER },
	{ "Zs", Categ_SEPARATOR_SPACE },
	{ "Zl", Categ_SEPARATOR_LINE },
	{ "Zp", Categ_SEPARATOR_PARAGRAPH },
	{ "Cc", Categ_OTHER_CONTROL },
	{ "Cf", Categ_OTHER_FORMAT },
	{ "Cs", Categ_OTHER_SURROGATE },
	{ "Co", Categ_OTHER_PRIVATE_USE },
	{ "Cn", Categ_OTHER_NOT_ASSIGNED },
	{ 0 }
};



const CharDataBuilder::Elt <GraphemeBreakProp>	CharDataBuilder::_grapheme_break_prop_list [] =
{
	{ "Other",       GraphemeBreakProp_OTHER },
	{ "CR",          GraphemeBreakProp_CR },
	{ "LF",          GraphemeBreakProp_LF },
	{ "Control",     GraphemeBreakProp_CONTROL },
	{ "Extend",      GraphemeBreakProp_EXTEND },
	{ "Prepend",     GraphemeBreakProp_PREPEND },
	{ "SpacingMark", GraphemeBreakProp_SP_MARK },
	{ "L",           GraphemeBreakProp_L },
	{ "V",           GraphemeBreakProp_V },
	{ "T",           GraphemeBreakProp_T },
	{ "LV",          GraphemeBreakProp_LV },
	{ "LVT",         GraphemeBreakProp_LVT },
	{ 0 }
};



const CharDataBuilder::Elt <LineBreakProp>	CharDataBuilder::_line_break_prop_list [] =
{
	{ "OP", LineBreakProp_OP },
	{ "CL", LineBreakProp_CL },
	{ "QU", LineBreakProp_QU },
	{ "GL", LineBreakProp_GL },
	{ "NS", LineBreakProp_NS },
	{ "EX", LineBreakProp_EX },
	{ "SY", LineBreakProp_SY },
	{ "IS", LineBreakProp_IS },
	{ "PR", LineBreakProp_PR },
	{ "PO", LineBreakProp_PO },
	{ "NU", LineBreakProp_NU },
	{ "AL", LineBreakProp_AL },
	{ "ID", LineBreakProp_ID },
	{ "IN", LineBreakProp_IN },
	{ "HY", LineBreakProp_HY },
	{ "BA", LineBreakProp_BA },
	{ "BB", LineBreakProp_BB },
	{ "B2", LineBreakProp_B2 },
	{ "ZW", LineBreakProp_ZW },
	{ "CM", LineBreakProp_CM },
	{ "WJ", LineBreakProp_WJ },
	{ "H2", LineBreakProp_H2 },
	{ "H3", LineBreakProp_H3 },
	{ "JL", LineBreakProp_JL },
	{ "JV", LineBreakProp_JV },
	{ "JT", LineBreakProp_JT },
	{ "SA", LineBreakProp_SA },
	{ "SP", LineBreakProp_SP },
	{ "PS", LineBreakProp_PS },
	{ "BK", LineBreakProp_BK },
	{ "CR", LineBreakProp_CR },
	{ "LF", LineBreakProp_LF },
	{ "NL", LineBreakProp_NL },
	{ "CB", LineBreakProp_CB },
	{ "SG", LineBreakProp_SG },
	{ "AI", LineBreakProp_AI },
	{ "XX", LineBreakProp_XX },
	{ 0 }
};



const CharDataBuilder::Elt <WordBreakProp>	CharDataBuilder::_word_break_prop_list [] =
{
	{ "Other", WordBreakProp_OTHER },
	{ "CR", WordBreakProp_CR },
	{ "LF", WordBreakProp_LF },
	{ "Newline", WordBreakProp_NEWLINE },
	{ "Extend", WordBreakProp_EXTEND },
	{ "Format", WordBreakProp_FORMAT },
	{ "Katakana", WordBreakProp_KATAKANA },
	{ "ALetter", WordBreakProp_ALETTER },
	{ "MidLetter", WordBreakProp_MIDLETTER },
	{ "MidNum", WordBreakProp_MIDNUM },
	{ "MidNumLet", WordBreakProp_MIDNUMLET },
	{ "Numeric", WordBreakProp_NUMERIC },
	{ "ExtendNumLet", WordBreakProp_EXTENDNUMLET },
	{ 0 }
};



const CharDataBuilder::Elt <BidiClass>	CharDataBuilder::_bidi_class_list [] =
{
	{ "L",   BidiClass_L },	
	{ "LRE", BidiClass_LRE },
	{ "LRO", BidiClass_LRO },
	{ "R",   BidiClass_R },
	{ "AL",  BidiClass_AL },
	{ "RLE", BidiClass_RLE },
	{ "RLO", BidiClass_RLO },
	{ "PDF", BidiClass_PDF },
	{ "EN",  BidiClass_EN },
	{ "ES",  BidiClass_ES },
	{ "ET",  BidiClass_ET },
	{ "AN",  BidiClass_AN },
	{ "CS",  BidiClass_CS },
	{ "NSM", BidiClass_NSM },
	{ "BN",  BidiClass_BN },
	{ "B",   BidiClass_B },
	{ "S",   BidiClass_S },
	{ "WS",  BidiClass_WS },
	{ "ON",  BidiClass_ON },
	{ 0 }
};



const CharDataInfo	CharDataBuilder::_not_assigned =
{
	0, 0, 0,
	0,
	Categ_OTHER_NOT_ASSIGNED,
	0, 0,
	GraphemeBreakProp_OTHER,
	LineBreakProp_XX,
	WordBreakProp_OTHER,
	BidiClass_ON
};



}	// namespace unicode
}	// namespace txt
}	// namespace fstb



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
