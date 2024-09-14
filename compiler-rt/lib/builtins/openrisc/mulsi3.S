//===--- mulsi3.S - Integer multiplication routines -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

	.balign 4
	.globl	__mulsi3
	.type	__mulsi3, @function
__mulsi3:
	l.movhi		r11, 0			/* initial r */

	/* Given R = X * Y ... */
1:	l.sfeq		r4, r0			/* while (y != 0) */
	l.bf		2f
	 l.andi		r5, r4, 1		/* if (y & 1) ... */
	l.add		r12, r11, r3
	l.sfne		r5, r0
#if defined(__or1k_cmov__)
	l.cmov		r11, r12, r11		/* ... r += x. */
	l.srli		r4, r4, 1		/* y >>= 1 */
#else
	l.bnf		3f
	 l.srli		r4, r4, 1		/* y >>= 1 */
	l.ori		r11, r12, 0
3:
#endif
	l.j		1b
	 l.add		r3, r3, r3		/* x <<= 1 */

2:	l.jr		r9
	 l.nop

	.size	__mulsi3, . - __mulsi3