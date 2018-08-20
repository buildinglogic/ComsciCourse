/*************************** thread.h ********************************

//#define STACK_SIZE 262144  

typedef void (*thread_startfunc_t) (void *);
extern int thread_libinit(thread_startfunc_t func, void *arg);
extern int thread_create(thread_startfunc_t func, void *arg);
extern int thread_yield(void);
extern int thread_lock(unsigned int lock);
extern int thread_unlock(unsigned int lock);
extern int thread_wait(unsigned int lock, unsigned int cond);
extern int thread_signal(unsigned int lock, unsigned int cond);
extern int thread_broadcast(unsigned int lock, unsigned int cond);
extern void start_preemptions(bool async, bool sync, int random_seed);

**************************** thread.h *******************************/

/*

// typedef struct ucontext {                       
//                struct ucontext *uc_link;
//                sigset_t         uc_sigmask;
//                stack_t          uc_stack;
//                mcontext_t       uc_mcontext;
//                ...
//            } ucontext_t;

*/


#include <ucontext.h>
#include "interrupt.h"
#include "thread.h"
#include <iostream>        // std::cout
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <queue>

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE);} while(0)

static int thread_libinit_called = 0;
static int thread_created = 0;
static int total_delete_count = 0;

static ucontext_t* running_t;
static ucontext_t Main;
static ucontext_t deleteThread_thread;
static ucontext_t* deleteThread;
static ucontext_t* initial_thread;
static ucontext_t * will_delete_thread = NULL;

static std::queue<ucontext_t*> ready_q;
static std::map<unsigned int,ucontext_t*> lock_state;         // <lock number, lock owner>
static std::map<unsigned int, std::queue<ucontext_t*> > lock_q; //lock queue
static std::map< unsigned int, std::map<unsigned int, std::queue<ucontext_t*> > > cv_q; // < <lock, CV>, queue>ss

typedef std::map< unsigned int, std::map<unsigned int, std::queue<ucontext_t*> > > lockCVmap;
int cleanlostthread ();
void print_map( std::map<unsigned int, std::queue<ucontext_t*> > &temp);
void print_CVmap( std::map<unsigned int, std::queue<ucontext_t*> > &temp);
void print_mapmap(lockCVmap &temp);
void print_lock_stat( std::map<unsigned int,ucontext_t*> &temp);

static bool interruptOn = true;

static void interrupt_enabled () {                                                                       
    while (interruptOn == false) {
        interruptOn = true;                                                                           
        interrupt_enable();
    }
}

static void interrupt_disabled () {                                                                      
    while (interruptOn == true) {
        interrupt_disable();
        interruptOn = false;                                                                      
    }
}

static int context_create(ucontext_t *ucontext_ptr){

	if(getcontext(ucontext_ptr) == -1) {               // getcontext: save CPU core context to memory
		//handle_error("getcontext failure");
		interrupt_enabled();
		//cleanlostthread();
		//exit(-1);
		return -1;
	}
	try{
		char *stack = new char [STACK_SIZE];            // STACK_SIZE 262144
		ucontext_ptr->uc_stack.ss_sp = stack;           // points to stack base
		ucontext_ptr->uc_stack.ss_size = STACK_SIZE;
		ucontext_ptr->uc_stack.ss_flags = 0;           // stack being used or not
		ucontext_ptr->uc_link = NULL;                  // uc_link points to the context that will be resumed when the current context terminates
		if (stack == NULL) {return -1;}
	}
	catch (std::bad_alloc& ba) {
		//std::cout<<"Allocation failed: "<<ba.what()<<"\n";
		interrupt_enabled();
		return -1;
	}
	thread_created++;
	return 0;
}

static int deleteFunct()
{
	if(running_t == NULL) {
		return -1;
	}
	ucontext_t* t = will_delete_thread;
	will_delete_thread = NULL;
	running_t = NULL;

	//std::cout<<"the stack size: "<<t->uc_stack.ss_size<<"\n";
	if( t->uc_stack.ss_sp != NULL ) {
		delete (char*) t->uc_stack.ss_sp ;
		total_delete_count ++;
	}
	delete t;

	//std::cout<<"after free \n";
	if( ready_q.empty() ) {
		std::cout<<"Thread library exiting.\n";
		if(total_delete_count != thread_created -1) {
			cleanlostthread();
		}
		//std::cout<<"thread_created = "<<thread_created<<std::endl;
		//std::cout<<"total_delete_count = "<<total_delete_count<<std::endl;
		exit(0);
	} else {
		running_t = ready_q.front();
		ready_q.pop();
		if( setcontext(running_t) == -1) {
			return -1;
		}
	}
	return 0;
}

 int cleanlostthread () {
	//clean lock_state
	if( !lock_state.empty() ) {
		lock_state.clear();
	}
	
	//clean lock_q
	if( !lock_q.empty() ) {
		for( std::map<unsigned int, std::queue<ucontext_t*> > ::iterator it = lock_q.begin(); \
			it != lock_q.end(); ++it) {
			while( !(it->second).empty() ) {
				(it->second).pop();
				total_delete_count++;
			}
		}
		lock_q.clear();
	}

	//clean cv_q
	if( !cv_q.empty()) {
		for( lockCVmap::iterator it = cv_q.begin(); it != cv_q.end(); ++it ) {

			if( !(it->second).empty() ) {
				for( std::map<unsigned int, std::queue<ucontext_t*> > ::iterator CVit = (it->second).begin(); \
					CVit != (it->second).end(); ++CVit) {
					while( !(CVit->second).empty() ) {
						(CVit->second).pop();
						total_delete_count++;
					}
				}
				(it->second).clear();
			}

		}
		cv_q.clear();
	}

	return 0;
}

static int stub(thread_startfunc_t func, void *arg){
	interrupt_enabled();
	func(arg); // or (*func)(argc)
	interrupt_disabled();
	will_delete_thread = running_t;
	running_t = deleteThread;
	//std::cout<<"the stack size: "<<will_delete_thread->uc_stack.ss_size<<"\n";
	swapcontext(will_delete_thread, deleteThread);	
	return 0;
}


static int context_switch(void) {
	interrupt_disabled();
	if( ready_q.empty() ) {
		std::cout<<"Thread library exiting.\n";
		if(total_delete_count != thread_created -1) {
			cleanlostthread();
		}
		exit(0);
		//interrupt_enabled();
		//return -1;
	}
	else { 
		ucontext_t *t = ready_q.front();
		ucontext_t *temp = running_t;
		running_t = t;
		ready_q.pop();
		if( swapcontext(temp,t) == -1 ) {
			//handle_error("swapcontext failed");
			interrupt_enabled();
		    	return -1;
		}
	} 
	interrupt_enabled();
	return 0;
}


int thread_libinit(thread_startfunc_t func, void *arg){
	interrupt_disabled();
	thread_libinit_called++;
	if(thread_libinit_called > 1) {                 // calling thread_libinit more than once
		interrupt_enabled();
		return -1;
	}
	
	deleteThread = &deleteThread_thread;
	if(context_create(deleteThread) == -1) {
		interrupt_enabled();
		return -1;
	}
	makecontext(deleteThread, (void(*)()) deleteFunct, 0, NULL);

	try {
		 initial_thread = new ucontext_t();       
	}
	catch (std::bad_alloc& ba) {
		interrupt_enabled();
		return -1;
	}
	if(context_create(initial_thread) == -1) {
		interrupt_enabled();
		return -1;
	}
	makecontext(initial_thread, (void(*)()) stub, 2, func, arg);
	
	running_t = initial_thread;
	if( swapcontext(&Main, initial_thread) == -1 ) {
		//handle_error("swapcontext failed");
		interrupt_enabled();
		return -1;
	}
	interrupt_enabled();
	return 0;
}


int thread_create(thread_startfunc_t func, void *arg) {
	interrupt_disabled();
	if(thread_libinit_called != 1) {      // calling another thread function before thread_libinit
		interrupt_enabled();
		return -1;
	}
	try {
		ucontext_t *t = new ucontext_t();
		if(context_create(t) == -1) {
			interrupt_enabled();
			return -1;
		}
		makecontext(t, (void(*)()) stub, 2, func, arg);
		if (t == NULL) {
			interrupt_enabled();
			return -1;
		}
		ready_q.push(t);       
	}
	catch (std::bad_alloc& ba) {
		interrupt_enabled();
		return -1;
	}	

	interrupt_enabled();
	return 0;
}


int thread_yield(void) {
	//caller running -> ready, goes to tail of ready queue, keep the lock
	//another thread from ready -> running
	interrupt_disabled();
	if(thread_libinit_called != 1) {
		interrupt_enabled();
		return -1;
	}
	//print_lock_stat(lock_state);
	if( !ready_q.empty() ) {
		ready_q.push(running_t);
		if(context_switch() == -1) {
			interrupt_enabled();
			//handle_error("context_switch");
			return -1;
		}	
	}
	
	interrupt_enabled();
	return 0;
}


int thread_lock(unsigned int lock) { 
	//if lock free, acquire it, continue to run
	//else caller running -> blocked, another thread ready -> running         
	interrupt_disabled();
	//std::cout<<"thread_lock\n";
	
	if(thread_libinit_called != 1 ) { // a thread that tries to acquire a lock it already has
		interrupt_enabled();
		return -1;
	}

	std::map<unsigned int,ucontext_t*>::iterator tempIt = lock_state.find(lock);
	if( tempIt != lock_state.end() ) {
		if( tempIt->second == running_t ) {
			interrupt_enable();
			return -1;
		}
	}
	
	if(lock_state.count(lock) == 0) { 
		lock_state.insert( std::pair<unsigned int, ucontext_t *> (lock, running_t) );           
	} 
	else {
		//std::cout<<"in else \n";
		if(lock_q.count(lock) == 0) { 
			std::queue<ucontext_t *> temp_queue; 
			temp_queue.push(running_t);
			lock_q.insert ( std::pair<unsigned int, std::queue<ucontext_t*> > (lock, temp_queue) );	
		}
		else {
			lock_q[lock].push(running_t);
		}
		if(context_switch() == -1) {
			interrupt_enabled();
			return -1;	
		} 
	}
	interrupt_enabled();
	/******************************************/
	//std::cout<<"right after thread_lock, ";
	//print_lock_stat(lock_state);
	return 0;
}


int thread_unlock(unsigned int lock) {
	//caller release lock, continue running
	//if lock queue not empty, lock -> ready, give it the lock
	interrupt_disabled();
	//std::cout<<"thread_unlock\n";
	if(thread_libinit_called != 1 ) {  
		interrupt_enabled();
		return -1;
	}
	if( lock_state.find(lock)==lock_state.end() ) {
		interrupt_enabled();
		return -1;
	} else {
		if ( (lock_state.find(lock))->second != running_t ) {		
			interrupt_enabled();
			return -1;
		}
	}

	lock_state[lock] = NULL;
	if( lock_q.find(lock) != lock_q.end() ) {
		if( !lock_q[lock].empty() ){
			ucontext_t *t = lock_q[lock].front();
			lock_state[lock] = t;
			ready_q.push(t);
			lock_q[lock].pop();
		} 
		else {
			lock_state.erase(lock);
		}
	} else {
		lock_state.erase(lock);
	}
	
	interrupt_enabled();
	//std::cout<<"right after thread_unlock, ";
	//print_lock_stat(lock_state);
	//std::cout<<"ready_q size: "<<ready_q.size()<<std::endl;
	return 0;
}


int thread_wait(unsigned int lock, unsigned int cond){
	//caller unlock the lock, running -> block, to waiter queue
	//another thready ready -> running
	//later, upon waking up, acquire the lock, run
	interrupt_disabled();
	//std::cout<<"thread_wait\n";
	if(thread_libinit_called != 1) {
		interrupt_enabled();
		return -1;
	}
	if(thread_unlock(lock) == -1) {
		interrupt_enabled();
		return -1;
	}

	lockCVmap::iterator it = cv_q.find(lock);
	if(it == cv_q.end()) { //no this lock
		std::map<unsigned int, std::queue<ucontext_t*> > tempMap;
		std::queue<ucontext_t *> temp_queue; 
		temp_queue.push(running_t);
		tempMap.insert ( std::pair<unsigned int, std::queue<ucontext_t*> >(cond, temp_queue) );
		cv_q.insert ( std::pair<unsigned int, std::map<unsigned int, std::queue<ucontext_t*> > >(lock, tempMap) );
	} else {
		if( (it->second).count(cond) == 0) {    //has this lock, no this CV     
			std::queue<ucontext_t *> temp_queue; 
			temp_queue.push(running_t);
			(it->second).insert ( std::pair<unsigned int, std::queue<ucontext_t*> > (cond, temp_queue) );
		} else {     //has this lock, has this CV
			std::map<unsigned int, std::queue<ucontext_t*> >::iterator cvIt;
			cvIt = (it->second).find(cond);
			(cvIt->second).push(running_t);
		}
	}
	//std::cout<<"before context_switch\n";
	//print_mapmap(cv_q);
	if(context_switch() == -1) {
		interrupt_enabled();
		return -1;	
	}
	if(thread_lock(lock) == -1) {
		interrupt_enabled();
		return -1;
	}
	interrupt_enabled();
	//std::cout<<"get back lock in thread_wait, ";
	//print_lock_stat(lock_state);
	//print_mapmap(cv_q);
	return 0;
}


int thread_signal(unsigned int lock, unsigned int cond){
	//if CV waiter is not empty, blocked -> ready
	interrupt_disabled();
	//std::cout<<"thread_signal\n";
	if(thread_libinit_called != 1) {
		interrupt_enabled();
		return -1;
	}
	//print_mapmap(cv_q);
	//std::cout<<"thread_signal: lock = "<<lock<<", cond = "<<cond<<std::endl;
	lockCVmap::iterator it = cv_q.find(lock);
	if( it == cv_q.end() ) { // signal a nonexisting lock CV pair
		interrupt_enabled();
		return -1;
	} else {  
		std::map<unsigned int, std::queue<ucontext_t*> >::iterator cvIt;
		cvIt = ( (*it).second ).find(cond);
		if( cvIt == ( (*it).second ).end() ) {  //has lock, no this CV
			interrupt_enabled();
			//std::cout<<"hre\n";
			return -1;
		} else {
			//std::cout<<"here\n";
			if( !( ( (*cvIt).second ).empty() ) ){ //the lock, the CV waiter list
				ready_q.push( ( (*cvIt).second ).front() );
				( (*cvIt).second ).pop();
			}
		}
	}
	
	interrupt_enabled();
	//std::cout<<"after thread_signal: \n";
	//print_mapmap(cv_q);
	return 0;
}


int thread_broadcast(unsigned int lock, unsigned int cond) {
	interrupt_disabled();
	if(thread_libinit_called != 1) {
		interrupt_enabled();
		return -1;
	}

	lockCVmap::iterator it = cv_q.find(lock);
	if( it == cv_q.end() ) { // broadcast a nonexisting lock CV pair
		interrupt_enabled();
		return -1;
	} else {  
		std::map<unsigned int, std::queue<ucontext_t*> >::iterator cvIt;
		cvIt = (it->second).find(cond);
		if( cvIt == (it->second).end() ) {  //has lock, no this CV
			interrupt_enabled();
			return -1;
		} else {
			while( !( (cvIt->second).empty() ) ){
				ready_q.push( (cvIt->second).front() );
				(cvIt->second).pop();
			}
		}	
	}

	interrupt_enabled();
	return 0;
}

 void print_lock_stat( std::map<unsigned int,ucontext_t*> &temp) {
	std::cout<<"lock_state are: \n";
	for( std::map<unsigned int,ucontext_t*>::iterator it = temp.begin(); it != temp.end(); ++it) {
		std::cout<<"< "<<it->first<<" , "<< (*it).second<<" >\n";
	}
	if(temp.empty()) {
		std::cout<<"< , >\n";
	}
}


 void print_map( std::map<unsigned int, std::queue<ucontext_t*> > &temp) {
	/*****lock#, lockqueue size) or (CV#, CV-waiter queue size****/
	if(temp.empty()) {
		std::cout<<"map is NULL\n";
		return;
	} else {
		for( std::map<unsigned int, std::queue<ucontext_t*> > ::iterator it = temp.begin(); it != temp.end(); ++it) {
			std::cout<<"( lock = "<<(*it).first<<" , "<<(*it).second.size()<<" )\n";
		}
	}
}

 void print_CVmap( std::map<unsigned int, std::queue<ucontext_t*> > &temp) {
	/*****lock#, lockqueue size) or (CV#, CV-waiter queue size****/
	if(temp.empty()) {
		std::cout<<"map is NULL\n";
		return;
	} else {
		for( std::map<unsigned int, std::queue<ucontext_t*> > ::iterator it = temp.begin(); it != temp.end(); ++it) {
			std::cout<<"( CV = "<<(*it).first<<" , "<<(*it).second.size()<<" )\n";
		}
	}
}

 void print_mapmap(lockCVmap &temp) {
	std::cout<<"cv_q map is: "<<std::endl;
	std::cout<<"[ \n";
	for(lockCVmap::iterator it = temp.begin(); it != temp.end(); ++it) {
		std::cout<<" < lock ="<<(*it).first<<" , \n";
		print_CVmap( (*it).second );
		std::cout<<" > \n";
	}
	std::cout<<"]\n";
}



