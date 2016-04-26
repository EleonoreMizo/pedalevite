/*****************************************************************************

        Interlocked.hpp
        Author: Laurent de Soras, 2011

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (conc_Interlocked_CODEHEADER_INCLUDED)
#define	conc_Interlocked_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "conc/fnc.h"

#if defined (_MSC_VER)
	#include <intrin.h>
#elif defined (_WIN32) || defined (WIN32) || defined (__WIN32__) || defined (__CYGWIN__) || defined (__CYGWIN32__)
	#define  WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#include <winnt.h>
#elif defined (__APPLE__)
	#include <libkern/OSAtomic.h>
#endif

#include <cassert>



namespace conc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



int32_t	Interlocked::swap (int32_t volatile &dest, int32_t excg)
{
	assert (is_ptr_aligned_nz (&dest));

#if defined (_MSC_VER)

	return (
		_InterlockedExchange (reinterpret_cast <volatile long *> (&dest), excg)
	);

#elif defined (_WIN32) || defined (WIN32) || defined (__WIN32__) || defined (__CYGWIN__) || defined (__CYGWIN32__)

	return (::InterlockedExchange (
		reinterpret_cast <volatile ::LONG *> (&dest),
		::LONG (excg)
	));

#else

	int32_t        old;
	do
	{
		old = dest;
	}
	while (cas (dest, excg, old) != old);

	return (old);

#endif
}



int32_t	Interlocked::cas (int32_t volatile &dest, int32_t excg, int32_t comp)
{
	assert (is_ptr_aligned_nz (&dest));

#if defined (_MSC_VER)

	return (_InterlockedCompareExchange (
		reinterpret_cast <volatile long *> (&dest),
		excg,
		comp
	));

#elif defined (_WIN32) || defined (WIN32) || defined (__WIN32__) || defined (__CYGWIN__) || defined (__CYGWIN32__)

	return (::InterlockedCompareExchange (
		reinterpret_cast <volatile ::LONG *> (&dest),
		::LONG (excg),
		::LONG (comp)
	));

#elif defined (__linux__)

	return (__sync_val_compare_and_swap (&dest, comp, excg));

#elif defined (__APPLE__)

	return (::OSAtomicCompareAndSwap32Barrier (
		comp,
		excg,
		const_cast <int32_t *> (reinterpret_cast <int32_t volatile *> (&dest))
	) ? comp : excg);

#else

	#error Unknown platform

#endif
}



int64_t	Interlocked::swap (int64_t volatile &dest, int64_t excg)
{
	assert (is_ptr_aligned_nz (&dest));

	int64_t        old;

#if defined (_WIN32) || defined (WIN32) || defined (__WIN32__) || defined (__CYGWIN__) || defined (__CYGWIN32__)

 #if conc_WORD_SIZE == 64 && defined (_MSC_VER)

	old = _InterlockedExchange64 (&dest, excg);

 #elif defined (_MSC_VER)	// conc_WORD_SIZE / _MSC_VER

	__asm
	{
		push           ebx
		mov            esi, [dest]
		mov            ebx, [dword ptr excg    ]
		mov            ecx, [dword ptr excg + 4]

	cas_loop:
		mov            eax, [esi    ]
		mov            edx, [esi + 4]
		lock cmpxchg8b [esi]
		jnz            cas_loop

		mov            [dword ptr old    ], eax
		mov            [dword ptr old + 4], edx
		pop            ebx
	}

 #else

	do
	{
		old = dest;
	}
	while (cas (dest, excg, old) != old);

 #endif	// conc_WORD_SIZE / _MSC_VER

#else

	do
	{
		old = dest;
	}
	while (cas (dest, excg, old) != old);

#endif

	return (old);
}



int64_t	Interlocked::cas (int64_t volatile &dest, int64_t excg, int64_t comp)
{
	assert (is_ptr_aligned_nz (&dest));

#if defined (_MSC_VER)

	return (_InterlockedCompareExchange64 (&dest, excg, comp));

#elif defined (_WIN32) || defined (WIN32) || defined (__WIN32__) || defined (__CYGWIN__) || defined (__CYGWIN32__)

	// Set to zero just to make the compiler stop complaining about
	// uninitialized variables.
	int64_t        old = 0;
	
	asm volatile (
	"	lea				 %[comp], %%esi			\n"
	"	mov				 (%%esi), %%eax			\n"
	"	mov				4(%%esi), %%edx			\n"
	"	lea				 %[excg], %%esi			\n"
	"	mov				 (%%esi), %%ebx			\n"
	"	mov				4(%%esi), %%ecx			\n"
	"	lea				 %[dest], %%esi		\n"
	"	lock cmpxchg8b	(%%esi)				\n"
	"	lea				 %[old],  %%esi		\n"
	"	mov				%%eax, (%%esi)			\n"
	"	mov				%%edx, 4(%%esi)		\n"
	:
	: [old]  "m" (old)
	, [dest] "m" (dest)
	, [excg] "m" (excg)
	, [comp] "m" (comp)
	: "eax", "ebx", "ecx", "edx", "esi"
	);

	return (old);

#elif defined (__linux__)

	return (__sync_val_compare_and_swap (&dest, comp, excg));

#elif defined (__APPLE__)

	return (::OSAtomicCompareAndSwap64Barrier (
		comp, 
		excg, 
		const_cast <int64_t *> (reinterpret_cast <int64_t volatile *> (&dest))
	) ? comp : excg);

#else

	#error Unknown platform

#endif
}



#if defined (conc_HAS_CAS_128)



void	Interlocked::swap (Data128 &old, volatile Data128 &dest, const Data128 &excg)
{
	assert (is_ptr_aligned_nz (&dest));

#if defined (__APPLE__)

	Data128 *         old_ptr  = &old;
	volatile Data128* dest_ptr = &dest;
	const Data128 *   excg_ptr = &excg;

	asm volatile ( 
	"	mov				 %[excg], %%r9			\n"

	"	push				%%rbx						\n"
	"	mov				%[dest], %%rsi			\n"
	"	movq				(%%r9), %%rbx			\n"
	"	mov				8(%%r9), %%rcx			\n"

	"cas_loop%=:	\n"
	"	mov				(%%rsi), %%rax			\n"
	"	mov				8(%%rsi), %%rdx		\n"
	"	lock cmpxchg16b	(%%rsi)				\n"
	"	jnz				cas_loop%=				\n"

	"	mov				 %[old], %%r10			\n"
	"	movq				%%rax, (%%r10)			\n"
	"	movq				%%rdx, 8(%%r10)		\n"
	"	pop				%%rbx						\n"
	: 
	: [excg] "m" (excg_ptr)
	, [old]  "m" (old_ptr)
	, [dest] "m" (dest_ptr)
	: "rsi", "rax", "rcx", "rdx", "r9", "r10"
	);

#else

	Data128        tmp;
	do
	{
		old = *(const Data128 *) (&dest);
		cas (tmp, dest, excg, old);
	}
	while (tmp != old);

#endif
}



void	Interlocked::cas (Data128 &old, volatile Data128 &dest, const Data128 &excg, const Data128 &comp)
{
	assert (is_ptr_aligned_nz (&dest));

#if defined (__APPLE__) || (defined (__CYGWIN__) && conc_WORD_SIZE == 64)

	Data128 *         old_ptr  = &old;
	volatile Data128* dest_ptr = &dest;
	const Data128 *   excg_ptr = &excg;
	const Data128 *   comp_ptr = &comp;

	asm volatile (
	"	mov              %[comp], %%r8   \n"
	"	mov              %[excg], %%r9   \n"
	"	mov              %[old],  %%r10  \n"
	"	mov              %[dest], %%rsi  \n"
	
	"	push             %%rbx           \n"
	"	movq             (%%r8),  %%rax  \n"
	"	movq            8(%%r8),  %%rdx  \n"
	"	movq             (%%r9),  %%rbx  \n"
	"	movq            8(%%r9),  %%rcx  \n"
	"	lock cmpxchg16b (%%rsi)          \n"
	"	movq             %%rax,  (%%r10) \n"
	"	movq             %%rdx, 8(%%r10) \n"
	"	pop              %%rbx           \n"
	:
	: [old]  "m" (old_ptr)
	, [dest] "m" (dest_ptr)
	, [excg] "m" (excg_ptr)
	, [comp] "m" (comp_ptr)
	: "rsi", "rax", "rcx", "rdx", "r8", "r9", "r10"
	);

#elif defined (_WIN32) || defined (WIN32) || defined (__WIN32__) || defined (__CYGWIN__) || defined (__CYGWIN32__)

	#if (defined (_MSC_VER) && ((_MSC_VER / 100) < 15))	// Before VC2008

		/*** To do ***/
		#error Requires assembly code

	#elif defined (_MSC_VER)

		const int64_t  excg_lo = ((const int64_t *) &excg) [0];
		const int64_t  excg_hi = ((const int64_t *) &excg) [1];

		old = comp;

		_InterlockedCompareExchange128 (
			reinterpret_cast <volatile int64_t *> (&dest),
			excg_hi,
			excg_lo,
			reinterpret_cast <int64_t *> (&old)
		);

	#else

	::InterlockedCompareExchange128 (
			reinterpret_cast <volatile int64_t *> (&dest),
			excg_hi,
			excg_lo,
			reinterpret_cast <int64_t *> (&old)
		);

	#endif

#elif defined (__linux__)

	old = __sync_val_compare_and_swap (&dest, comp, excg);

#endif
}



#if defined (_MSC_VER)

bool	Interlocked::Data128::operator == (const Data128 & other) const
{
	return (   _data [0] == other._data [0]
	        && _data [1] == other._data [1]);
}

bool	Interlocked::Data128::operator != (const Data128 & other) const
{
	return (   _data [0] != other._data [0]
	        || _data [1] != other._data [1]);
}

#endif	// _MSC_VER

#endif	// conc_HAS_CAS_128



#if defined (_MSC_VER)
	#pragma warning (push)
	#pragma warning (4 : 4311 4312)
#endif

void *	Interlocked::swap (void * volatile &dest_ptr, void *excg_ptr)
{
	return (reinterpret_cast <void *> (
		swap (
			*reinterpret_cast <IntPtr volatile *> (&dest_ptr),
			reinterpret_cast <IntPtr> (excg_ptr)
		)
	));
}



void *	Interlocked::cas (void * volatile &dest_ptr, void *excg_ptr, void *comp_ptr)
{
	return (reinterpret_cast <void *> (
		cas (
			*reinterpret_cast <IntPtr volatile *> (&dest_ptr),
			reinterpret_cast <IntPtr> (excg_ptr),
			reinterpret_cast <IntPtr> (comp_ptr)
		)
	));
}

#if defined (_MSC_VER)
	#pragma warning (pop)
#endif



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace conc



#endif	// conc_Interlocked_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
