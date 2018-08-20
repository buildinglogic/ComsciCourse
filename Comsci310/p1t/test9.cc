#include "thread.h"
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <errno.h> //errno

using namespace std;

#define handle_error_en(en, msg) \
	do {errno = en; perror(msg); exit(EXIT_FAILURE);} while(0)

void two(void* arg) {
    cout << "should enter here\n";
}

void one(void* arg) {
    int t = thread_libinit((thread_startfunc_t) two, (void*) NULL);
    if (t != -1) {
        handle_error_en(t, "wrong: calling thread_libinit more than once");
    }
}

int main(int argc, char *argv[]) {
    int s =  thread_libinit((thread_startfunc_t) one, (void*) NULL);
    if (s != 0) {
	handle_error_en(s, "thread_libinit failed");
    }
    return 0;
}
