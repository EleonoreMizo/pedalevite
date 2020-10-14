/*****************************************************************************

        op.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (lal_op_HEADER_INCLUDED)
#define lal_op_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "lal/Dir.h"
#include "lal/MatResizableInterface.h"
#include "lal/MatView.h"

#include <vector>



namespace lal
{



template <typename T>
MatView <T>    vec_to_matview (std::vector <T> &v, Dir dir);
template <typename T>
const MatView <T> vec_to_matview (const std::vector <T> &v, Dir dir);
template <typename T>
void           fill (MatResizableInterface <T> &dst, T val);
template <typename T>
void           transpose (MatResizableInterface <T> &dst, const MatInterface <T> &other);
template <typename T>
void           copy (MatResizableInterface <T> &dst, const MatInterface <T> &other);
template <typename T>
void           neg (MatResizableInterface <T> &dst, const MatInterface <T> &other);
template <typename T>
void           add (MatResizableInterface <T> &dst, const MatInterface <T> &lhs, T rhs);
template <typename T>
void           add (MatResizableInterface <T> &dst, const MatInterface <T> &lhs, const MatInterface <T> &rhs);
template <typename T>
void           sub (MatResizableInterface <T> &dst, const MatInterface <T> &lhs, const MatInterface <T> &rhs);
template <typename T>
void           mul (MatResizableInterface <T> &dst, const MatInterface <T> &lhs, T rhs);
template <typename T>
void           mul (MatResizableInterface <T> &dst, const MatInterface <T> &lhs, const MatInterface <T> &rhs);
template <typename T>
void           mul_transp (MatResizableInterface <T> &dst, const MatInterface <T> &lhs, const MatInterface <T> &rhs);

template <typename T>
void           decompose_lu (MatInterface <T> &mat, const std::vector <int> &r_arr, const std::vector <int> &c_arr);
template <typename T>
void           traverse_lu (std::vector <T> &x, std::vector <T> &y, const std::vector <T> &b, const MatInterface <T> &mat, const std::vector <int> &r_arr, const std::vector <int> &c_arr);
template <typename T>
void           invert (MatResizableInterface <T> &inv, std::vector <T> &y, const MatInterface <T> &mat, const std::vector <int> &r_arr, const std::vector <int> &c_arr);



}  // namespace lal



#include "lal/op.hpp"



#endif   // lal_op_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
