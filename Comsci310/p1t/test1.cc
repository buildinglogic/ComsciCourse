#include <stdlib.h>
#include <iostream>
#include "thread.h"

void finish(void * arg) {
	return;
}

int main(int argc, char *argv[]) {
    int s =  thread_libinit((thread_startfunc_t) finish, (void*) NULL);
    exit(0);
    return 1;
}