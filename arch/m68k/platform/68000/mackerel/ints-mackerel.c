#include <linux/kernel.h>
#include <linux/init.h>

void __init trap_init(void)
{
    printk("trap_init()\r\n");
}

void __init init_IRQ(void)
{
    printk("init_IRQ()\r\n");
}

void resume(void)
{
    printk("resume()\r\n");
}

void ret_from_exception(void)
{
    printk("ret_from_exception()\r\n");
}
