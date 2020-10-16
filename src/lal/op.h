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
#include "lal/MatViewConst.h"

#include <vector>



namespace lal
{

template <typename T>
inline void    res_assign (T &y, T x) { y = x; }
template <typename T>
inline void    res_add (T &y, T x) { y += x; }
template <typename T>
inline void    res_sub (T &y, T x) { y -= x; }

template <typename T>
MatView <T>    vec_to_matview (std::vector <T> &v, Dir dir);
template <typename T>
MatViewConst <T> vec_to_matview (const std::vector <T> &v, Dir dir);
template <typename T, typename D>
void           vec_to_mat (D &dst, const std::vector <T> &v, Dir dir);

template <typename T>
T              norm_inf (const std::vector <T> &v);

template <typename T>
void           fill (MatInterface <T> &dst, T val);
template <typename T>
void           fill (std::vector <T> &dst, T val);
template <typename T, typename D>
void           transpose (D &dst, const MatConstInterface <T> &other);
template <typename T, typename D>
void           copy (D &dst, const MatConstInterface <T> &other);
template <typename T>
void           copy (std::vector <T> &dst, const std::vector <T> &other);

template <typename T>
void           mul_cw (std::vector <T> &dst, const std::vector <T> &lhs, const std::vector <T> &rhs);

template <typename T, typename D>
void           neg (D &dst, const MatConstInterface <T> &other);
template <typename T, typename D>
void           add (D &dst, const MatConstInterface <T> &lhs, T rhs);
template <typename T>
void           add (std::vector <T> &dst, const std::vector <T> &lhs, const std::vector <T> &rhs);
template <typename T>
void           sub (std::vector <T> &dst, const std::vector <T> &lhs, const std::vector <T> &rhs);
template <typename T, typename D>
void           add (D &dst, const MatConstInterface <T> &lhs, const MatConstInterface <T> &rhs);
template <typename T, typename D>
void           sub (D &dst, const MatConstInterface <T> &lhs, const MatConstInterface <T> &rhs);
template <typename T>
void           mul (std::vector <T> &dst, const std::vector <T> &lhs, T rhs);
template <typename T, typename D>
void           mul (D &dst, const MatConstInterface <T> &lhs, T rhs);
template <typename T, typename D>
void           mul_lhs_dia (D &dst, const std::vector <T> &lhs, const MatConstInterface <T> &rhs);
template <typename T>
void           mul (std::vector <T> &dst, const MatConstInterface <T> &lhs, const std::vector <T> &rhs);
template <typename T, typename D>
void           mul (D &dst, const MatConstInterface <T> &lhs, const MatConstInterface <T> &rhs);
template <typename T, typename D>
void           mul_transp_lhs_dia (D &dst, const MatConstInterface <T> &lhs, const std::vector <T> &rhs);
template <typename T, typename D>
void           mul_transp_lhs (D &dst, const MatConstInterface <T> &lhs, const MatConstInterface <T> &rhs);
template <typename T, typename D>
void           mul_transp_rhs (D &dst, const MatConstInterface <T> &lhs, const MatConstInterface <T> &rhs, decltype (res_assign <T>) fnc = res_assign <T>);

template <typename T>
void           decompose_lu (MatInterface <T> &mat, const std::vector <int> &r_arr, const std::vector <int> &c_arr);
template <typename T>
void           traverse_lu (std::vector <T> &x, std::vector <T> &y, const std::vector <T> &b, const MatConstInterface <T> &mat, const std::vector <int> &r_arr, const std::vector <int> &c_arr);
template <typename T, typename D>
void           invert (D &inv, std::vector <T> &y, const MatConstInterface <T> &mat, const std::vector <int> &r_arr, const std::vector <int> &c_arr);



}  // namespace lal



#include "lal/op.hpp"



#endif   // lal_op_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
