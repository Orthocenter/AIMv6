#include <config.h>

u32 load_u32(u8 *ptr) {
	return (((u32)ptr[3] << 8 | ptr[2]) << 8 | ptr[1]) << 8 | ptr[0];
}
