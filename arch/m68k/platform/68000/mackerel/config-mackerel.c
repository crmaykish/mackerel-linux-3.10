#include <linux/kernel.h>
#include <linux/init.h>

void __init config_BSP(char *command, int len)
{
    printk(KERN_INFO "\nMackerel 68k support by Colin Maykish <crmaykish@gmail.com>\n");
}
