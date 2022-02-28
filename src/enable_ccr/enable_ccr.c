#include <linux/module.h>
#include <linux/kernel.h>

void enable_ccr (void *info)
{
#if defined (__aarch64__)
	uint32_t cr = 0;
	// Set the User Enable register, bit 0
	asm volatile ("msr pmuserenr_el0, %0\t\n" :: "r" (1));
	// 0: Enable all counters in the PMNC control-register
	// 2: Reset CCNT to zero
	// 3: if 1, divide by 64
	asm volatile ("mrs %0, pmcr_el0\t\n" : "=r" (cr));
	cr |= (1U<<0);  // set bit 0 - enable ccnt
	cr |= (1U<<2);  // set bit 2 - clear ccnt
	cr &= ~(1U<<3); // clear bit 3
	asm volatile ("msr pmcr_el0, %0\t\n" :: "r" (cr));
	// Enable cycle counter specifically
	// bit 31: enable cycle counter
	// bits 0-3: enable performance counters 0-3
	asm volatile ("mrs %0, pmcntenset_el0\t\n" : "=r" (cr));
	cr |= 0x80000000;
	asm volatile ("msr pmcntenset_el0, %0\t\n" :: "r" (cr));
#else
	// Set the User Enable register, bit 0
	asm volatile ("mcr p15, 0, %0, c9, c14, 0" :: "r" (1));
	// Enable all counters in the PNMC control-register
	asm volatile ("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r" (1));
	// Enable cycle counter specifically
	// bit 31: enable cycle counter
	// bits 0-3: enable performance counters 0-3
	asm volatile ("MCR p15, 0, %0, c9, c12, 1\t\n" :: "r" (0x80000000));
#endif
}

int init_module (void)
{
	// Each cpu has its own set of registers
	on_each_cpu (enable_ccr, NULL, 0);
	printk (KERN_INFO "Userspace access to CCR enabled\n");
	return 0;
}

void cleanup_module (void)
{
	// Nothing
}

