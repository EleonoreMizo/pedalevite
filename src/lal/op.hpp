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

#include <numeric>

#include <cassert>



namespace lal
{



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename T, typename F>
void	op_unary (MatResizableInterface <T> &dst, const MatInterface <T> &other, F fnc)
{
	const int      rows = other.get_rows ();
	const int      cols = other.get_cols ();
	dst.resize (rows, cols);

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



template <typename T, typename F>
void	op_binary (MatResizableInterface <T> &dst, const MatInterface <T> &lhs, const MatInterface <T> &rhs, F fnc)
{
	const int      rows = lhs.get_rows ();
	const int      cols = lhs.get_cols ();
	assert (rows == rhs.get_rows ());
	assert (cols == rhs.get_cols ());
	dst.resize (rows, cols);

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
const MatView <T>	vec_to_matview (const std::vector <T> &v, Dir dir)
{
	assert (dir != Dir::D);

	const int      len  = int (v.size ());
	const int      rows = (dir == Dir::V) ? len :   1;
	const int      cols = (dir == Dir::V) ?   1 : len;
	MatView <T>    view;
	view.setup (rows, cols, true, v.data (), 1);

	return view;
}



template <typename T>
void	fill (MatResizableInterface <T> &dst, T val)
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
void	transpose (MatResizableInterface <T> &dst, const MatInterface <T> &other)
{
	const int      rows = other.get_cols (); // Inverted on purpose
	const int      cols = other.get_rows (); // --"--
	dst.resize (rows, cols);

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



template <typename T>
void	copy (MatResizableInterface <T> &dst, const MatInterface <T> &other)
{
	op_unary (dst, other, [] (T x) { return x; });
}



template <typename T>
void	neg (MatResizableInterface <T> &dst, const MatInterface <T> &other)
{
	op_unary (dst, other, [] (T x) { return -x; });
}



template <typename T>
void	add (MatResizableInterface <T> &dst, const MatInterface <T> &lhs, T rhs)
{
	op_unary (dst, lhs, [rhs] (T x) { return x + rhs; });
}



template <typename T>
void	add (MatResizableInterface <T> &dst, const MatInterface <T> &lhs, const MatInterface <T> &rhs)
{
	op_binary (dst, lhs, rhs, [] (T x1, T x2) { return x1 + x2; });
}



template <typename T>
void	sub (MatResizableInterface <T> &dst, const MatInterface <T> &lhs, const MatInterface <T> &rhs)
{
	op_binary (dst, lhs, rhs, [] (T x1, T x2) { return x1 - x2; });
}



template <typename T>
void	mul (MatResizableInterface <T> &dst, const MatInterface <T> &lhs, T rhs)
{
	op_unary (dst, lhs, [rhs] (T x) { return x * rhs; });
}



template <typename T>
void	mul (MatResizableInterface <T> &dst, const MatInterface <T> &lhs, const MatInterface <T> &rhs)
{
	const int      rows = lhs.get_rows ();
	const int      cols = rhs.get_cols ();
	const int      len  = lhs.get_cols ();
	assert (len == rhs.get_rows ());
	dst.resize (rows, cols);

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



// dst = lhs * transpose (rhs)
template <typename T>
void	mul_transp (MatResizableInterface <T> &dst, const MatInterface <T> &lhs, const MatInterface <T> &rhs)
{
	const int      rows = lhs.get_rows ();
	const int      cols = rhs.get_rows ();
	const int      len  = lhs.get_cols ();
	assert (len == rhs.get_cols ());
	dst.resize (rows, cols);

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
			T              sum   = T (0);
			for (int k = 0; k < len; ++k)
			{
				sum += src1_ptr [pos_1 + k] * src2_ptr [pos_2 + k];
			}
			dst_ptr [c] = sum;
			pos_2 += stride_s2;
		}

		dst_ptr += stride_d;
	}
}



// LU decomposition of a matrix, given an entierly known list of pivots
// mat is the square matrix to decompose, in-place.
// r_arr and c_arr are pre-filled reordering vectors for the rows and columns,
// same size as mat.
// As output, mat is the L and U combination, without the identity part for L
// The result cells keep the same order as mat, so the matrices do not look
// triangular. r_arr and c_arr must be used to retrieve the triangular forms.
template <typename T>
void	decompose_lu (MatInterface <T> &mat, const std::vector <int> &r_arr, const std::vector <int> &c_arr)
{
	const int      n = mat.get_rows ();
	assert (mat.get_cols () == n);
	assert (int (r_arr.size ()) == n);
	assert (int (c_arr.size ()) == n);
	assert (std::accumulate (r_arr.begin (), r_arr.end (), 0) == n * (n - 1) / 2);
	assert (std::accumulate (c_arr.begin (), c_arr.end (), 0) == n * (n - 1) / 2);

	T *            mat_ptr = mat.get_data ();
	const int      stride  = mat.get_stride ();

	for (int k = 0; k < n - 1; ++k)
	{
		// The pivot
		const int      r_k     = r_arr [k];
		const int      c_k     = c_arr [k];
		T *            r_k_ptr = mat_ptr + r_k * stride;
		const T        ukk_inv = T (1) / r_k_ptr [c_k];

		// Subtract the other rows
		for (int j = k + 1; j < n; ++j)
		{
			const int      r_j     = r_arr [j];
			T *            r_j_ptr = mat_ptr + r_j * stride;

			// L
			const T        ljk = r_j_ptr [c_k] * ukk_inv;
			r_j_ptr [c_k] = ljk;

			// U
			for (int d = k + 1; d < n; ++d)
			{
				const int      c_d = c_arr [d];
				r_j_ptr [c_d] -= ljk * r_k_ptr [c_d];
			}
		}
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
void	traverse_lu (std::vector <T> &x, std::vector <T> &y, const std::vector <T> &b, const MatInterface <T> &lu, const std::vector <int> &r_arr, const std::vector <int> &c_arr)
{
	const int      n = lu.get_rows ();
	assert (lu.get_cols () == n);
	assert (int (b.size ()) == n);
	assert (int (r_arr.size ()) == n);
	assert (int (c_arr.size ()) == n);
	assert (std::accumulate (r_arr.begin (), r_arr.end (), 0) == n * (n - 1) / 2);
	assert (std::accumulate (c_arr.begin (), c_arr.end (), 0) == n * (n - 1) / 2);

	x.resize (n);
	y.resize (n);

	const T *      lu_ptr = lu.get_data ();
	const int      stride = lu.get_stride ();

	// Down: L * y = b
	for (int i = 0; i < n; ++i)
	{
		const int      r_i     = r_arr [i];
		const T *      r_i_ptr = lu_ptr + r_i * stride;

		T              sum     = b [r_i];
		for (int j = 0; j < i; ++j)
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
		const T *      r_i_ptr = lu_ptr + r_i * stride;

		T              sum     = y [i];
		for (int j = i + 1; j < n; ++j)
		{
			const int      c_j = c_arr [j];
			sum -= x [c_j] * r_i_ptr [c_j];
		}
		const int      c_i = c_arr [i];
		assert (r_i_ptr [c_i] != 0);
		x [c_i] = sum / r_i_ptr [c_i];
	}
}



// lu is the result of the decompose_lu() call.
// inv is the result of the inversion.
// Other parameters are the same as traverse_lu().
template <typename T>
void	invert (MatResizableInterface <T> &inv, std::vector <T> &y, const MatInterface <T> &lu, const std::vector <int> &r_arr, const std::vector <int> &c_arr)
{
	const int      n = lu.get_rows ();
	assert (lu.get_cols () == n);
	assert (int (r_arr.size ()) == n);
	assert (int (c_arr.size ()) == n);
	assert (std::accumulate (r_arr.begin (), r_arr.end (), 0) == n * (n - 1) / 2);
	assert (std::accumulate (c_arr.begin (), c_arr.end (), 0) == n * (n - 1) / 2);

	inv.resize (n, n);
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
