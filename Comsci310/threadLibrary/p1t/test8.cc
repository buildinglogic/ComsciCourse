#include "thread.h"
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <errno.h> //errno

using namespace std;

#define handle_error_en(en, msg) \
	do {errno = en; perror(msg); exit(EXIT_FAILURE);} while(0)


void one(void* arg) {

    int t = thread_lock(1);	
    if (t == -1) {
        handle_error_en(t, "wrong: thread_lock failed");    // initial acquire failed
    }
    
    t = thread_lock(1);
    if (t != -1) {
        handle_error_en(t, "wrong: tries to acquire a lock it already has");
    }
    
    t = thread_unlock(1);
    if (t == -1) {
         handle_error_en(t, "wrong: thread_unlock failed"); 
    }
    
    t = thread_unlock(1);
    if (t != -1) {
         handle_error_en(t, "wrong: release a lock it doesn't have"); 
    }
   
}

int main(int argc, char *argv[]) {
    int s =  thread_libinit((thread_startfunc_t) one, (void*) NULL);
    if (s != 0) {
	handle_error_en(s, "thread_libinit failed");
    }
    return 0;
}
