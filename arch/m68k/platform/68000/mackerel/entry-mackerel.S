#include <linux/linkage.h>
#include <asm/unistd.h>
#include <asm/thread_info.h>
#include <asm/errno.h>
#include <asm/setup.h>
#include <asm/segment.h>
#include <asm/traps.h>
#include <asm/asm-offsets.h>
#include <asm/entry.h>

.text

.globl system_call
.globl resume
.globl ret_from_exception
.globl ret_from_signal
.globl sys_call_table
.globl bad_interrupt

.globl inthandler_vec1

inthandler_vec1:
	SAVE_ALL_INT
	movel	%sp,%sp@-
	movel	#1,%sp@- 		/*  put vector # on stack */
	jsr	do_IRQ			/*  process the IRQ */
	addql	#8,%sp			/*  pop parameters off stack */
	bra	ret_from_exception


ret_from_exception:
	btst	#5,%sp@(PT_OFF_SR)	/* check if returning to kernel*/
	jeq	Luser_return		/* if so, skip resched, signals*/

Lkernel_return:
	RESTORE_ALL

Luser_return:
	/* only allow interrupts when we are really the last one on the*/
	/* kernel stack, otherwise stack overflow can occur during*/
	/* heavy interrupt load*/
	andw	#ALLOWINT,%sr

	movel	%sp,%d1			/* get thread_info pointer */
	andl	#-THREAD_SIZE,%d1
	movel	%d1,%a2
1:
	move	%a2@(TINFO_FLAGS),%d1	/* thread_info->flags */
	jne	Lwork_to_do
	RESTORE_ALL

Lwork_to_do:
	movel	%a2@(TINFO_FLAGS),%d1	/* thread_info->flags */
	btst	#TIF_NEED_RESCHED,%d1
	jne	reschedule

Lsignal_return:
	subql	#4,%sp			/* dummy return address*/
	SAVE_SWITCH_STACK
	pea	%sp@(SWITCH_STACK_SIZE)
	bsrw	do_notify_resume
	addql	#4,%sp
	RESTORE_SWITCH_STACK
	addql	#4,%sp
	jra	1b


/*
 * Beware - when entering resume, prev (the current task) is
 * in a0, next (the new task) is in a1, so don't change these
 * registers until their contents are no longer needed.
 */
ENTRY(resume)
	movel	%a0,%d1				/* save prev thread in d1 */
	movew	%sr,%a0@(TASK_THREAD+THREAD_SR)	/* save sr */
	SAVE_SWITCH_STACK
	movel	%sp,%a0@(TASK_THREAD+THREAD_KSP) /* save kernel stack */
	movel	%usp,%a3			/* save usp */
	movel	%a3,%a0@(TASK_THREAD+THREAD_USP)

	movel	%a1@(TASK_THREAD+THREAD_USP),%a3 /* restore user stack */
	movel	%a3,%usp
	movel	%a1@(TASK_THREAD+THREAD_KSP),%sp /* restore new thread stack */
	movew	%a1@(TASK_THREAD+THREAD_SR),%sr	/* restore thread status reg */
	RESTORE_SWITCH_STACK
	rts