#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/traps.h>
#include <asm/machdep.h>
#include <asm/mackerel.h>

asmlinkage void system_call(void);
asmlinkage irqreturn_t inthandler1(void);

void process_int(int vec, struct pt_regs *fp)
{
    MEM(MFP_GPDR) += 1;
    do_IRQ(1, fp);
}

static void intc_irq_unmask(struct irq_data *d)
{
    // TODO
}

static void intc_irq_mask(struct irq_data *d)
{
    // TODO
}

static struct irq_chip intc_irq_chip = {
    .name = "MACKEREL-INTC",
    .irq_mask = intc_irq_mask,
    .irq_unmask = intc_irq_unmask,
};

void __init trap_init(void)
{
    printk("trap_init()\r\n");

    _ramvec[32] = system_call;
    _ramvec[0x48] = inthandler1;
}

void __init init_IRQ(void)
{
    int i;

    printk("init_IRQ()\r\n");

    for (i = 0; (i < NR_IRQS); i++)
    {
        irq_set_chip(i, &intc_irq_chip);
        irq_set_handler(i, handle_level_irq);
    }
}
