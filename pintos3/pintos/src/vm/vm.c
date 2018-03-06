#include "vm.h"
#include "frame.h"
#include "suppage.h"
#include "userprog/pagedir.h"
#include "devices/block.h"
#include "threads/palloc.h"
#include "threads/thread.h"
#include "lib/kernel/bitmap.h"
#include "threads/interrupt.h"
#include <stdio.h>

#define PGSIZE 4096
#define SECTSIZE 512
#define SECTPERPG (PGSIZE/SECTSIZE)

void swap_out(){	
	/* TO DO...
	get Least Recently Used page from stack to evict */
	struct FRAME_STACK* stack_block = pop_frame_stack();
	uint32_t* pd = stack_block->pd;
	void *upage = stack_block->upage;
	void *kpage = stack_block->kpage;
	bool writable = stack_block->writable;
	int idx =	push_swap_tb(pd, upage, kpage, writable);
	if(idx==-1) thread_exit();
	/*write LRU page to block */
	struct block* swap_block = block_get_role(BLOCK_SWAP);

	int i;
	for(i=0;i<SECTPERPG;i++){
		block_write(swap_block,	i+SECTPERPG*idx, kpage+(SECTSIZE*i));
	}
	
	/* push page in swap table */
	palloc_free_page(kpage);
	free(stack_block);
}

bool swap_in(struct SUPPAGE* ste){
	int idx = ste->idx;
	if(idx==-1) thread_exit();
	
	void* knpage = palloc_get_page(PAL_USER|PAL_ZERO);
	ste->kpage = knpage;
	struct block* swap_block = block_get_role(BLOCK_SWAP);
	int i;
	for(i=0;i<SECTPERPG;i++){
		block_read(swap_block, i+SECTPERPG*idx, knpage+(SECTSIZE*i));
	}
	/*update pagetable entries, frame stack, bitmaps */
	pagedir_set_page(ste->pd, ste->upage, ste->kpage, ste->writable);
	push_frame_stack(ste->pd, ste->upage, ste->kpage, ste->writable);
	push_invt_tb(ste->pd, ste->upage, ste->kpage, ste->writable);
	bitmap_set(swap_map, ste->idx, false);
	return true;
}
