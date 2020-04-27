/* Memory allocation routines.

Copyright 1991, 1993, 1994, 2000-2002, 2012 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of either:

  * the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.

or

  * the GNU General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any
    later version.

or both in parallel, as here.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received copies of the GNU General Public License and the
GNU Lesser General Public License along with the GNU MP Library.  If not,
see https://www.gnu.org/licenses/.  */

#if defined(WARM_RUNTIME_SET_ALLOCATOR) || (defined(WARM_COMPILE_TIME_SET_ALLOCATOR) && defined(WARM_USE_DEFAULT_ALLOCATOR))
#include "gmp.h"

#include <stdio.h>
#include <stdlib.h>

#include "gmp-impl.h"

#if defined(WARM_RUNTIME_SET_ALLOCATOR)
  void * (*__gmp_allocate_func) (size_t) = __gmp_default_allocate;
  void * (*__gmp_reallocate_func) (void *, size_t, size_t) = __gmp_default_reallocate;
  void   (*__gmp_free_func) (void *, size_t) = __gmp_default_free;
#elif defined(WARM_COMPILE_TIME_SET_ALLOCATOR)
# define __gmp_default_allocate __gmp_allocate_func
# define __gmp_default_reallocate __gmp_reallocate_func
# define __gmp_default_free __gmp_free_func
#endif

#if !defined(DEBUG)
void *__gmp_default_allocate(size_t size)
{
  void *ret = malloc(size);

  if (ret == NULL)
  {
    fprintf (stderr, "GNU MP: Cannot allocate memory (size=%lu)\n", (long) size);
    abort ();
  }

  return ret;
}

void *__gmp_default_reallocate(void *oldptr, size_t old_size, size_t new_size)
{
  void *ret = realloc(oldptr, new_size);

  if (ret == NULL)
  {
    fprintf (stderr, "GNU MP: Cannot reallocate memory (old_size=%lu new_size=%lu)\n", (long) old_size, (long) new_size);
    abort ();
  }

  return ret;
}

void __gmp_default_free(void *blk_ptr, size_t blk_size)
{
  free (blk_ptr);
}

#else

void *__gmp_default_allocate(size_t size)
{
  void *ret;

  size_t req_size = size;
  size += 2 * GMP_LIMB_BYTES;

  ret = malloc(size);
  if (ret == NULL)
  {
    fprintf (stderr, "GNU MP: Cannot allocate memory (size=%lu)\n", (long) size);
    abort ();
  }

  mp_ptr p = ret;
  p++;
  p[-1] = (0xdeadbeef << 31) + 0xdeafdeed;
  if (req_size % GMP_LIMB_BYTES == 0)
  {
    p[req_size / GMP_LIMB_BYTES] = ~((0xdeadbeef << 31) + 0xdeafdeed);
  }
  ret = p;

  return ret;
}

void *__gmp_default_reallocate(void *oldptr, size_t old_size, size_t new_size)
{
  void *ret;

  size_t req_size = new_size;

  if (old_size != 0)
  {
    mp_ptr p = oldptr;
    if (p[-1] != (0xdeadbeef << 31) + 0xdeafdeed)
	  {
      fprintf (stderr, "gmp: (realloc) data clobbered before allocation block\n");
      abort ();
	  }
    
    if (old_size % GMP_LIMB_BYTES == 0) 
    {
      if (p[old_size / GMP_LIMB_BYTES] != ~((0xdeadbeef << 31) + 0xdeafdeed))
      {
        fprintf (stderr, "gmp: (realloc) data clobbered after allocation block\n");
        abort ();
      }
      oldptr = p - 1;
    }
  }

  new_size += 2 * GMP_LIMB_BYTES;

  ret = realloc(oldptr, new_size);
  if (ret == NULL)
  {
    fprintf (stderr, "GNU MP: Cannot reallocate memory (old_size=%lu new_size=%lu)\n", (long) old_size, (long) new_size);
    abort ();
  }

  mp_ptr p = ret;
  p++;
  p[-1] = (0xdeadbeef << 31) + 0xdeafdeed;
  if (req_size % GMP_LIMB_BYTES == 0)
  {
    p[req_size / GMP_LIMB_BYTES] = ~((0xdeadbeef << 31) + 0xdeafdeed);
  }
  ret = p;
  
  return ret;
}

void __gmp_default_free(void *blk_ptr, size_t blk_size)
{
  mp_ptr p = blk_ptr;
  if (blk_size != 0)
  {
    if (p[-1] != (0xdeadbeef << 31) + 0xdeafdeed)
    {
      fprintf (stderr, "gmp: (free) data clobbered before allocation block\n");
      abort ();
    }
    if (blk_size % GMP_LIMB_BYTES == 0)
    {
      if (p[blk_size / GMP_LIMB_BYTES] != ~((0xdeadbeef << 31) + 0xdeafdeed))
      {
        fprintf (stderr, "gmp: (free) data clobbered after allocation block\n");
        abort ();
      }
    }
  }
  blk_ptr = p - 1;

  free (blk_ptr);
}
#endif

#if defined(WARM_COMPILE_TIME_SET_ALLOCATOR)
# undef __gmp_default_allocate __gmp_allocate_func
# undef __gmp_default_reallocate __gmp_reallocate_func
# undef __gmp_default_free __gmp_free_func
#endif

#endif
