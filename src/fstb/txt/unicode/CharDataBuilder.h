/*****************************************************************************

        CharDataBuilder.h
        Author: Laurent de Soras, 2008

The purpose of this class is only to generate static data as source code for
CharData.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_txt_unicode_CharDataBuilder_HEADER_INCLUDED)
#define	fstb_txt_unicode_CharDataBuilder_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"fstb/txt/unicode/BidiClass.h"
#include	"fstb/txt/unicode/Categ.h"
#include	"fstb/txt/unicode/CharData.h"
#include	"fstb/txt/unicode/CharDataInfo.h"
#include	"fstb/txt/unicode/GraphemeBreakProp.h"
#include	"fstb/txt/unicode/LineBreakProp.h"
#include	"fstb/txt/unicode/WordBreakProp.h"

#include	<map>
#include	<string>
#include	<vector>



namespace fstb
{
namespace txt
{
namespace unicode
{



class CharDataBuilder
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum {			PAGE_SIZE_X	= 1L << CharData::PAGE_SIZE_L2	};

	static int		build_tables_from_unicode_ref_data (std::string &source, const char unicode_data_txt_filename_0 [], const char grapheme_break_property_txt_filename_0 [], const char line_break_txt_filename_0 [], const char word_break_property_txt_filename_0 []);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:


	enum Field
	{
		Field_CODE_POINT		= 0,
		Field_NAME,
		Field_CATEG,
		Field_BIDI_CLASS		= 4,
		Field_DECIMAL_DIGIT	= 6,
		Field_UPPER_CASE		= 12,
		Field_LOWER_CASE,
		Field_TITLE_CASE
	};

	class ZoneContent
	{
	public:
		inline bool		operator < (const ZoneContent &other) const;
		long				_val_arr [PAGE_SIZE_X];
	};

	typedef	std::vector <std::string>	Line;
	typedef	std::vector <Line>	Sheet;
	typedef	std::vector	<CharDataInfo>	InfoList;
	typedef	std::map <CharDataInfo, InfoList::size_type>	MapInfoIndex;
	typedef	std::vector <InfoList::size_type>	RefList;
	typedef	std::map <ZoneContent, long>	ZoneMapIndex;
	typedef	bool	TableBreakGrapheme [GraphemeBreakProp_NBR_ELT] [GraphemeBreakProp_NBR_ELT];
	typedef	bool	TableBreakWord [WordBreakPropTablePrev_NBR_ELT] [WordBreakPropTableNext_NBR_ELT];

	typedef	std::map <int, const char *>	ValueNameMap;

	template <class T>
	class Elt
	{
	public:
		const char *	_name_0;
		T					_value;
	};

	static int		read_file_as_string (std::string &data, const char unicode_data_txt_filename_0 []);
	static void		conv_semicolon_file_as_string_to_sheet (Sheet &sheet, const std::string &data);
	static void		remove_comments (Sheet &sheet);
	static void		remove_empty_lines (Sheet &sheet);
	static void		extract_main_data (InfoList &info_list, const Sheet &sheet);
	static void		extract_grapheme_break_data (InfoList &info_list, const Sheet &sheet);
	static void		extract_line_break_data (InfoList &info_list, const Sheet &sheet);
	static void		extract_word_break_data (InfoList &info_list, const Sheet &sheet);
	static void		shrink_char_info_set (InfoList &info_list, RefList &ref_list);
	static void		compact_data (std::vector <long> &index_page_list, std::vector <long> &index_info_list, const InfoList &info_list, const RefList &ref_list);
	static void		build_table_break_grapheme (TableBreakGrapheme &table);
	static void		build_table_break_word (TableBreakWord &table);
	static void		print_begin (std::string &source);
	static void		print_info_unique_list (std::string &source, const InfoList &info_list);
	static void		print_index_info_list (std::string &source, const std::vector <long> &index_info_list);
	static void		print_index_page_list (std::string &source, const std::vector <long> &index_page_list);
	static void		print_table_break_grapheme (std::string &source, const TableBreakGrapheme table_break_grapheme);
	static void		print_table_break_word (std::string &source, const TableBreakWord table_break_word);
	static void		print_end (std::string &source);
	static void		detect_range (char32_t &range_first, char32_t &range_last, const char field_0 []);

	static const Elt <Categ>
						_categ_list [];
	static const Elt <GraphemeBreakProp>
						_grapheme_break_prop_list [];
	static const Elt <LineBreakProp>
						_line_break_prop_list [];
	static const Elt <WordBreakProp>
						_word_break_prop_list [];
	static const Elt <BidiClass>
						_bidi_class_list [];
	static const CharDataInfo
						_not_assigned;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						CharDataBuilder ();
						CharDataBuilder (const CharDataBuilder &other);
	CharDataBuilder &
						operator = (const CharDataBuilder &other);
	bool				operator == (const CharDataBuilder &other);
	bool				operator != (const CharDataBuilder &other);

};	// class CharDataBuilder



}	// namespace unicode
}	// namespace txt
}	// namespace fstb



//#include	"fstb/txt/unicode/CharDataBuilder.hpp"



#endif	// fstb_txt_unicode_CharDataBuilder_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
