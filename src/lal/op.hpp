/*****************************************************************************

        op.hpp
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (lal_op_CODEHEADER_INCLUDED)
#define lal_op_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "lal/Dir.h"

#include <algorithm>
#include <numeric>

#include <cassert>
#include <cmath>



namespace lal
{



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
void	resize_check (MatResizableInterface <T> &dst, int rows, int cols)
{
	dst.resize (rows, cols);
}

template <typename T>
#if defined (NDEBUG)
void	resize_check (MatInterface <T> &, int, int) { }
#else
void	resize_check (MatInterface <T> &dst, int rows, int cols)
{
	assert (dst.get_rows () == rows);
	assert (dst.get_cols () == cols);
}
#endif



template <typename D, typename T, typename F>
void	op_unary (D &dst, const MatConstInterface <T> &other, F fnc)
{
	const int      rows = other.get_rows ();
	const int      cols = other.get_cols ();
	resize_check (dst, rows, cols);

	T *            dst_ptr  = dst.get_data ();
	const T *      src_ptr  = other.get_data ();
	const int      stride_d = dst.get_stride ();
	const int      stride_s = other.get_stride ();

	for (int r = 0; r < rows; ++r)
	{
		for (int c = 0; c < cols; ++c)
		{
			dst_ptr [c] = fnc (src_ptr [c]);
		}
		dst_ptr += stride_d;
		src_ptr += stride_s;
	}
}



template <typename D, typename T, typename F>
void	op_binary (D &dst, const MatConstInterface <T> &lhs, const MatConstInterface <T> &rhs, F fnc)
{
	const int      rows = lhs.get_rows ();
	const int      cols = lhs.get_cols ();
	assert (rows == rhs.get_rows ());
	assert (cols == rhs.get_cols ());
	resize_check (dst, rows, cols);

	T *            dst_ptr   = dst.get_data ();
	const T *      src1_ptr  = lhs.get_data ();
	const T *      src2_ptr  = rhs.get_data ();
	const int      stride_d  = dst.get_stride ();
	const int      stride_s1 = lhs.get_stride ();
	const int      stride_s2 = rhs.get_stride ();

	for (int r = 0; r < rows; ++r)
	{
		for (int c = 0; c < cols; ++c)
		{
			dst_ptr [c] = fnc (src1_ptr [c], src2_ptr [c]);
		}
		dst_ptr  += stride_d;
		src1_ptr += stride_s1;
		src2_ptr += stride_s2;
	}
}



template <typename T, typename F>
void	op_unary (std::vector <T> &dst, const std::vector <T> &other, F fnc)
{
	const int      len = int (other.size ());
	dst.resize (len);

	for (int pos = 0; pos < len; ++pos)
	{
		dst [pos] = fnc (other [pos]);
	}
}



template <typename T, typename F>
void	op_binary (std::vector <T> &dst, const std::vector <T> &lhs, const std::vector <T> &rhs, F fnc)
{
	const int      len = int (lhs.size ());
	assert (int (rhs.size ()) == len);
	dst.resize (len);

	for (int pos = 0; pos < len; ++pos)
	{
		dst [pos] = fnc (lhs [pos], rhs [pos]);
	}
}



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T>
MatView <T>	vec_to_matview (std::vector <T> &v, Dir dir)
{
	assert (dir != Dir::D);

	const int      len  = int (v.size ());
	const int      rows = (dir == Dir::V) ? len :   1;
	const int      cols = (dir == Dir::V) ?   1 : len;
	MatView <T>    view;
	view.setup (rows, cols, false, v.data (), 1);

	return view;
}



template <typename T>
MatViewConst <T>	vec_to_matview (const std::vector <T> &v, Dir dir)
{
	assert (dir != Dir::D);

	const int      len  = int (v.size ());
	const int      rows = (dir == Dir::V) ? len :   1;
	const int      cols = (dir == Dir::V) ?   1 : len;
	MatView <T>    view;
	view.setup (rows, cols, true, v.data (), 1);

	return view;
}



template <typename T, typename D>
void	vec_to_mat (D &dst, const std::vector <T> &v, Dir dir)
{
	const int      len = int (v.size ());
	int            dr  = 0;
	int            dc  = 0;
	switch (dir)
	{
	case Dir::V: dr = 1; dc = 0; break;
	case Dir::H: dr = 0; dc = 1; break;
	case Dir::D: dr = 1; dc = 1; break;
	}
	const int      rows    = dr * len;
	const int      cols    = dc * len;
	resize_check (dst, rows, cols);
	fill (dst, T (0));
	T *            dst_ptr = dst.get_data ();
	const int      stride  = dst.get_stride ();
	const int      step    = dr * stride + dc;
	for (int k = 0; k < len; ++k)
	{
		*dst_ptr = v [k];
		dst_ptr += step;
	}
}



template <typename T>
T	norm_inf (const std::vector <T> &v)
{
	T              norm { 0 };

	std::for_each (v.begin (), v.end (), [&norm] (T x) {
		norm = std::max (norm, T (fabs (x)));
	});

	return norm;
}




template <typename T>
void	fill (MatInterface <T> &dst, T val)
{
	const int      rows     = dst.get_rows ();
	const int      cols     = dst.get_cols ();
	T *            dst_ptr  = dst.get_data ();
	const int      stride_d = dst.get_stride ();

	for (int r = 0; r < rows; ++r)
	{
		for (int c = 0; c < cols; ++c)
		{
			dst_ptr [c] = val;
		}
		dst_ptr += stride_d;
	}
}



template <typename T>
void	fill (std::vector <T> &dst, T val)
{
	std::fill (dst.begin (), dst.end (), val);
}



template <typename T, typename D>
void	transpose (D &dst, const MatConstInterface <T> &other)
{
	const int      rows = other.get_cols (); // Inverted on purpose
	const int      cols = other.get_rows (); // --"--
	resize_check (dst, rows, cols);

	T *            dst_ptr  = dst.get_data ();
	const T *      src_ptr  = other.get_data ();
	const int      stride_d = dst.get_stride ();
	const int      stride_s = other.get_stride ();

	for (int r = 0; r < rows; ++r)
	{
		for (int c = 0; c < cols; ++c)
		{
			dst_ptr [c] = *src_ptr;
			src_ptr += stride_s;
		}
		dst_ptr += stride_d;
		src_ptr += 1 - stride_s * cols;
	}
}



template <typename T, typename D>
void	copy (D &dst, const MatConstInterface <T> &other)
{
	op_unary (dst, other, [] (T x) { return x; });
}



template <typename T>
void	copy (std::vector <T> &dst, const std::vector <T> &other)
{
	op_unary (dst, other, [] (T x) { return x; });
}



template <typename T>
void	mul_cw (std::vector <T> &dst, const std::vector <T> &lhs, const std::vector <T> &rhs)
{
	op_binary (dst, lhs, rhs, [] (T x1, T x2) { return x1 * x2; });
}



template <typename T, typename D>
void	neg (D &dst, const MatConstInterface <T> &other)
{
	op_unary (dst, other, [] (T x) { return -x; });
}



template <typename T, typename D>
void	add (D &dst, const MatConstInterface <T> &lhs, T rhs)
{
	op_unary (dst, lhs, [rhs] (T x) { return x + rhs; });
}



template <typename T>
void	add (std::vector <T> &dst, const std::vector <T> &lhs, const std::vector <T> &rhs)
{
	op_binary (dst, lhs, rhs, [] (T x1, T x2) { return x1 + x2; });
}



template <typename T>
void	sub (std::vector <T> &dst, const std::vector <T> &lhs, const std::vector <T> &rhs)
{
	op_binary (dst, lhs, rhs, [] (T x1, T x2) { return x1 - x2; });
}



template <typename T, typename D>
void	add (D &dst, const MatConstInterface <T> &lhs, const MatConstInterface <T> &rhs)
{
	op_binary (dst, lhs, rhs, [] (T x1, T x2) { return x1 + x2; });
}



template <typename T, typename D>
void	sub (D &dst, const MatConstInterface <T> &lhs, const MatConstInterface <T> &rhs)
{
	op_binary (dst, lhs, rhs, [] (T x1, T x2) { return x1 - x2; });
}



template <typename T>
void	mul (std::vector <T> &dst, const std::vector <T> &lhs, T rhs)
{
	op_unary (dst, lhs, [rhs] (T x) { return x * rhs; });
}



template <typename T, typename D>
void	mul (D &dst, const MatConstInterface <T> &lhs, T rhs)
{
	op_unary (dst, lhs, [rhs] (T x) { return x * rhs; });
}



template <typename T, typename D>
void	mul_lhs_dia (D &dst, const std::vector <T> &lhs, const MatConstInterface <T> &rhs)
{
	const int      len = lhs.get_rows ();
	assert (int (lhs.get_cols ()) == len);
	assert (int (rhs.size ()) == len);
	resize_check (dst, len, len);

	T *            dst_ptr   = dst.get_data ();
	const T *      src1_ptr  = lhs.get_data ();
	const int      stride_d  = dst.get_stride ();
	const int      stride_s1 = lhs.get_stride ();

	for (int r = 0; r < len; ++r)
	{
		for (int c = 0; c < len; ++c)
		{
			dst_ptr [c] = src1_ptr [c] * rhs [c];
		}
		dst_ptr  += stride_d;
		src1_ptr += stride_s1;
	}
}



// vertical vect = mat * vertical vect
template <typename T>
void	mul (std::vector <T> &dst, const MatConstInterface <T> &lhs, const std::vector <T> &rhs)
{
	const int      rows = lhs.get_rows ();
	const int      cols = lhs.get_cols ();
	assert (int (rhs.size ()) == cols);
	dst.resize (rows);

	const T *      src1_ptr  = lhs.get_data ();
	const int      stride_s1 = lhs.get_stride ();

	for (int r = 0; r < rows; ++r)
	{
		T           sum = T (0);
		for (int c = 0; c < cols; ++c)
		{
			sum += src1_ptr [c] * rhs [c];
		}
		dst [r] = sum;
		src1_ptr += stride_s1;
	}
}



template <typename T, typename D>
void	mul (D &dst, const MatConstInterface <T> &lhs, const MatConstInterface <T> &rhs)
{
	const int      rows = lhs.get_rows ();
	const int      cols = rhs.get_cols ();
	const int      len  = lhs.get_cols ();
	assert (len == rhs.get_rows ());
	resize_check (dst, rows, cols);

	T *            dst_ptr   = dst.get_data ();
	const T *      src1_ptr  = lhs.get_data ();
	const T *      src2_ptr  = rhs.get_data ();
	const int      stride_d  = dst.get_stride ();
	const int      stride_s1 = lhs.get_stride ();
	const int      stride_s2 = rhs.get_stride ();

	for (int r = 0; r < rows; ++r)
	{
		const int      pos_1 = r * stride_s1;
		for (int c = 0; c < cols; ++c)
		{
			int            pos_2 = c;
			T              sum   = T (0);
			for (int k = 0; k < len; ++k)
			{
				sum += src1_ptr [pos_1 + k] * src2_ptr [pos_2];
				pos_2 += stride_s2;
			}
			dst_ptr [c] = sum;
		}

		dst_ptr += stride_d;
	}
}



// dst = transpose (lhs) * rhs_as_diagonal_matrix
template <typename T, typename D>
void	mul_transp_lhs_dia (D &dst, const MatConstInterface <T> &lhs, const std::vector <T> &rhs)
{
	const int      rows = lhs.get_cols ();
	const int      cols = int (rhs.size ());
	assert (int (rhs.size ()) == lhs.get_rows ());
	resize_check (dst, rows, cols);

	T *            dst_ptr   = dst.get_data ();
	const T *      src1_ptr  = lhs.get_data ();
	const int      stride_d  = dst.get_stride ();
	const int      stride_s1 = lhs.get_stride ();

	for (int r = 0; r < rows; ++r)
	{
		const T *      src1r_ptr = src1_ptr + r;

		for (int c = 0; c < cols; ++c)
		{
			dst_ptr [c] = (*src1r_ptr) * rhs [c];
			src1r_ptr += stride_s1;
		}

		dst_ptr += stride_d;
	}
}



// dst = transpose (lhs) * rhs
template <typename T, typename D>
void	mul_transp_lhs (D &dst, const MatConstInterface <T> &lhs, const MatConstInterface <T> &rhs)
{
	const int      rows = lhs.get_cols ();
	const int      cols = rhs.get_cols ();
	const int      len  = lhs.get_rows ();
	assert (len == rhs.get_rows ());
	resize_check (dst, rows, cols);

	T *            dst_ptr   = dst.get_data ();
	const T *      src1_ptr  = lhs.get_data ();
	const T *      src2_ptr  = rhs.get_data ();
	const int      stride_d  = dst.get_stride ();
	const int      stride_s1 = lhs.get_stride ();
	const int      stride_s2 = rhs.get_stride ();

	for (int r = 0; r < rows; ++r)
	{
		for (int c = 0; c < cols; ++c)
		{
			T              sum       = T (0);
			const T *      src1r_ptr = src1_ptr + r;
			const T *      src2c_ptr = src2_ptr + c;
			for (int k = 0; k < len; ++k)
			{
				sum += *src1r_ptr * *src2c_ptr;
				src1r_ptr += stride_s1;
				src2c_ptr += stride_s2;
			}
			dst_ptr [c] = sum;
		}

		dst_ptr += stride_d;
	}
}



// fnc (dst, lhs * transpose (rhs))
// where fnc is an assignment function (res_assign/add/sub)
template <typename T, typename D>
void	mul_transp_rhs (D &dst, const MatConstInterface <T> &lhs, const MatConstInterface <T> &rhs, decltype (res_assign <T>) fnc)
{
	const int      rows = lhs.get_rows ();
	const int      cols = rhs.get_rows ();
	const int      len  = lhs.get_cols ();
	assert (len == rhs.get_cols ());
	resize_check (dst, rows, cols);

	T *            dst_ptr   = dst.get_data ();
	const T *      src1_ptr  = lhs.get_data ();
	const T *      src2_ptr  = rhs.get_data ();
	const int      stride_d  = dst.get_stride ();
	const int      stride_s1 = lhs.get_stride ();
	const int      stride_s2 = rhs.get_stride ();

	for (int r = 0; r < rows; ++r)
	{
		const int      pos_1 = r * stride_s1;
		int            pos_2 = 0;
		for (int c = 0; c < cols; ++c)
		{
			T              sum = T (0);
			for (int k = 0; k < len; ++k)
			{
				sum += src1_ptr [pos_1 + k] * src2_ptr [pos_2 + k];
			}
			fnc (dst_ptr [c], sum);
			pos_2 += stride_s2;
		}

		dst_ptr += stride_d;
	}
}



// LU decomposition of a matrix, given an entierly known list of pivots
// mat is the square matrix to decompose, in-place.
// fnc_r and fnc_c are functions for reordering the rows and vectors
// (pivot coordinate mapping), for rows and columns respectively.
// Prototype: int f (int x)
// As output, mat is the L and U combination, without the identity part for L
// The result cells keep the same order as mat, so the matrices do not look
// triangular. fnc_r and fnc_c must be used to retrieve the triangular forms.
template <typename T, typename FR, typename FC>
void	decompose_lu (MatInterface <T> &mat, FR fnc_r, FC fnc_c)
{
	const int      n = mat.get_rows ();
	assert (mat.get_cols () == n);

	T *            mat_ptr = mat.get_data ();
	const int      stride  = mat.get_stride ();

	for (int k = 0; k < n - 1; ++k)
	{
		// The pivot
		const int      r_k     = fnc_r (k);
		const int      c_k     = fnc_c (k);
		T *            r_k_ptr = mat_ptr + r_k * stride;
		const T        ukk_inv = T (1) / r_k_ptr [c_k];

		// Subtract the other rows
		for (int j = k + 1; j < n; ++j)
		{
			const int      r_j     = fnc_r (j);
			T *            r_j_ptr = mat_ptr + r_j * stride;

			// L
			const T        ljk = r_j_ptr [c_k] * ukk_inv;
			r_j_ptr [c_k] = ljk;

			// U
			for (int d = k + 1; d < n; ++d)
			{
				const int      c_d = fnc_c (d);
				r_j_ptr [c_d] -= ljk * r_k_ptr [c_d];
			}
		}
	}
}



// r_arr and c_arr are pre-filled reordering vectors for the rows and columns,
// same size as mat.
template <typename T>
void	decompose_lu (MatInterface <T> &mat, const std::vector <int> &r_arr, const std::vector <int> &c_arr)
{
#if ! defined (NDEBUG)
	const int      n = mat.get_rows ();
	assert (mat.get_cols () == n);
	assert (int (r_arr.size ()) == n);
	assert (int (c_arr.size ()) == n);
	assert (std::accumulate (r_arr.begin (), r_arr.end (), 0) == n * (n - 1) / 2);
	assert (std::accumulate (c_arr.begin (), c_arr.end (), 0) == n * (n - 1) / 2);
#endif // NDEBUG

	decompose_lu (
		mat,
		[&r_arr] (int r) { return r_arr [r]; },
		[&c_arr] (int c) { return c_arr [c]; }
	);
}



// Solves L * U * x = b with implicit row and column reordering
// lu is the result of the decompose_lu() call.
// b is the vector of the same number of rows as lu, whose rows are reorderd
// with fnc_c.
// y is a temporary vector of the same size as b
// fnc_r and fnc_c should be the same as the decompose_lu() call.
// Result: a vector x, same size and same row ordering as b (fnc_c).
template <typename T, typename FR, typename FC>
void	traverse_lu (std::vector <T> &x, std::vector <T> &y, const std::vector <T> &b, const MatConstInterface <T> &lu, FR fnc_r, FC fnc_c)
{
	const int      n = lu.get_rows ();
	assert (lu.get_cols () == n);
	assert (int (b.size ()) == n);

	x.resize (n);
	y.resize (n);

	const T *      lu_ptr = lu.get_data ();
	const int      stride = lu.get_stride ();

	// Down: L * y = b
	for (int i = 0; i < n; ++i)
	{
		const int      r_i     = fnc_r (i);
		const T *      r_i_ptr = lu_ptr + r_i * stride;

		T              sum     = b [r_i];
		for (int j = 0; j < i; ++j)
		{
			const int      c_j = fnc_c (j);
			sum -= y [j] * r_i_ptr [c_j];
		}
		y [i] = sum;
	}

	// Up: U * x = y
	for (int i = n - 1; i >= 0; --i)
	{
		const int      r_i     = fnc_r (i);
		const T *      r_i_ptr = lu_ptr + r_i * stride;

		T              sum     = y [i];
		for (int j = i + 1; j < n; ++j)
		{
			const int      c_j = fnc_c (j);
			sum -= x [c_j] * r_i_ptr [c_j];
		}
		const int      c_i = fnc_c (i);
		assert (r_i_ptr [c_i] != 0);
		x [c_i] = sum / r_i_ptr [c_i];
	}
}



// Solves L * U * x = b with implicit row and column reordering
// lu is the result of the decompose_lu() call.
// b is the vector of the same number of rows as lu, whose rows are reorderd
// with c_arr.
// y is a temporary vector of the same size as b
// r_arr and c_arr should be the same as the decompose_lu() call.
// Result: a vector x, same size and same row ordering as b (c_arr).
template <typename T>
void	traverse_lu (std::vector <T> &x, std::vector <T> &y, const std::vector <T> &b, const MatConstInterface <T> &lu, const std::vector <int> &r_arr, const std::vector <int> &c_arr)
{
#if ! defined (NDEBUG)
	const int      n = lu.get_rows ();
	assert (lu.get_cols () == n);
	assert (int (b.size ()) == n);
	assert (int (r_arr.size ()) == n);
	assert (int (c_arr.size ()) == n);
	assert (std::accumulate (r_arr.begin (), r_arr.end (), 0) == n * (n - 1) / 2);
	assert (std::accumulate (c_arr.begin (), c_arr.end (), 0) == n * (n - 1) / 2);
#endif // NDEBUG

	traverse_lu (
		x, y, b, lu,
		[&r_arr] (int r) { return r_arr [r]; },
		[&c_arr] (int c) { return c_arr [c]; }
	);
}



// lu is the result of the decompose_lu() call.
// inv is the result of the inversion.
// Other parameters are the same as traverse_lu().
template <typename T, typename D>
void	invert (D &inv, std::vector <T> &y, const MatConstInterface <T> &lu, const std::vector <int> &r_arr, const std::vector <int> &c_arr)
{
	const int      n = lu.get_rows ();
	assert (lu.get_cols () == n);
	assert (int (r_arr.size ()) == n);
	assert (int (c_arr.size ()) == n);
	assert (std::accumulate (r_arr.begin (), r_arr.end (), 0) == n * (n - 1) / 2);
	assert (std::accumulate (c_arr.begin (), c_arr.end (), 0) == n * (n - 1) / 2);

	resize_check (inv, n, n);
	y.resize (n);

	const T *      lu_ptr   = lu.get_data ();
	const int      stride_l = lu.get_stride ();

	T *            inv_ptr  = inv.get_data ();
	const int      stride_i = inv.get_stride ();

	// Each unit vector
	for (int k = 0; k < n; ++k)
	{
		const int      r_k = r_arr [k];
		const int      c_k = c_arr [k];

		// Down: L * y = b
		std::fill (y.begin (), y.begin () + k, T (0));
		y [k] = 1;
		for (int i = k + 1; i < n; ++i)
		{
			const int      r_i     = r_arr [i];
			const T *      r_i_ptr = lu_ptr + r_i * stride_l;

			T              sum     = -r_i_ptr [c_k];
			for (int j = k + 1; j < i; ++j)
			{
				const int      c_j = c_arr [j];
				sum -= y [j] * r_i_ptr [c_j];
			}
			y [i] = sum;
		}

		// Up: U * x = y
		for (int i = n - 1; i >= 0; --i)
		{
			const int      r_i     = r_arr [i];
			const T *      r_i_ptr = lu_ptr + r_i * stride_l;

			T              sum     = y [i];
			for (int j = i + 1; j < n; ++j)
			{
				const int      c_j     = c_arr [j];
				T *            c_j_ptr = inv_ptr + c_j * stride_i;
				sum -= c_j_ptr [r_k] * r_i_ptr [c_j];
			}
			const int      c_i     = c_arr [i];
			T *            c_i_ptr = inv_ptr + c_i * stride_i;
			assert (r_i_ptr [c_i] != 0);
			c_i_ptr [r_k] = sum / r_i_ptr [c_i];
		}
	}
}



}  // namespace lal



#endif   // lal_op_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
