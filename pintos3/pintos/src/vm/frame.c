#include "frame.h"
#include "lib/kernel/list.h"
#include "userprog/pagedir.h"
#include "threads/malloc.h"
#include "threads/pte.h"


void init_frame_tb(void){
	list_init(&frame_list);
	lock_init(&frame_lock);
}

void push_frame_stack(uint32_t* pd, void* upage,void* kpage, bool writable){
	/* TO DO...
	push frame to frame list */
	struct FRAME_STACK* frame_block = malloc(sizeof(struct FRAME_STACK));
	frame_block->pd = pd;
	frame_block->upage = upage;
	frame_block->kpage = kpage;
	frame_block->writable = writable;
	list_push_back(&frame_list,&frame_block->frame_elem);
}

struct FRAME_STACK* pop_frame_stack(void){
	return list_entry(list_pop_front(&frame_list), 
			struct FRAME_STACK,frame_elem);
}

