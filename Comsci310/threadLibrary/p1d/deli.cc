
/************* main function ************************/
/*	board_limit    			the maxmimum capacity of the board 
	cashier_size  			the number of cashiers
	active_cashier  		how many cashiers still activeorder_on_board  		how many orders on the borard
	largest_num_of_orders   the maxmimum # of orders can be on the board
	board_address 			a pointer to the orders board, implemented by vector< pair<cashier#,sandwich#> >
	
	tinfo 		  		the array of thread_info
	thread_info   		record the all thread info for cashier
	
	board_lock		lock for the board; limit one thread one time
	CV_can_make		condition variable, maker thread could make sandwich
	CV_can_post		condition variable, cashier thread could post order

    ********* maker(pseudo main) *******************

	while( boar.size()() > 0) {
		lock(board_lock) 
		while( board.size()() < largest_num_of_orders) {         
			wait(board_lock, CV_can_make)								
		}	
		choose the closest sandwich and make it--post it + remove from board				 
		broadcast(CV_can_post)
		unlock(board_lock)
	}								        												
	
	**************** cashier  *********************
	
	while(has order waiting for order) {
		lock(board_lock)
		while( board.find(cashier_x) || board.size()()==largest_num_of_orders ) {
			wait(block_lock, CV_can_post)
		}
		post an order to the board
		signal(CV_can_make)
		unlock(board_lock)
	}	
	
*/
/******************************************************************/


#include "thread.h"   //thread library
#include <fstream>  //read in files
#include <iostream> // std::cout
#include <cstdlib>  //atoi, abs
#include <algorithm> //std::min
#include <vector>
#include <errno.h> //errno
#include <utility> //pair
//using namespace std;

/* error handle MACRO   */
#define handle_error_en(en, msg) \
	do {errno = en; perror(msg); exit(EXIT_FAILURE);} while(0)
#define hand_error(msg)\
	do {perror(msg); exit(EXIT_FAILURE);} while{0}

/*    lock and CVs     */
static int board_lock = 1;
static int CV_can_post = 10;
static int CV_can_make = 11;

/*    struct for thread      */
struct _thread_info { /*used as argument to thread_create*/
	//pthread_t thread_id;    /*ID returned by pthread_create()*/
	int 	thread_num;     /*application defined thread # (cashier #)*/
	char *argv_string;      /*from command-line argument*/
};
typedef struct _thread_info thread_info;

/*     global variable      */
static int board_limit = 0; //cord_board size()
static int cashier_size = 0; //# of cashiers
static int active_cashier = 0; //cashiers still active
//static int sandwich_left = 0; //sandwichs left
static int largest_num_of_orders = 0; //std::min(board_limit, active_cashier); //need to manual update

static std::vector< std::pair<int,int> > *board_address; //cashier and maker share the same board


 static bool contain_cashier(std::vector< std::pair<int, int> > *temp, int m)
 {
 	for(size_t i=0; i<temp->size(); i++) {
 		if( (temp->at(i)).first == m) {
 			return true;
 		}
 	}

 	return false;
 }         


void cashier (void * arg) 
{
	thread_info *tinfo = (thread_info *)arg;
	int cashier_x = tinfo->thread_num;
	std::ifstream cashier_file(tinfo->argv_string);
	std::vector<int> sandwich_array;
	int temp = 0;
	while(cashier_file >> temp) {
		sandwich_array.push_back(temp);
	}
/*
	for(std::vector<int>::iterator it = sandwich_array.begin(); it<sandwich_array.end(); ++it) {
	std::cout<<*it<<std::endl;
}
	std::cout<<"sanwich array size"<<sandwich_array.size()<<std::endl;
*/
	for(size_t i=0; i<sandwich_array.size(); i++) {
		thread_lock(board_lock);
		while( contain_cashier(board_address, cashier_x) || \
					(int) board_address->size() == largest_num_of_orders) {
			thread_wait(board_lock, CV_can_post);
		}
		std::cout << "POSTED: cashier " << cashier_x << " sandwich " << sandwich_array[i] << std::endl;
		std::pair<int,int> foo;
		foo = std::make_pair(cashier_x, sandwich_array[i]);
		board_address->push_back(foo);
		//thread_yield();
		if( (int)board_address->size() == largest_num_of_orders ) {
			thread_signal(board_lock, CV_can_make);
		}		

		thread_wait(board_lock, cashier_x);
		thread_unlock(board_lock);
//std::cout<<"cashier "<< cashier_x << "loop"<<i<<std::endl;
	}
		active_cashier--;
		largest_num_of_orders = std::min(active_cashier, board_limit);
		thread_signal(board_lock, CV_can_make);
	return;
}

//this is equal to the maker thread
void pseudo_main(void * arg)
{
	//start_preemptions(true, true, 5);	
	thread_info *tinfo = (thread_info *)arg;
	int s;
	int current_sandwich = -1;

	/*   create the board vector   */
	std::vector< std::pair<int, int> > *board = new std::vector< std::pair<int, int> >();
	board_address = board;

	/*  create cashier thread   */
	for(size_t i=0; (int)i<cashier_size; i++) {
		s = thread_create( (thread_startfunc_t) cashier, tinfo+i);
		if (s != 0) {
			handle_error_en(s, "thread_create failed");
		}
	}
	
	while( active_cashier > 0 || board->size() > 0 ) {	
		thread_lock(board_lock);
		while( (int)board->size() < largest_num_of_orders) {
			thread_wait(board_lock, CV_can_make);
		}
		if( board->size() == 0) {
			thread_unlock(board_lock);
			break;
		}
		int choose = 0;
		int difference = abs( (board->at(0)).second - current_sandwich);
		for(size_t i=0; i<board->size(); i++) {
			if( abs((board->at(i)).second - current_sandwich) < difference) {
				difference = abs((board->at(i)).second - current_sandwich);		// make change here		
				choose = i;
			}
		}
		int posted_cashier = 0;
		posted_cashier = (board->at(choose)).first;
		current_sandwich = (board->at(choose)).second;
		std::cout << "READY: cashier " << (board->at(choose)).first << " sandwich " << (board->at(choose)).second << std::endl;
		board->erase( board->begin() + choose );
		
		thread_broadcast(board_lock, CV_can_post);
		thread_signal(board_lock, posted_cashier);
		thread_unlock(board_lock);
	}

	delete board;
	delete[] tinfo;
	return;
}


int main(int argc, char* argv[])
{
    thread_info * tinfo; 
	int s; 
	if(argc < 3) {
		perror("Input: ./xx board_limit cashier1 cashier2 ...");
		exit(EXIT_FAILURE);
	}
	board_limit = atoi(argv[1]);
	cashier_size = argc -2;
	active_cashier = cashier_size; //all cashiers active for initiation
	tinfo = new thread_info[cashier_size];
  	largest_num_of_orders = std::min(active_cashier, board_limit);

	//read in the cashier files and store into tinfo
	for(int i=0; i<cashier_size; i++) {
		tinfo[i].thread_num = i ;
		tinfo[i].argv_string = argv[i+2];
	}

	//jump to the pseudo_main thread, equal to "maker thread"
	s = thread_libinit( (thread_startfunc_t) pseudo_main, tinfo);
	if (s != 0) {
		handle_error_en(s, "thread_libinit failed");
	}
	return 0;
}// end of main function







