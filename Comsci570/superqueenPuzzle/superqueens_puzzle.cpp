
/**************superqueens puzzle ***************
Algorithm:
Use the A* from fifteen puzzle 
Difference:
    fill the "1" row by row 0 -> n
    add a global Node * lowest_cost_sofar to record the best solution found so far
    In each "actions", compare their cost increment, and only expand those with minimum increment
    when to end: 
        If cost == 0 at the end, return the result and exit program
        If cost > 0; then continue till no element in priority_queue, and return the loowest_cost_sofar
**********************************************/

#include <stdio.h> //printf
#include <queue> // priority_queue
#include <vector> // vector S
#include <fstream> //ifstream
//#include <cmath> //std::abs
#include <cstdlib> //abs, atoi
#include <iostream> //cout
#include <iomanip> //std::stew
#include <utility>      // std::pair, std::make_pair

using namespace std;
    
class Node;
Node * lowest_cost_sofar;

bool compare(Node *, Node*);

class Node {

        int cost; 
        int row; 
        int column;
        int N; //n
        Node * parent;
        int* state; //store matrix in a 1-dimension array
    
    public:
    //constructor
    Node(int a, int b, int c, int d, Node * temp, int * rhs): cost(a),row(b),column(c),N(d), parent(temp), state(NULL){
        //cost = Calculate_H(rhs);
        int * new_state = new int[N*N]();
        for(size_t i=0; i<N*N; i++) {
            new_state[i]=rhs[i];
        }
        state = new_state;
    }

    Node(const Node & rhs): cost(rhs.cost),row(rhs.row),column(rhs.column),N(rhs.N),parent(rhs.parent) {
            int* new_state = new int[N*N]();
            for(size_t i=0; i< N*N; i++) {
                new_state[i]=(rhs.state)[i];
            }
            state = new_state;
    }

    Node & operator=(const Node & rhs) {
        if(this != &rhs) {
            int* new_state = new int[rhs.N * rhs.N]();
            for(size_t i=0; i<rhs.N * rhs.N; i++) {
                new_state[i]=(rhs.state)[i];
            }
            delete[] state;
            state = new_state;
            cost = rhs.cost;
            row = rhs.row;
            column = rhs.column;
            N = rhs.N;
            parent = rhs.parent;
        }
        return *this;
    }

    ~Node(){
        delete[] state;
    }


    void Print_State();

    void printNode();
    
    void printSuccess(Node *);

    int Get_Cost() const {
        return cost;
    }

    bool IsKnightmove(int A_row,int A_col,int B_row, int B_col) {
        if( abs( (B_row - A_row) * (B_col - A_col) ) ==2 ) {
            return 1;
        }
        return 0;
    }

    bool IsDiagonal( int A_row,int A_col,int B_row, int B_col ) {
        if( abs(B_row - A_row) == abs(B_col - A_col) ) {
            return 1;
        }
        return 0;
    }

    int Calculate_H(int * s, int parent_cost, int add_row, int add_col){ 
    //the new queen put at (add_row, add_col)
        int increment = 0;
        for(size_t i=0; i<add_row; i++) {
            for(size_t j=0; j<N; j++) {
                if(s[i*N+j] == 1) {
                    if( j == add_col ) {
                        increment++;
                    } else {
                        if ( IsKnightmove(i,j,add_row,add_col) ) {
                            increment++;
                        } else {
                            if( IsDiagonal(i,j,add_row, add_col) ) {
                                increment ++;
                            }
                        }
                    }
                }
            }
        }
        int child_cost = parent_cost + increment;
        return child_cost;
    }

    Node * Child_Node_Generate(Node * parent, int col){
        Node * newNode = new Node(*parent);
        newNode->row += 1;
        newNode->column = col;
        (newNode->state)[ (newNode->row)*N + col ]=1;
        newNode->cost = Calculate_H(newNode->state, parent->cost, newNode->row, col);
        //cout<<"N = "<<newNode->N<<" child cost = "<<newNode->cost<<endl;
        newNode->parent = parent;
        return newNode;
    }

    // bool States_Equal(Node * a, Node * b) {
    //     for(size_t i=0; i<=N*N; i++) {
    //         if( (a->state)[i] - (b->state)[i] != 0) {
    //             return 0;
    //         }
    //     }
    //     return 1;
    // }

    int Graph_Search(priority_queue< Node *, vector<Node *>, decltype(&compare) > & pq, vector<Node *> * visited){
        if( pq.empty() ) {
            std::cout<<"Search till the end of Queue: "<<endl;
            printSuccess(lowest_cost_sofar);
            return 1;
        }
        Node * current = pq.top();
        pq.pop();

        if(current->row < N-1) {
            vector<Node *> child_candidate;
            for (size_t i=0; i<N; i++) {
                Node * child = Child_Node_Generate(current, i ); 
                child_candidate.push_back(child);
            }
            
            int min_cost = (child_candidate[0])->cost;
            for(size_t i=0; i<child_candidate.size(); i++) {
                if(child_candidate[i]->cost < min_cost) {
                    min_cost = child_candidate[i]->cost;
                }
            }

            for(size_t i=0; i<N; i++) {
                if(child_candidate[i]->cost == min_cost) {
                    pq.push(child_candidate[i]);
                    visited->push_back(child_candidate[i]);

                    if(child_candidate[i]->row == N-1 ) {
                        if (child_candidate[i]->cost == 0) {
                            cout<<"find the first path with cost=0: "<<endl;
                            printSuccess(child_candidate[i]);
                            return 1;
                        } else {
                            if(lowest_cost_sofar == NULL) {
                                lowest_cost_sofar = child_candidate[i];
                            }
                            if (lowest_cost_sofar != NULL && (child_candidate[i]->cost < lowest_cost_sofar->cost)) {
                                lowest_cost_sofar = child_candidate[i];
                            }
                            //lowest_cost_sofar->Print_State();
                        }
                    }

                } else{
                    delete child_candidate[i];
                }
            }
        }

        return Graph_Search(pq, visited);
    }

}; //end of class Node


bool compare(Node * a, Node *b) {
    return (a->Get_Cost() > b->Get_Cost() );
}

//using pQueue = priority_queue< Node *, vector<Node *>, less<Node *> >;

int main(int argc, char * argv[]){
    if(argc != 2) {
        printf("Input: ./excutable_file superqueens_size \n");
    }
    int sq_size = atoi(argv[1]); 
    priority_queue< Node *, vector<Node *>, decltype(&compare) > Q(&compare);
    vector<Node*> *S = new vector<Node *>();//explored set for tracking

    freopen ("output.txt","w",stdout);//redirect the output to the output.txt
 
    //Node initiation
    int board_size = sq_size*sq_size;
    for(size_t i=0; i<sq_size; i++) {
        int *init_state = new int[board_size]();
        init_state[i] = 1;
        Node * row0_node = new Node(0,0,i,sq_size,NULL,init_state);
        delete[] init_state;
        //row0_node->printNode();
        Q.push(row0_node);
        S->push_back(row0_node);
    }
    
    bool success = false;
    if( Q.top() -> Graph_Search(Q, S) ) {
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
            for(size_t i=0; i< N*N; i++) {
                cout<<right<<setw(2)<<state[i];
                if((i+1)%N == 0) {
                    cout<<" \n";
                }
            }
        } else {
            cout<<"state is empty \n"<<endl;
        }
        
    }

    void Node::printNode() {
        cout<<"cost = "<<cost<<"; "<<"row = "<<row<<"; "<<"column = "<<column<<"; "<<"N = "<<N<<"\n";
        
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
        Node * cur = current;
        vector<Node *> temp;
        while( current != NULL) {
            temp.push_back(current);
            current = current->parent;
        }
        cout<<"Filling order (row, column): ";
        for(int i= (int)temp.size()-1; i>=0; i--) {
            cout<<"("<<temp[i]->row<<", "<<temp[i]->column<<") ";
        }
        cout<<endl;
        cout<<"cost = "<<cur->cost<<endl;
        cur->Print_State();
    }










