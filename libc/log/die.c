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
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/lockcmpxchgp.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"

/**
 * Aborts process after printing a backtrace.
 *
 * If a debugger is present then this will trigger a breakpoint.
 */
relegated wontreturn void __die(void) {
  /* asan runtime depends on this function */
  int me, owner;
  static int sync;
  owner = 0;
  me = sys_gettid();
  if (_lockcmpxchgp(&sync, &owner, me)) {
    __restore_tty();
    if (IsDebuggerPresent(false)) {
      DebugBreak();
    }
    ShowBacktrace(2, __builtin_frame_address(0));
    _Exitr(77);
  } else if (owner == me) {
    kprintf("die failed while dying\n");
    _Exitr(78);
  } else {
    _Exit1(79);
  }
}
