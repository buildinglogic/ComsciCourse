      #include  "thread.h"
       #include <stdio.h>
       #include <stdlib.h>
#include <iostream>

      static int count = 0;
       void thread_start(void *arg)
       {
            thread_lock(1);
            count ++;
          
            if(count%5 == 0) {
              std::cout<<"wait, count = "<<count<<std::endl;
              thread_wait(1, 5);
            }
              std::cout<<" execute thread, count = "<<count<<std::endl;
            if(count%5 == 1) {
              std::cout<<"signal, count = "<<count<<std::endl;
              thread_signal(1,5);
            }
            thread_unlock(1);
            return ;
       }

       void pseudo_main(void * arg) {
            
            int num_threads= 500;
            int s;
           for (int tnum = 0; tnum < num_threads; tnum++) {
               s = thread_create( (thread_startfunc_t) thread_start, arg);
               if (s != 0) {
                      return;
               } 
           }

       }

       int main(int argc, char *argv[])
       {
          int s ;
          s = thread_libinit( (thread_startfunc_t) pseudo_main, (void *) NULL);
           if (s != 0) {
              return -1;
           }
        return 0;
       }