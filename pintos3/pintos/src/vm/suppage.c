#include "suppage.h"
#include "frame.h"
#include "threads/thread.h"
#include "threads/pte.h"
#include "userprog/pagedir.h"
#include "threads/palloc.h"
#include "lib/kernel/bitmap.h"
#include "lib/kernel/list.h"


void init_invt_tb(void){
	int i;
	for(i=0;i<MAX_FRAME;i++){
		invt_tb[i].idx = -1;
		invt_tb[i].pd = NULL;
		invt_tb[i].upage = NULL;
		invt_tb[i].kpage = NULL;
		invt_tb[i].writable = true;

		swap_tb[i].idx = -1;
		swap_tb[i].pd = NULL;
		swap_tb[i].upage = NULL;
		swap_tb[i].kpage = NULL;
		swap_tb[i].writable = true;
		
	}
	invt_map = bitmap_create(1000);
	bitmap_set_all(invt_map,false);
	swap_map = bitmap_create(1000);
	bitmap_set_all(swap_map,false);
}

void push_invt_tb(uint32_t *pd, void* upage,void* kpage, bool writable){
	int i;
	for(i=0;i<MAX_FRAME;i++){
		if(swap_tb[i].upage == upage){
			bitmap_set(swap_map, i, false);
			break;
		}
		if(invt_tb[i].kpage==kpage && invt_tb[i].idx==-1){ 
			return;
		}
	}
	int idx=bitmap_scan(invt_map, 0, 1,false);
	bitmap_set(invt_map, idx,true);
	
	invt_tb[idx].idx = -1;
	invt_tb[idx].pd = pd;
	invt_tb[idx].upage = upage;
	invt_tb[idx].kpage = kpage;
	invt_tb[idx].writable = writable;
}

int push_swap_tb(uint32_t *pd,void* upage, void* kpage,bool writable){
	int i;
	for(i=0;i<MAX_FRAME;i++){
		if(invt_tb[i].upage == upage){
			bitmap_set(invt_map, i, false);
			break;
		}
	}
	int idx=bitmap_scan(swap_map, 0, 1,false);
	bitmap_set(swap_map, idx,true);
	
	swap_tb[idx].idx = idx;
	swap_tb[idx].pd = pd;
	swap_tb[idx].upage = upage;
	swap_tb[idx].kpage = kpage;
	swap_tb[idx].writable = writable;

	pagedir_clear_page(swap_tb[idx].pd, swap_tb[idx].upage);
	return idx;
}

struct SUPPAGE* find_entry_by_upage(uint32_t* pd,void* upage){
	struct list* list;
	int i;
	for(i=0;i<MAX_FRAME;i++){
		if( swap_tb[i].upage == upage){
			return &swap_tb[i];
		}
	}
	return NULL;
}

