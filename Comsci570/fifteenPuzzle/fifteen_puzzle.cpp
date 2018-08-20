#include <stdio.h> //printf
#include <queue> // priority_queue
#include <vector> // vector S
#include <fstream> //ifstream
//#include <cmath> //std::abs
#include <cstdlib> //abs
#include <iostream> //cout
#include <iomanip> //std::stew
#include <utility>      // std::pair, std::make_pair

using namespace std;

int knightmoveDistance[256] = {
	0, 3, 2, 5, 3, 4, 1, 2, 2, 1, 4, 3, 5, 2, 3, 2, 
3, 0, 3, 2, 2, 3, 2, 1, 1, 2, 1, 4, 2, 3, 2, 3, 
2, 3, 0, 3, 1, 2, 3, 2, 4, 1, 2, 1, 3, 2, 3, 2, 
5, 2, 3, 0, 2, 1, 4, 3, 3, 4, 1, 2, 2, 3, 2, 5, 
3, 2, 1, 2, 0, 3, 2, 3, 3, 2, 1, 2, 2, 1, 4, 3, 
4, 3, 2, 1, 3, 0, 3, 2, 2, 3, 2, 1, 1, 2, 1, 4, 
1, 2, 3, 4, 2, 3, 0, 3, 1, 2, 3, 2, 4, 1, 2, 1, 
2, 1, 2, 3, 3, 2, 3, 0, 2, 1, 2, 3, 3, 4, 1, 2, 
2, 1, 4, 3, 3, 2, 1, 2, 0, 3, 2, 3, 3, 2, 1, 2, 
1, 2, 1, 4, 2, 3, 2, 1, 3, 0, 3, 2, 4, 3, 2, 1, 
4, 1, 2, 1, 1, 2, 3, 2, 2, 3, 0, 3, 1, 2, 3, 4, 
3, 4, 1, 2, 2, 1, 2, 3, 3, 2, 3, 0, 2, 1, 2, 3, 
5, 2, 3, 2, 2, 1, 4, 3, 3, 4, 1, 2, 0, 3, 2, 5, 
2, 3, 2, 3, 1, 2, 1, 4, 2, 3, 2, 1, 3, 0, 3, 2, 
3, 2, 3, 2, 4, 1, 2, 1, 1, 2, 3, 2, 2, 3, 0, 3, 
2, 3, 2, 5, 3, 4, 1, 2, 2, 1, 4, 3, 5, 2, 3, 0
};
	
class Node;

bool compare(Node *, Node*);

class Node {

		int g;
		int h;
		int f;
		pair<int,int> swith_move;
		Node * parent;
		int* state; // array of numbers: 1,2,3,4,5,6,0,8,9,10,11,12,13,14,15,7 (example on HW1)
	
	public:
	//constructor
	Node(int a, int b, int c, Node * temp, int * rhs): g(a),h(b),f(c),parent(temp){
		swith_move.first = 0;
		swith_move.second = 0;
		h=Calculate_H(rhs);
		f=g+h;
		int * new_state = new int[16]();
		for(size_t i=0; i<=15; i++) {
			new_state[i]=rhs[i];
		}
		state = new_state;
	}

	Node(const Node & rhs): g(rhs.g),h(rhs.h) {
			swith_move.first = rhs.swith_move.first;
			swith_move.second = rhs.swith_move.second;
			f=g+h;
			parent=rhs.parent;
			int* new_state = new int[16]();
			for(size_t i=0; i<=15; i++) {
				new_state[i]=rhs.state[i];
			}
			state = new_state;
	}

	Node & operator=(const Node & rhs) {
		if(this != &rhs) {
			int* new_state = new int[16]();
			for(size_t i=0; i<=15; i++) {
				new_state[i]=rhs.state[i];
			}
			delete[] state;
			state = new_state;
			swith_move.first = rhs.swith_move.first;
			swith_move.second = rhs.swith_move.second;
			g = rhs.g;
			h = rhs.h;
			f=g+h;
		}
		return *this;
	}

	~Node(){
		delete[] state;
	}


	void Print_State();

	void printNode();
	
	void printSuccess(Node *);

	int Get_F() const {
		return f;
	}

	bool Goal_Test(Node * current)
	{
		if(current == NULL) {return false;}
		for(size_t i=0; i<15; i++) {
			if( (current->state)[i] != (i+1)) {
				return false;
			}
		}
		if( (current->state)[15] != 0 ) {
			return false;
		}
		return true;
	}

	// int Calculate_H(int * s){ // Manhattan distance
	// 	int distance = 0;
	// 	for (int i=0; i<=15; i++) {
	// 		if( s[i] != 0){
	// 			distance += abs( (s[i]-1) / 4 - i/4)+ abs( (s[i]-1) % 4 - i%4);
	// 		} else {
	// 			distance += 3-i/4 + 3-i%4;
	// 		}
			
	// 	}
	// 	return distance;
	// }

	int Calculate_H(int * s){
		int distance = 0;
		for (int i=0; i<=15; i++) {
			int to_row = (s[i]-1)/4;
			int to_col = (s[i]-1)%4;
			int j = 4*to_row + to_col;
			int k = 16*i +j;
			distance += knightmoveDistance[k];
		}
		return distance;

	}

	void Switch_Position_State(int i, int j){
		int temp = state[i];
		state[i] = state[j];
		state[j] = temp;
	}

	Node * Child_Node_Generate(Node * parent, int to_space, int from){
		Node * newNode = new Node(*parent);
		newNode->swith_move.first = (parent->state)[to_space];
		newNode->swith_move.second = (parent->state)[from];
		newNode->Switch_Position_State(to_space, from);
		newNode->g++;
		newNode->h = Calculate_H(newNode->state);
		newNode->f = newNode->g + newNode->h;
		newNode->parent = parent;
		return newNode;
	}

	int Find_Zero(Node * current){
		for(size_t i=0; i<=15; i++) {
			if( (current->state)[i] == 0) {
				return i;
			}
		}
		exit(EXIT_FAILURE);
	}

	// //Find_Actions for adjacent move
	// void Find_Actions(vector<int> & move_positions, int i) {
	// 	int temp[4]={0};
	// 	temp[0] = i-4;
	// 	temp[1] = i-1;
	// 	temp[2] = i+1;
	// 	temp[3] = i+4;
	// 	for(size_t i=0; i<=3; i++) {
	// 		if( temp[i]>=0 && temp[i] <=15) {
	// 			move_positions.push_back(temp[i]);
	// 		}
	// 	}
	// }

	//Find_Actions for "knight" move
	void Find_Actions(vector<int> & move_positions, int i) {
		//vector of available positions to move
		if( i+2 <= 15 && (i+2)/4 - i/4 == 1) { //move: left 2 down 1
			move_positions.push_back(i+2);
		} 
		if( i+6 <=15 && (i+6)/4 - i/4 ==1 ) {//move:right 2 down 1
			move_positions.push_back(i+6);
		} 
		if( i+7 <=15 && (i+7)/4 -i/4 == 2) {//move: left 1 down 2
			move_positions.push_back(i+7);
		} 
		if( i+9 <=15 && (i+9)/4 - i/4 ==2) {//move: right 1, down 2
			move_positions.push_back(i+9);
		}
		if( i-2 >=0 &&  (i-2)/4 -i/4 ==-1) {//move: right 2, up 1
			move_positions.push_back(i-2);
		} 
		if( i-6 >=0 && (i-6)/4 -i/4 == -1) {//move: left 2, up 1
			move_positions.push_back(i-6);
		} 
		if(i-7 >=0 && (i-7)/4 - i/4 ==-2) {// move: right 1, up 2
			move_positions.push_back(i-7);
		}
		if(i-9 >=0 && (i-9)/4 - i/4 ==-2) {// move: left 1, up 2
			move_positions.push_back(i-9);
		}
	}

	bool States_Equal(Node * a, Node * b) {
		for(size_t i=0; i<=15; i++) {
			if( (a->state)[i] - (b->state)[i] != 0) {
				return 0;
			}
		}
		return 1;
	}

	int Graph_Search(priority_queue< Node *, vector<Node *>, decltype(&compare) > & pq, vector<Node *> * visited){
		if( pq.empty() ) {
			std::cout<<"failed till no elment in Queue"<<endl;
			return 0;
		}
		Node * current = pq.top();
		pq.pop();
		
		//current->printNode(); // for test: print the pq's top element

		if ( Goal_Test(current) ) {
			cout<<"Success with steps: "<<current->g<<endl; //out current
			//cout<<"Print sequential states in reverse order: "<<endl;
			printSuccess(current);
			return 1; //find the goal
		}
		int space_position = Find_Zero(current);
		//cout<<"zero at: "<<space_position<<endl;

		vector<int> actions;
		Find_Actions(actions, space_position);

		 //for test: print out the actios vector
		// for(int i=0; i<actions.size();i++) {
		// 	cout<<actions[i]<<" ";
		// }

		for (size_t i=0; i<actions.size(); i++) {
			Node * child = Child_Node_Generate(current, space_position, actions[i] ); 
			//child->printNode();
			bool duplicate_state = false;
			for(size_t j=0; j<visited->size(); j++) {
				if ( States_Equal(child, visited->at(j)) ) {
					duplicate_state =true;
					delete child;
					break;
				}
			}
			if(duplicate_state == false) {
				pq.push(child);
				visited->push_back(child);
			}
		}
		return Graph_Search(pq, visited);
	}

}; //end of class Node


bool compare(Node * a, Node *b) {
	return (a->Get_F() > b->Get_F() );
}

//using pQueue = priority_queue< Node *, vector<Node *>, less<Node *> >;

int main(int argc, char * argv[]){
	if(argc != 2) {
		printf("Input: ./excutable_file input_file \n");
	}

    priority_queue< Node *, vector<Node *>, decltype(&compare) > Q(&compare);
	vector<Node*> *S = new vector<Node *>();//explored set for tracking
	
	freopen ("output.txt","w",stdout);//redirect the output to the output.txt

	int* input = new int[16]();
	ifstream file(argv[1]);
	for(size_t i=0; i<4; i++) {
		for(size_t j=0; j<4; j++) {
			file >> input[i*4+j];
		}
	}
	file.close();

	Node * initial_node = new Node(0,0,0,NULL,input); // read from input
	delete[] input;
	
	//initial_node->printNode(); //check the initiation is right
	Q.push(initial_node);
	S->push_back(initial_node);
	bool success = false;
	if( initial_node -> Graph_Search(Q, S)) {
		success = true;
	}
	
	//free Q and S
	for(size_t i=0; i<S->size();i++) {
			delete S->at(i);
	}
	S->clear();
	while( !Q.empty() ) {
			Q.pop();
	}
	
	fclose (stdout);	
	if(success) {
		return 1;
	} else {
		return 0;
	}
}




void Node::Print_State(){
		if(state != NULL) {
			for(size_t i=0; i<=15; i++) {
				cout<<right<<setw(7)<<state[i];
				if((i+1)%4 == 0) {
					cout<<" \n";
				}
			}
		} else {
			cout<<"state is empty \n"<<endl;
		}
		
	}

	void Node::printNode() {
		cout<<"g = "<<g<<"; "<<"h = "<<h<<"; "<<"f = "<<f<<"\n";
		
		Node * temp = parent;
		cout<<"parent's state = \n";
		if(temp != NULL) {
			temp->Print_State();
		} else{
			cout<<"No parent"<<endl;
		}

		cout<<"own's state = \n";
		Print_State();
		cout<<endl;
	}

	void Node::printSuccess(Node * current) {
		vector<Node *> temp;
		while( current != NULL) {
			temp.push_back(current);
			current = current->parent;
		}
		for(int i= (int)temp.size()-1; i>=0; i--) {
			if(i == (int)temp.size()-1) {
				cout<<"Beginning state: "<<endl;
			} else {
				cout<<"Switched "<<temp[i]->swith_move.first<<" & "<<temp[i]->swith_move.second<<endl;
			}
			temp[i]->Print_State();
			cout<<endl;
		}
	}










