/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/termios.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

/**
 * Opens new pseudo teletypewriter.
 *
 * @return fd of master pty, or -1 w/ errno
 * @params flags is usually O_RDWR|O_NOCTTY
 * @return file descriptor, or -1 w/ errno
 */
int posix_openpt(int flags) {
  int rc;
  if ((flags & O_ACCMODE) != O_RDWR) {
    rc = einval();
  } else if (IsLinux() || IsXnu()) {
    rc = sys_open("/dev/ptmx", flags, 0);
  } else if (IsOpenbsd()) {
    rc = sys_open("/dev/ptm", flags, 0);
  } else if (IsFreebsd()) {
    rc = sys_posix_openpt(flags);
    if (rc == -1 && errno == ENOSPC) errno = EAGAIN;
  } else {
    rc = enosys();
  }
  STRACE("posix_openpt(%#o) → %d% m", flags, rc);
  return rc;
}
