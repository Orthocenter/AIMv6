#include "slab_test.h"
#include "slab.h"

u32 get_cache_num(cache_t *cache) {
	u32 num = 0;
	while(cache) {
		num++;
		cache = cache->next;
	}
	return num;
}

void slab_test() {
	u32* cache_shift = get_cache_shift();
	u32* cache_size = get_cache_size();
	cache_t** pools = get_pools();

	uart_spin_puts("MM/slab: slab test begin\r\n");
	
	u32 cache_num[POOL_NUM];
	uart_spin_puts("MM/slab: test initial amount of each pool: ");
	for(u32 i = 0; i < POOL_NUM; i++) {
		u32 num = get_cache_num(pools[i]);
		cache_num[i] = num;
		expect_no_newline(num, SLAB_SIZE << PAGE_SHIFT >> cache_shift[i]);
		uart_spin_puts(" ");
	}
	uart_spin_puts("\r\n");

	void* addr[POOL_NUM];
	uart_spin_puts("MM/slab: test allocate objects of each cache size: ");
	for(u32 i = 0; i < POOL_NUM; i++) {
		void *original_addr = pools[i];
		addr[i] = alloc_obj(cache_size[i]);
		expect_no_newline(original_addr == addr[i] && get_cache_num(pools[i]) == cache_num[i] - 1, true);
		uart_spin_puts(" ");
	}
	uart_spin_puts("\r\n");

	void* addr2[POOL_NUM];
	uart_spin_puts("MM/slab: test allocate objects which are smaller than each cache size: ");
	for(u32 i = 0; i < POOL_NUM; i++) {
		void *original_addr = pools[i];
		addr2[i] = alloc_obj(cache_size[i] - 1);
		expect_no_newline(original_addr == addr2[i] && get_cache_num(pools[i]) == cache_num[i] - 2, true);
		uart_spin_puts(" ");
	}
	uart_spin_puts("\r\n");


	uart_spin_puts("MM/slab: test freeing allocated objects (of cache size): ");
	for(u32 i = 0; i < POOL_NUM; i++) {
		free_obj(addr[i], cache_size[i]);
		expect_no_newline(pools[i] == addr[i] && get_cache_num(pools[i]) == cache_num[i] - 1, true);
		uart_spin_puts(" ");
	}
	uart_spin_puts("\r\n");

	uart_spin_puts("MM/slab: test freeing allocated objects (of cache size - 1): ");
	for(u32 i = 0; i < POOL_NUM; i++) {
		free_obj(addr2[i], cache_size[i] - 1);
		expect_no_newline(pools[i] == addr2[i] && get_cache_num(pools[i]) == cache_num[i], true);
		uart_spin_puts(" ");
	}
	uart_spin_puts("\r\n");

	uart_spin_puts("MM/slab: test allocate object whose size is more than 2048 bytes: ");
	void *addr_2049 = alloc_obj(2049);
	expect(addr_2049, NULL);

	uart_spin_puts("MM/slab: test there has no side effect after prior test: ");
	for(u32 i = 0; i < POOL_NUM; i++) {
		expect_no_newline(pools[i] == addr2[i] && get_cache_num(pools[i]) == cache_num[i], true);
		uart_spin_puts(" ");
	}
	uart_spin_puts("\r\n");

	uart_spin_puts("MM/slab: all test passed\r\n");
}

