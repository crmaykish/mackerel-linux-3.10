#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/traps.h>
#include <asm/io.h>
#include <asm/machdep.h>
#include <asm/siginfo.h>
#include <linux/kallsyms.h>
#include <linux/signal.h>
#include <linux/ptrace.h>
#include <asm/mackerel.h>

static irq_handler_t timer_interrupt;

static irqreturn_t mfp_timer_tick(int irq, void *dummy)
{
    MEM(MFP_GPDR) += 1;

	return timer_interrupt(irq, dummy);
}

static struct irqaction mackerel_timer_irq = {
	.name	 = "timer",
	.flags	 = IRQF_DISABLED | IRQF_TIMER,
	.handler = mfp_timer_tick,
};

void mackerel_timer_init(irq_handler_t handler)
{
    printk("hw_timer_init()\r\n");

    // Set MFP Timer B to run at 36 Hz and trigger an interrupt on every tick
    MEM(MFP_TBDR) = 0;         // Timer B counter max (i.e 255);
    MEM(MFP_TBCR) = 0b0010111; // Timer B enabled, delay mode, /200 prescalar
    MEM(MFP_VR) = 0x40;        // Set base vector for MFP interrupt handlers
    MEM(MFP_IERA) = 0x01;      // Enable interrupts for Timer B
    MEM(MFP_IMRA) = 0x01;      // Unmask interrupt for Timer B
    
    timer_interrupt = handler;

    setup_irq(1, &mackerel_timer_irq);
}


void __init config_BSP(char *command, int len)
{
    printk(KERN_INFO "\nMackerel 68k support by Colin Maykish <crmaykish@gmail.com>\n");

    mach_sched_init = mackerel_timer_init;
}
