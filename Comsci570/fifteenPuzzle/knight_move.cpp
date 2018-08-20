#include <stdio.h> //printf
#include <queue> // priority_queue
#include <vector> // vector S
#include <fstream> //ifstream
//#include <cmath> //std::abs
#include <cstdlib> //abs
#include <iostream> //cout

using namespace std;

//class Comparator;
class Node;

bool compare(Node *, Node*);

class Node {

		int g;
		int h;
		int f;
		Node * parent;
		int* state; // array of numbers: 1,2,3,4,5,6,0,8,9,10,11,12,13,14,15,7 (example on HW1)
	
	public:
	//constructor
	Node(int a, int b, int c, Node * temp, int * rhs): g(a),h(b),f(c),parent(temp){
		h=Calculate_H(rhs);
		f=g+h;
		int * new_state = new int[16];
		for(size_t i=0; i<=15; i++) {
			new_state[i]=rhs[i];
		}
		state = new_state;
	}

	Node(const Node & rhs): g(rhs.g),h(rhs.h) {
			f=g+h;
			parent=rhs.parent;
			int* new_state = new int[16];
			for(size_t i=0; i<=15; i++) {
				new_state[i]=rhs.state[i];
			}
			state = new_state;
	}

	Node & operator=(const Node & rhs) {
		if(this != &rhs) {
			int* new_state = new int[16];
			for(size_t i=0; i<=15; i++) {
				new_state[i]=rhs.state[i];
			}
			delete[] state;
			state = new_state;
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
	
	int Get_F() const {
		return f;
	}

	bool Goal_Test(Node * current)
	{
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

	int Calculate_H(int * s){ // Manhattan distance
		int distance = 0;
		for (int i=0; i<=15; i++) {
			if( s[i] != 0){
				distance += abs( (s[i]-1) / 4 - i/4)+ abs( (s[i]-1) % 4 - i%4);
			} else {
				distance += 3-i/4 + 3-i%4;
			}
			
		}
		return distance;
	}

	void Switch_Position_State(int i, int j){
		int temp = (this->state)[i];
		(this->state)[i]=(this->state)[j];
		(this->state)[j]=temp;
	}

	Node * Child_Node_Generate(Node * parent, int to_space, int from){
		Node * newNode = new Node(*parent);
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
			cout<<"Success with steps: "<<current->g<<endl;; //out current
			current->Print_State();
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
			if( !visited->empty() ) {
				for(size_t j=0; j<visited->size(); j++) {
					if ( !States_Equal(child, visited->at(j)) ) {
						pq.push(child);
						visited->push_back(child);
					}
				}
			} else {
				visited->push_back(child);
			}
			
			
		}
		Graph_Search(pq, visited);
	}

}; //end of class Node
/*
class Comparator {
public:
    bool operator()( const Node * a, const Node * b ){ 
        return (a->Get_F() < b->Get_F() );
    }
};
*/
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
	
	int* input = new int[16];
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
	if( initial_node -> Graph_Search(Q, S)) {
		//free Q;
		for(size_t i=0; i<S->size();i++) {
			delete S->at(i);
		}
		S->clear();
		while( !Q.empty()) {
			Q.pop();
		}
		//free S;
		return 1;
	}
	
	//free Q;
		for(size_t i=0; i<S->size();i++) {
			delete S->at(i);
		}
		S->clear();
		while( !Q.empty()) {
			Q.pop();
		}
		//free S;
	return 0;
}




void Node::Print_State(){
		if(state != NULL) {
			for(size_t i=0; i<=15; i++) {
				printf("%d ", state[i]);
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











