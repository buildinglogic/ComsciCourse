#include "thread.h"
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <errno.h> //errno

using namespace std;

#define handle_error_en(en, msg) \
  do {errno = en; perror(msg); exit(EXIT_FAILURE);} while(0)

void one(void* arg) {
    cout <<"test program\n";
}

int main(int argc, char *argv[]) {
    int s = thread_create((thread_startfunc_t) one, (void*) NULL);
    if ( s != -1) {
  handle_error_en(s, "wrong: called thread_create before thread_libinit");
    } 
    
    s = thread_lock(0);
    if (s != -1) {
  handle_error_en(s, "wrong: called thread_lock before thread_libinit");
    } 
    
    s = thread_unlock(0);
    if (s != -1) {
      handle_error_en(s, "wrong: called thread_unlock before thread_libinit");
    } 

    s = thread_yield();
    if (s != -1) {
      handle_error_en(s, "wrong: called thread_yield before thread_libinit");
    } 

    s = thread_wait(0,0);
    if (s != -1) {
      handle_error_en(s, "wrong: called thread_wait before thread_libinit");
    }

    s = thread_signal(0,0);
    if (s != -1) {
      handle_error_en(s, "wrong: called thread_signal before thread_libinit");
    } 

    s = thread_broadcast(0,0);
    if (s != -1) {
      handle_error_en(s, "wrong: called thread_broadcast before thread_libinit");
    } 
    
    return 0;
}
