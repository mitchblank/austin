// This file is part of "austin" which is released under GPL.
//
// See file LICENCE or go to http://www.gnu.org/licenses/ for full license
// details.
//
// Austin is a Python frame stack sampler for CPython.
//
// Copyright (c) 2018 Gabriele N. Tornetta <phoenix1987@gmail.com>.
// All rights reserved.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef MEM_H
#define MEM_H


#include <sys/types.h>

#include "platform.h"

#if defined PL_LINUX
  #include <sys/uio.h>
  ssize_t process_vm_readv(
    pid_t, const struct iovec *, unsigned long liovcnt,
    const struct iovec *remote_iov, unsigned long riovcnt, unsigned long flags
  );

#elif defined(PL_WIN)
  #include <windows.h>

#elif defined(PL_MACOS)
  #include <mach/mach.h>
  #include <mach/mach_vm.h>
  #include <mach/machine/kern_return.h>

#endif

#include "error.h"
#include "logging.h"

#define OUT_OF_BOUND                  -1


/**
 * Copy a data structure from the given remote address structure.
 * @param  raddr the remote address
 * @param  dt    the data structure as a local variable
 * @return       zero on success, otherwise non-zero.
 */
#define copy_from_raddr(raddr, dt) copy_memory(raddr->pid, raddr->addr, sizeof(dt), &dt)


/**
 * Copy a data structure from the given remote address structure.
 * @param  raddr the remote address
 * @param  dt    the data structure as a local variable
 * @return       zero on success, otherwise non-zero.
 */
#define copy_from_raddr_v(raddr, dt, n) copy_memory(raddr->pid, raddr->addr, n, &dt)


/**
 * Same as copy_from_raddr, but with explicit arguments instead of a pointer to
 * a remote address structure
 * @param  pid  the process ID
 * @param  addr the remote address
 * @param  dt   the data structure as a local variable.
 * @return      zero on success, otherwise non-zero.
 */
#define copy_datatype(pid, addr, dt) copy_memory(pid, addr, sizeof(dt), &dt)


typedef struct {
  pid_t  pid;
  void * addr;
} raddr_t;


/**
 * Copy a chunk of memory from a portion of the virtual memory of another
 * process.
 * @param pid_t   the process reference (platform-dependent)
 * @param void *  the remote address
 * @param ssize_t the number of bytes to read
 * @param void *  the destination buffer, expected to be at least as large as
 *                the number of bytes to read.
 * @return        zero on success, otherwise non-zero.
 */
static inline int
copy_memory(pid_t pid, void * addr, ssize_t len, void * buf) {
  ssize_t result;

  #if defined(PL_LINUX)                                              /* LINUX */
  struct iovec local[1];
  struct iovec remote[1];

  local[0].iov_base = buf;
  local[0].iov_len = len;
  remote[0].iov_base = addr;
  remote[0].iov_len = len;

  result = process_vm_readv(pid, local, 1, remote, 1, 0);
  if (result == -1) {
    switch (errno) {
    case ESRCH:
      set_error(EPROCNPID);
      break;
    case EPERM:
      set_error(EPROCPERM);
      break;
    default:
      set_error(EMEMCOPY);
    }
  }

  #elif defined(PL_WIN)                                                /* WIN */
  size_t n;
  result = ReadProcessMemory((HANDLE) pid, addr, buf, len, &n) ? n : -1;
  if (result == -1) {
    switch(GetLastError()) {
    case ERROR_ACCESS_DENIED:
      set_error(EPROCPERM);
      break;
    case ERROR_INVALID_HANDLE:
      set_error(EPROCNPID);
      break;
    default:
      set_error(EMEMCOPY);
    }
  }

  #elif defined(PL_MACOS)                                              /* MAC */
  kern_return_t kr = mach_vm_read_overwrite(
    (mach_port_t) pid,
    (mach_vm_address_t) addr,
    len,
    (mach_vm_address_t) buf,
    (mach_vm_size_t *) &result
  );
  if (kr != KERN_SUCCESS) {
    // If we got to the point of calling this function on macOS then we must
    // have permissions to call task_for_pid successfully. This also mean that
    // the PID that was used must have been valid. Therefore this call can only
    // fail if the process no longer exists.
    set_error(EPROCNPID);
    FAIL;
  }

  #endif

  return result != len;
}

#endif // MEM_H
