#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/traps.h>

extern e_vector *_ramvec;

static void intc_irq_enable(struct irq_data *d)
{
}

static void intc_irq_disable(struct irq_data *d)
{
}

static struct irq_chip intc_irq_chip = {
	.name		= "MACKEREL-INTC",
	.irq_enable	= intc_irq_enable,
	.irq_disable	= intc_irq_disable,
};

asmlinkage irqreturn_t inthandler_vec1(void);

void __init trap_init(void)
{
    printk("trap_init()\r\n");

    _ramvec[0x48] = (e_vector)inthandler_vec1;
}

void __init init_IRQ(void)
{
    int irq;

    printk("init_IRQ()\r\n");

    for (irq = 0; (irq < NR_IRQS); irq++)
    {
        irq_set_chip_and_handler(irq, &intc_irq_chip, handle_simple_irq);
    }
}
