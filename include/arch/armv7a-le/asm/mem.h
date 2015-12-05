#ifndef _ASM_MEM
#define _ASM_MEM

/* Physical memory space layout:
 * 0		..  1M			reserved
 * 1M		..	2M			MBR
 * 2M		..  2M+16M		kernel
 * 18M-18K	..  18M			l1 page table of kernel
 *
 * 18M		..	512M-16M	free space
 *
 * 512M-16M	..	512M		kernel stack
 */

/* Virtual memeory space layout
 * 0		..	2G			free space
 * 2G		..	2G+16M		kernel
 * 2G+16M-16K.	2G+16M		l1 page table of kernel
 * 
 * 4G-528M	..	4G-512M		kernel stack
 * 4G-512M	..	4G			Device
 */


#define KERN_BASE 0x80000000 // kernel: 2G -> 2G+16M
#define KERN_BASE_PHY 0x00200000 // kernel physical address: 2M
#define KERN_SIZE 16 // 16M

#define KERN_TTB_BASE 0x80FFC000 // kernel tranlation table base: 2G+16M-16K
#define KERN_TTB_BASE_PHY 0x011FC000 // kernel ttb physical address: 18M-16K

#define KERN_STACK 0xDFFFFFFF // kernel stack: 4G-512M -> 4G-512M-16M
#define KERN_STACK_PHY 0x1FEFFFFF // kernel stack physical address: 511M
#define KERN_STACK_SIZE 16 // 16M

#define FREE_MEM_PHY_ST 0x01200000 // free physical memory start address
#define FREE_MEM_PHY_ED 0x1EEFFFFF // free physical memory end address

#define DEVICE_MEM_PHY_ST 0xE0000000
#define DEVICE_MEM_PHY_ED 0xFFFFFFFF

#endif
