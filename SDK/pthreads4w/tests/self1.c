/*
 * self1.c
 *
 *
 * --------------------------------------------------------------------------
 *
 *      Pthreads4w - POSIX Threads Library for Win32
 *      Copyright(C) 1998 John E. Bossom
 *      Copyright(C) 1999-2018, Pthreads4w contributors
 *
 *      Homepage: https://sourceforge.net/projects/pthreads4w/
 *
 *      The current list of contributors is contained
 *      in the file CONTRIBUTORS included with the source
 *      code distribution. The list can also be seen at the
 *      following World Wide Web location:
 *      https://sourceforge.net/p/pthreads4w/wiki/Contributors/
 *
 * This file is part of Pthreads4w.
 *
 *    Pthreads4w is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    Pthreads4w is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Pthreads4w.  If not, see <http://www.gnu.org/licenses/>. *
 *
 * --------------------------------------------------------------------------
 *
 * Test for pthread_self().
 *
 * Depends on API functions:
 *	pthread_self()
 *
 * Implicitly depends on:
 *	pthread_getspecific()
 *	pthread_setspecific()
 */

#include "test.h"

int
main(int argc, char * argv[])
{
	/*
	 * This should always succeed unless the system has no
	 * resources (memory) left.
	 */
	pthread_t self;

#if defined(PTW32_STATIC_LIB) && !(defined(_MSC_VER) || defined(__MINGW32__))
	pthread_win32_process_attach_np();
#endif

	self = pthread_self();

	assert(self.p != NULL);

#if defined(PTW32_STATIC_LIB) && !(defined(_MSC_VER) || defined(__MINGW32__))
	pthread_win32_process_detach_np();
#endif
	return 0;
}
