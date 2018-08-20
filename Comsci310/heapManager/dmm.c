#include <stdio.h>  // needed for size_t
#include <unistd.h> // needed for sbrk
#include <assert.h> // needed for asserts
#include "dmm.h"

/* You can improve the below metadata structure using the concepts from Bryant
 * and OHallaron book (chapter 9).
 */

typedef struct metadata {
  /* size_t is the return type of the sizeof operator. Since the size of an
   * object depends on the architecture and its implementation, size_t is used
   * to represent the maximum size of any object in the particular
   * implementation. size contains the size of the data object or the number of
   * free bytes
   */
  size_t size;
  struct metadata* next;
  struct metadata* prev; 
} metadata_t;

/* freelist maintains all the blocks which are not in use; freelist is kept
 * sorted to improve coalescing efficiency 
 */

static metadata_t* freelist = NULL;

void* dmalloc(size_t numbytes) {
  /* initialize through sbrk call first time */
  if(freelist == NULL) { 			
    if(!dmalloc_init())
      return NULL;
  }

  assert(numbytes > 0);

  /* your code here */
  // loop freelist to locate the right free block
  metadata_t * temp = freelist;
  while(temp != NULL && numbytes > temp->size) {
    temp = temp->next;
    if(temp == NULL) {
      printf("can't find enough free block");
      return NULL;
    }
  }

  /* split a free block 
  //2 cases: 1)if <METADATA_T_ALIGNED internal fragmentation, then won't split)and malloc */
  //         2)split into 2 blocks; delete the 1st one and add the 2nd one

  //1st case:: don't need to split, just delete
  if( temp->size <= numbytes + METADATA_T_ALIGNED ) {
    // delete it from freelist
    if( temp->prev == NULL) {//head of freelist
      freelist = temp->next;
      if(freelist != NULL) {
	freelist->prev = NULL;
      }
      //printf("freelist will NLL\n");
    } 
    else {//not the head of freelist
      if(temp->next == NULL) {//tail of freelist
	temp->prev->next = NULL;
      }
      else {//in the middle
	temp->prev->next = temp->next;
	temp->next->prev = temp->prev;
      }
    }
    temp->next = NULL;
    temp->prev = NULL;
  }
  
  //2nd case: need to split the block into 2, delete the 1st one and add the 2nd one
  else {
    metadata_t * newtemp =(metadata_t*) ((char*)temp+METADATA_T_ALIGNED+numbytes);
    newtemp->size = temp->size - numbytes - METADATA_T_ALIGNED;
    if(temp->prev == NULL) {//head of freelist
      freelist = newtemp;
    }
    newtemp->next = temp->next;
    newtemp->prev = temp->prev;
    if(temp->prev != NULL) {//update the block before
      temp->prev->next = newtemp;
    }
    if(temp->next != NULL) {//update the block after
      temp->next->prev = newtemp;
    }
    /* update the splitted block */
    temp->size = numbytes;
    temp->next = NULL;
    temp->prev = NULL;
  }
  /* return the malloced space */
  return (void *)((char *)temp + METADATA_T_ALIGNED);
}

void dfree(void* ptr) {
  /* your code here */
  if(ptr == NULL) {
	return;
  }
  char * p = (char *) ptr;
  metadata_t * temp = (metadata_t *) (((char *) ptr) - METADATA_T_ALIGNED);

  //part I: free the block and update the freelist
  
  //delete the contents
  for(size_t i=0; i < temp->size; i++) {
    *(p + i) = '\0';
  }
  //add to the freelist
  metadata_t * fp = freelist;
  //1st: empty freelist
  if(fp == NULL) {
    freelist = temp;
  }
  else {
    //2nd: add to the head of freelist
    if(temp < fp) {//add to the head
      temp->next = freelist;
      freelist->prev = temp;
      temp->prev = NULL;
      freelist = temp;
      //printf("here\n");
    }
    //3rd: not add to the head
    else {
      //printf("there\n");
      //find the fp, the first free block after temp
      while( fp->next != NULL && fp < (metadata_t *)p) {
	fp = fp->next;
      }
      //3.1: add to the tail
      if( fp < (metadata_t *)p ) {//add to the tail
	fp->next = temp;
	temp->prev = fp;
	temp->next = NULL;
      }
      //3.2: add to the middle
      else {
	temp->prev = fp->prev;
	fp->prev->next = temp;
	fp->prev = temp;
	temp->next = fp;
      }
    }
  }
  
  //part II: check the freelist and coalese
  //1st: merge with prev
  if( temp->prev != NULL && (temp->prev)->size + METADATA_T_ALIGNED + (char*)(temp->prev) == (char *)temp ) {
    size_t newSize = temp->prev->size + temp->size + METADATA_T_ALIGNED;
    //1.1: temp is not the tail of freelist
    if(temp->next != NULL) {
      fp = temp->next;
      fp->prev = temp->prev;
      temp->prev->next = fp;
      temp = temp->prev;
    }
    //1.2: temp is the tail of freelist
    else {
      temp = temp->prev;
      temp->next = NULL;
    }
    //update the size and clear the contents
    temp->size = newSize;
    p = (char*)temp + METADATA_T_ALIGNED;
    for(size_t i=0; i<newSize; i++) {
      *(p + i) = '\0';
    }
  }

  //2nd: merge with next
  if( temp->next != NULL && (char*)(temp->next) - temp->size - METADATA_T_ALIGNED == (char *)temp ) {
    //printf("temp_size:%zu\ntempnext_size:%zu\nMeTA:%zu\n",temp->size,temp->next->size,METADATA_T_ALIGNED);
    size_t newSize = temp->size + temp->next->size + METADATA_T_ALIGNED;
    //printf("newSize: %zu\n",newSize);
    //1.1: temp->next is not the tail
    if(temp->next->next != NULL) {
      temp->next->next->prev = temp;
      temp->next =temp->next->next; 
    }
    //1.2: temp->next is the tail
    else {
      temp->next = NULL;
    }
    //update the size and clear the contents
    temp->size = newSize;
    p = (char*)temp + METADATA_T_ALIGNED;
    for(size_t i=0; i<newSize; i++) {
      *(p+i) = '\0';
    }
  }
  //printf("freelistsize%zu\n",freelist->size);
  
}//end of free

bool dmalloc_init() {

  /* Two choices: 
   * 1. Append prologue and epilogue blocks to the start and the
   * end of the freelist 
   *
   * 2. Initialize freelist pointers to NULL
   *
   * Note: We provide the code for 2. Using 1 will help you to tackle the 
   * corner cases succinctly.
   */

  size_t max_bytes = ALIGN(MAX_HEAP_SIZE);
  /* returns heap_region, which is initialized to freelist */
  freelist = (metadata_t*) sbrk(max_bytes); 
  /* Q: Why casting is used? i.e., why (void*)-1? */
  if (freelist == (void *)-1)
    return false;
  freelist->next = NULL;
  freelist->prev = NULL;
  freelist->size = max_bytes-METADATA_T_ALIGNED;
  return true;
}

/* for debugging; can be turned off through -NDEBUG flag*/
void print_freelist() {
  metadata_t *freelist_head = freelist;
  while(freelist_head != NULL) {
    DEBUG("\tFreelist Size:%zd, Head:%p, Prev:%p, Next:%p\t",
	  freelist_head->size,
	  freelist_head,
	  freelist_head->prev,
	  freelist_head->next);
    freelist_head = freelist_head->next;
  }
  DEBUG("\n");
}
