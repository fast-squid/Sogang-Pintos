#include <debug.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "lib/kernel/list.h"

#define MAX_FRAME 500
#define NOT_ALLOCATED 2
#define IN_MEM 1
#define IN_DISK 0

struct SUPPAGE{
	int idx;
	uint32_t* pd;
	void* upage;
	void* kpage;
	bool writable;
};
struct SUPPAGE invt_tb[MAX_FRAME];
struct SUPPAGE swap_tb[MAX_FRAME];
struct bitmap *invt_map;
struct bitmap *swap_map;


void init_invt_tb(void);
void push_invt_tb(uint32_t *pd, void* upage,void* kpage, bool writable);
int push_swap_tb(uint32_t *pd, void* upage,void* kpage, bool writable);
struct SUPPAGE* find_entry_by_upage(uint32_t* pd,void* upage);
