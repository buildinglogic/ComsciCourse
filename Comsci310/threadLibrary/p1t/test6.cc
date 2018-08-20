#include <stdlib.h>
#include <iostream>
#include "thread.h"
#include <assert.h>
#include <string.h>

using namespace std;

int g=0;

void loop(void *a) {
  char *id;
  int i;

  id = (char *) a;
  cout <<"loop called with id " << (char *) id << endl;

  for (i=0; i<5; i++, g++ ) {
	//if( strcmp(id, "parent thread")==0) { cout<<"in parent\n"; exit(1);}
    cout << id << ":\t" << i << "\t" << g <<endl;
    //if( strcmp(id, "child thread")==0) { cout<<"in child\n"; exit(1);}  
    if (thread_yield()) {
      cout << "thread_yield failed\n";
      exit(1);
    }
    //if( strcmp(id, "child thread")==0) { cout<<"in child\n"; exit(1);}
    //cout << "after thread_yield \n";
  }
}

void parent(void *a) {
  int arg;
  arg = (long int) a;

  cout << "parent called with arg " << arg << endl;
  if (thread_create((thread_startfunc_t) loop, (void *) "child thread")) {
    cout << "thread_create failed\n";
    exit(1);
  }

  loop( (void *) "parent thread");
}

int main() {
  if (thread_libinit( (thread_startfunc_t) parent, (void *) 100)) {
    cout << "thread_libinit failed\n";
    exit(1);
  }
}
