#include "scu.h"

void enable_SCU() {
	u32 *mpcore = MPCORE_PHYSBASE;
	*mpcore |= 1;
}