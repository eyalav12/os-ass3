#include "ustack.h"
#include "kernel/types.h"
#include "kernel/riscv.h"
#include "kernel/stat.h"
#include "user.h"

#define BLOCKSIZE 512


typedef struct header {
    struct header *next;
    uint len;
} Header;


static Header *stack_top;

void* ustack_malloc(uint len){
    if (len > BLOCKSIZE) {
        return (void*)-1;
    }

    uint size = PGROUNDUP(len);
    if (stack_top && ((char*) stack_top + sizeof(Header) + stack_top->len) + size > (char*) sbrk(0)) {
        return (void*)-1;
    }

    if (stack_top == 0) {
        stack_top = (Header*)sbrk(0);
        if (sbrk(PGSIZE) ==(void*) -1) {
            return (void*)-1;
        }
    }

    if (!stack_top->next) {
        void *new_block = sbrk(0);
        if (sbrk(PGSIZE + sizeof(Header) == -1)) {
            return (void*)-1;
        }
        stack_top->next = (Header*) ((char*)PGROUNDDOWN((uint64)new_block) + PGSIZE);
        stack_top->next->next = 0;
        stack_top->next->len = 0;
    }
     
      

    void* ptr = (char*) stack_top + sizeof(Header) + stack_top->len;
    stack_top->len += size;
    return ptr;
}





int ustack_free(void){
    if (stack_top == 0) {
        return -1;
    }

    Header *header = stack_top;
    stack_top = stack_top->next;

    if (stack_top == 0)  {
        //void* last_page = (char*) header - header->len - sizeof(Header);
        //if (sbrk(-PGSIZE) == -1) {
        //    return -1;
        //}
        sbrk(-PGSIZE);                  //according to their assumption
    }

    return header->len;
}



