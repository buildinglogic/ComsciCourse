#include "thread.h"
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <errno.h> //errno

using namespace std;

#define handle_error_en(en, msg) \
	do {errno = en; perror(msg); exit(EXIT_FAILURE);} while(0)


void one(void* arg) {
    thread_lock(0);
    thread_lock(1);
    cout <<"wait...\n";
    int t = thread_wait(0,0);
    if (t == -1) {        
        handle_error_en(t, "wrong: thread_wait failed.");
    }
    int r = thread_wait(1,0);
    if (r != -1) {        
        handle_error_en(t, "wrong: Every CV is bound to exactly one mutex");
    }

}

int main(int argc, char *argv[]) {
    int s =  thread_libinit((thread_startfunc_t) one, (void*) NULL);
    if (s != 0) {
	handle_error_en(s, "thread_libinit failed");
    }
    return 0;
}
