/*
    FreeRTOS V8.2.3 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution and was contributed
    to the project by Technolution B.V. (www.technolution.nl,
    freertos-riscv@technolution.eu) under the terms of the FreeRTOS
    contributors license.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry''s de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/


#if __riscv_xlen == 64
# define STORE    sd
# define LOAD     ld
# define REGBYTES 8
#else
# define STORE    sw
# define LOAD     lw
# define REGBYTES 4
#endif

/* Enable interrupts when returning from the handler */
#define MSTATUS_PRV1 0x1880

.global portSAVE_CONTEXT
.global portRESTORE_CONTEXT
.global TIMER_CMP_INT
.global xPortStartScheduler
.global vPortYield
.global vTaskIncrementTick
.global vPortEndScheduler
.global xExitStack

/*
.pushsection .counters
counter_portSAVE_CONTEXT:
	.word 0xdeadbee0
total_portSAVE_CONTEXT:
	.word 0xdeadbee1
counter_portRESTORE_CONTEXT:
	.word 0xdeadbee2
total_portRESTORE_CONTEXT:
	.word 0xdeadbee3
counter_portSAVE_EPC:
	.word 0xdeadbee4
total_portSAVE_EPC:
	.word 0xdeadbee5
counter_portSAVE_RA:
	.word 0xdeadbee6
total_portSAVE_RA:
	.word 0xdeadbee7
mini_stack_t6:
	.word 0xdeadbee8
	.word 0xdeadbee9
mini_stack_t5:
	.word 0xdeadbee8
	.word 0xdeadbee9
mini_stack_t4:
	.word 0xdeadbee8
	.word 0xdeadbee9
.popsection
*/

.macro add_bp_lable name
bp_lable_\name:
	.endm

.macro reset_mcycle
	csrw mcycle, x0
	.endm

/*
.macro save_cycles name
	csrr t6, mcycle
	la t5, counter_\name
	LOAD t4, 0(t5)
	addi t4, t4, 1
	STORE t4, 0(t5)
	LOAD t4, 4(t5)
	add t4, t4, t6
	STORE t4, 4(t5)
.endm
*/

/* Macro for saving task context */
.macro portSAVE_CONTEXT
	reset_mcycle
	.global	pxCurrentTCB
	/* make room in stack */
	addi	sp, sp, -REGBYTES * 32

	/* Save Context */
	STORE	x1, 0x0(sp)
	STORE	x2, 1 * REGBYTES(sp)
	STORE	x3, 2 * REGBYTES(sp)
	STORE	x4, 3 * REGBYTES(sp)
	STORE	x5, 4 * REGBYTES(sp)
	STORE	x6, 5 * REGBYTES(sp)
	STORE	x7, 6 * REGBYTES(sp)
	STORE	x8, 7 * REGBYTES(sp)
	STORE	x9, 8 * REGBYTES(sp)
	STORE	x10, 9 * REGBYTES(sp)
	STORE	x11, 10 * REGBYTES(sp)
	STORE	x12, 11 * REGBYTES(sp)
	STORE	x13, 12 * REGBYTES(sp)
	STORE	x14, 13 * REGBYTES(sp)
	STORE	x15, 14 * REGBYTES(sp)
	STORE	x16, 15 * REGBYTES(sp)
	STORE	x17, 16 * REGBYTES(sp)
	STORE	x18, 17 * REGBYTES(sp)
	STORE	x19, 18 * REGBYTES(sp)
	STORE	x20, 19 * REGBYTES(sp)
	STORE	x21, 20 * REGBYTES(sp)
	STORE	x22, 21 * REGBYTES(sp)
	STORE	x23, 22 * REGBYTES(sp)
	STORE	x24, 23 * REGBYTES(sp)
	STORE	x25, 24 * REGBYTES(sp)
	STORE	x26, 25 * REGBYTES(sp)
	STORE	x27, 26 * REGBYTES(sp)
	STORE	x28, 27 * REGBYTES(sp)
	STORE	x29, 28 * REGBYTES(sp)
	STORE	x30, 29 * REGBYTES(sp)
	STORE	x31, 30 * REGBYTES(sp)
	
	/* Store current stackpointer in task control block (TCB) */
	LOAD	t0, pxCurrentTCB	//pointer
	STORE	sp, 0x0(t0) // save sp to t0
	add_bp_lable portSAVE_CONTEXT\@
	.endm

/* Saves current error program counter (EPC) as task program counter */
.macro portSAVE_EPC
	reset_mcycle
  	csrr	t0, mepc
	STORE	t0, 31 * REGBYTES(sp)
	add_bp_lable portSAVE_EPC\@
	.endm

/* Saves current return adress (RA) as task program counter */
.macro portSAVE_RA
	reset_mcycle
	STORE	ra, 31 * REGBYTES(sp)
	add_bp_lable portSAVE_RA\@
	.endm

/* Macro for restoring task context */
.macro portRESTORE_CONTEXT
	reset_mcycle
	.global	pxCurrentTCB
	/* Load stack pointer from the current TCB */
	LOAD	sp, pxCurrentTCB
	LOAD	sp, 0x0(sp)

	/* Load task program counter */
	LOAD	t0, 31 * REGBYTES(sp)
  	csrw	mepc, t0

	/* Run in machine mode */
  	li 		t0, MSTATUS_PRV1
  	csrs	mstatus, t0

	/* Restore registers,
	   Skip global pointer because that does not change */
	LOAD	x1, 0x0(sp)
	LOAD	x4, 3 * REGBYTES(sp)
	LOAD	x5, 4 * REGBYTES(sp)
	LOAD	x6, 5 * REGBYTES(sp)
	LOAD	x7, 6 * REGBYTES(sp)
	LOAD	x8, 7 * REGBYTES(sp)
	LOAD	x9, 8 * REGBYTES(sp)
	LOAD	x10, 9 * REGBYTES(sp)
	LOAD	x11, 10 * REGBYTES(sp)
	LOAD	x12, 11 * REGBYTES(sp)
	LOAD	x13, 12 * REGBYTES(sp)
	LOAD	x14, 13 * REGBYTES(sp)
	LOAD	x15, 14 * REGBYTES(sp)
	LOAD	x16, 15 * REGBYTES(sp)
	LOAD	x17, 16 * REGBYTES(sp)
	LOAD	x18, 17 * REGBYTES(sp)
	LOAD	x19, 18 * REGBYTES(sp)
	LOAD	x20, 19 * REGBYTES(sp)
	LOAD	x21, 20 * REGBYTES(sp)
	LOAD	x22, 21 * REGBYTES(sp)
	LOAD	x23, 22 * REGBYTES(sp)
	LOAD	x24, 23 * REGBYTES(sp)
	LOAD	x25, 24 * REGBYTES(sp)
	LOAD	x26, 25 * REGBYTES(sp)
	LOAD	x27, 26 * REGBYTES(sp)
	LOAD	x28, 27 * REGBYTES(sp)
	LOAD	x29, 28 * REGBYTES(sp)
	LOAD	x30, 29 * REGBYTES(sp)
	LOAD	x31, 30 * REGBYTES(sp)

	addi	sp, sp, REGBYTES * 32
	add_bp_lable portRESTORE_CONTEXT\@
	mret
	.endm

/* handle timer interrupt */
TIMER_CMP_INT:
	portSAVE_CONTEXT // save task context
	portSAVE_EPC // epc is in t0
	jal	vPortSysTickHandler // handle timer irq, possible context switch? tcp may hold a new value? uses pxCurrentTCB
	portRESTORE_CONTEXT // restore context

xPortStartScheduler:
#ifdef __gracefulExit
	/* Stores context when starting the scheduler in xStartContext.
	   This is used for when you want to gracefully exit the scheduler.
	   For example if you want to test multiple instances after each other in one test suite.
     */
	la		t0, xStartContext
	STORE	x1, 0x0(t0)
	STORE	x2, 1 * REGBYTES(t0)
	STORE	x3, 2 * REGBYTES(t0)
	STORE	x4, 3 * REGBYTES(t0)
	STORE	x5, 4 * REGBYTES(t0)
	STORE	x6, 5 * REGBYTES(t0)
	STORE	x7, 6 * REGBYTES(t0)
	STORE	x8, 7 * REGBYTES(t0)
	STORE	x9, 8 * REGBYTES(t0)
	STORE	x10, 9 * REGBYTES(t0)
	STORE	x11, 10 * REGBYTES(t0)
	STORE	x12, 11 * REGBYTES(t0)
	STORE	x13, 12 * REGBYTES(t0)
	STORE	x14, 13 * REGBYTES(t0)
	STORE	x15, 14 * REGBYTES(t0)
	STORE	x16, 15 * REGBYTES(t0)
	STORE	x17, 16 * REGBYTES(t0)
	STORE	x18, 17 * REGBYTES(t0)
	STORE	x19, 18 * REGBYTES(t0)
	STORE	x20, 19 * REGBYTES(t0)
	STORE	x21, 20 * REGBYTES(t0)
	STORE	x22, 21 * REGBYTES(t0)
	STORE	x23, 22 * REGBYTES(t0)
	STORE	x24, 23 * REGBYTES(t0)
	STORE	x25, 24 * REGBYTES(t0)
	STORE	x26, 25 * REGBYTES(t0)
	STORE	x27, 26 * REGBYTES(t0)
	STORE	x28, 27 * REGBYTES(t0)
	STORE	x29, 28 * REGBYTES(t0)
	STORE	x30, 29 * REGBYTES(t0)
	STORE	x31, 30 * REGBYTES(t0)
#endif
	jal		vPortSetupTimer // enable timer irq
	portRESTORE_CONTEXT

vPortEndScheduler:
#ifdef __gracefulExit
	/* Load current context from xStartContext */
	la		t0, xStartContext
	LOAD	x1, 0x0(t0)
	LOAD	x2, 1 * REGBYTES(t0)
	LOAD	x3, 2 * REGBYTES(t0)
	LOAD	x4, 3 * REGBYTES(t0)
	LOAD	x5, 4 * REGBYTES(t0)
	LOAD	x6, 5 * REGBYTES(t0)
	LOAD	x7, 6 * REGBYTES(t0)
	LOAD	x8, 7 * REGBYTES(t0)
	LOAD	x9, 8 * REGBYTES(t0)
	LOAD	x10, 9 * REGBYTES(t0)
	LOAD	x11, 10 * REGBYTES(t0)
	LOAD	x12, 11 * REGBYTES(t0)
	LOAD	x13, 12 * REGBYTES(t0)
	LOAD	x14, 13 * REGBYTES(t0)
	LOAD	x15, 14 * REGBYTES(t0)
	LOAD	x16, 15 * REGBYTES(t0)
	LOAD	x17, 16 * REGBYTES(t0)
	LOAD	x18, 17 * REGBYTES(t0)
	LOAD	x19, 18 * REGBYTES(t0)
	LOAD	x20, 19 * REGBYTES(t0)
	LOAD	x21, 20 * REGBYTES(t0)
	LOAD	x22, 21 * REGBYTES(t0)
	LOAD	x23, 22 * REGBYTES(t0)
	LOAD	x24, 23 * REGBYTES(t0)
	LOAD	x25, 24 * REGBYTES(t0)
	LOAD	x26, 25 * REGBYTES(t0)
	LOAD	x27, 26 * REGBYTES(t0)
	LOAD	x28, 27 * REGBYTES(t0)
	LOAD	x29, 28 * REGBYTES(t0)
	LOAD	x30, 39 * REGBYTES(t0)
	LOAD	x31, 30 * REGBYTES(t0)
#endif
	ret

vPortYield:
	/*
	*  This routine can be called from outside of interrupt handler. This means
	*  interrupts may be enabled at this point. This is probably okay for registers and
	*  stack. However, "mepc" will be overwritten by the interrupt handler if a timer
	*  interrupt happens during the yield. To avoid this, prevent interrupts before starting.
	*  The write to mstatus in the restore context routine will enable interrupts after the
	*  mret. A more fine-grain lock may be possible.
	*/  
	csrci mstatus, 8

	portSAVE_CONTEXT
	portSAVE_RA
	jal	vTaskSwitchContext
	portRESTORE_CONTEXT
