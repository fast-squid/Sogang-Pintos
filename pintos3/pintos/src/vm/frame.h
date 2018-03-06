#define FPAGES 400
#include "lib/kernel/list.h"
#include "threads/synch.h"
struct lock frame_lock;
struct FRAME_STACK{
	uint32_t* pd;
	void* upage;
	void* kpage;
	bool writable;
	struct list_elem frame_elem;
};
struct list frame_list;

void init_frame_tb(void);
void push_frame_stack(uint32_t *pd, void* upage, void* kpage,bool writable);
struct FRAME_STACK* pop_frame_stack(void);

