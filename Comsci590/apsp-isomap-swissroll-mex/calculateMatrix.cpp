// C++

/********implementaton of algorithm with c++ ****************
An implementation of algorithm (ii)modified dynamic programming, with pseudo-matrix-multiplicaiton; (iii)Floyd-Warshall algorithm, and corresponding block-wise versions. All implementation using Cilk parallelization.
***********************************************************/

/******** input requirement ******************
input matrix double ** W, assert(row == column)
output matrix double ** D
**********************************************/

#include <cilk/cilk.h> //******for cilk Plus *****/
#include <cilk/cilk_api.h>  //for cilkview
#include <iostream>
#include <algorithm> //std::min
#include <stdlib.h>  //exit(EXIT_SUCCESS)
#include <limits>  //infinity
#include <iterator>
#include <functional>
#include <cassert>
#include <string>
#include <pthread.h>


void special_matrix_multiply_C(double **matrix, int row, int column);
void special_matrix_multiply_cilk(double **matrix, int row, int column);
void show_matrix_matrix(double **matrix, int row, int column);
void show_matrix(double* matrix, int row, int column);

void block_MM ( double *C, double * A, double * B, int row_A, int column_A, int row_B, int column_B, int gran);
double ** block_partition (double * A, int row1, int row2, int column1, int column2);
void add_MM( double *C, double * A, double * B, int n);
double * put_together (double * A11, double * A12, double * A21, double * A22, int row1, int row2, int column1,int column2);
double * modified_MM( double * A, double * B, int row_A, int column_A, int row_B, int column_B);

int  set_worker(char* N);

//implementation of matrix multiplication with C
void apsp_MM_C(double *D, double * W, int row, int column) 
{
  assert(row == column);
  std::cout<<"\n ######running matrix multiplication APSP C ....."<<std::endl;
  
  double **tempMatrix =  new double *[row];//initiate a temporary matrix to store D1, D2 ..  
  for(size_t i=0; i<row; i++) {
    tempMatrix[i] = new double[column];
    std::copy( W+column*i, W+column*(i+1), tempMatrix[i]);
  }    
 
  size_t m =1;
  while( m<row-1 ) {
    special_matrix_multiply_C(tempMatrix, row, column);
    m = 2*m;
  }

  for(size_t i=0; i<row; i++) {
    for(size_t j=0; j<column; j++) {
      *(D + column*i + j) = *(tempMatrix[i]+j);// D <- tempMatrix
    }
  }
 
  for(size_t i=0; i<row; i++) {
    delete[] tempMatrix[i];
  }
  delete[] tempMatrix;
}//end of apsd_MM

//implementation of matrix multiplication with C including cilk_for
void apsp_MM_cilk(double *D, double * W, int row, int column) 
{
  assert(row == column);
  std::cout<<"\n ######running matrix multiplication APSP cilk ....."<<std::endl;
  
  double **tempMatrix =  new double *[row];//initiate a temporary matrix to store D1, D2 ..  
  for(size_t i=0; i<row; i++) {
    tempMatrix[i] = new double[column];
    std::copy( W+column*i, W+column*(i+1), tempMatrix[i]);
  }    
 
  size_t m =1;
  while( m<row-1 ) {
    special_matrix_multiply_cilk(tempMatrix, row, column);
    m = 2*m;
  }

  for(size_t i=0; i<row; i++) {
    for(size_t j=0; j<column; j++) {
      *(D + column*i + j) = *(tempMatrix[i]+j);// D <- tempMatrix
    }
  }
 
  for(size_t i=0; i<row; i++) {
    delete[] tempMatrix[i];
  }
  delete[] tempMatrix;
}//end of apsd_MM

//implementatoin of algoritm (iii) Floyd-Warshall
void apsp_floyd_C(double * D, double * W, int row, int column) 
{
  assert( row == column);
  std::cout<<"\n ######running  APSP Floyd C ....."<<std::endl;

  double INF = std::numeric_limits<double>::infinity();
  double **tempMatrix1 = new double *[row];
  double **tempMatrix2 = new double *[row];
  for(size_t i=0; i<row; i++) {
    tempMatrix1[i] = new double[column];
    tempMatrix2[i] = new double[column];
    std::copy( W+column*i, W+column*(i+1), tempMatrix1[i]);  
  }
 
  for(size_t k=0; k<row; k++) {
    for(size_t i=0; i<row; i++) {
      for(size_t j=0; j<column; j++) {
	*(tempMatrix2[i]+j)=INF;
	*(tempMatrix2[i]+j) = std::min( *(tempMatrix1[i]+j),(*(tempMatrix1[i]+k)+*(tempMatrix1[k]+j)) );
      }
    }
    for(size_t i=0; i<row; i++) {
      for(size_t j=0; j<column; j++) {
	*(tempMatrix1[i]+j) = *(tempMatrix2[i]+j);
      }
    }
  }
  
  for(size_t i=0; i<row; i++) {
    for(size_t j=0; j<column; j++) {
      *(D + column*i + j) = *(tempMatrix2[i]+j);// D <- tempMatrix
   }
  }
  
  for(size_t i=0; i<row; i++) {
    delete[] tempMatrix1[i];
    delete[] tempMatrix2[i];
  }
  delete[] tempMatrix1;
  delete[] tempMatrix2;
}//end of apsd_floyd function

//implementatoin of algoritm (iii) Floyd-Warshall
void apsp_floyd_cilk(double * D, double * W, int row, int column) 
{
  assert( row == column);
  std::cout<<"\n ######running  APSP Floyd cilk ....."<<std::endl;

  double INF = std::numeric_limits<double>::infinity();
  double **tempMatrix1 = new double *[row];
  double **tempMatrix2 = new double *[row];
  for(size_t i=0; i<row; i++) {
    tempMatrix1[i] = new double[column];
    tempMatrix2[i] = new double[column];
    std::copy( W+column*i, W+column*(i+1), tempMatrix1[i]);  
  }
 
  for(size_t k=0; k<row; k++) {
    cilk_for(size_t i=0; i<row; i++) {
      for(size_t j=0; j<column; j++) {
  *(tempMatrix2[i]+j)=INF;
  *(tempMatrix2[i]+j) = std::min( *(tempMatrix1[i]+j),(*(tempMatrix1[i]+k)+*(tempMatrix1[k]+j)) );
      }
    }
    for(size_t i=0; i<row; i++) {
      for(size_t j=0; j<column; j++) {
  *(tempMatrix1[i]+j) = *(tempMatrix2[i]+j);
      }
    }
  }
  
  for(size_t i=0; i<row; i++) {
    for(size_t j=0; j<column; j++) {
      *(D + column*i + j) = *(tempMatrix2[i]+j);// D <- tempMatrix
   }
  }
  
  for(size_t i=0; i<row; i++) {
    delete[] tempMatrix1[i];
    delete[] tempMatrix2[i];
  }
  delete[] tempMatrix1;
  delete[] tempMatrix2;
}//end of apsd_floyd function

//implementation of block-wise matrix-multiplication
void apsp_block_MM(double *W, double * D, int row, int column,int gran)
{
    assert(row == column);
    std::cout<<"\n ######running  Block MM ....."<<std::endl;

    double * d = new double[row*column];
    for (size_t i=0; i<row*column; i++){
      d[i] = D[i];
    }

    //cilk::cilkview cv;
    //cv.start();
    for (size_t k=1; k<row-1; k++){
      block_MM(W,d,d,row,column,row,column,gran);
      for (size_t i=0; i<row*column; i++){
        d[i] = W[i];
      }
      k = 2*k;
    }
    //cv.stop();
    //cv.dump("apsp_block_MM");  
}


void block_MM ( double *C, double * A, double * B, int x_A, int y_A, int x_B, int y_B, int gran){
    assert(y_A=x_B);
    if (x_A<=gran && x_B<=gran && y_B<=gran){
       double *tmp = modified_MM(A,B,x_A,y_A,x_B,y_B);
        for (int i=0;i<x_A*y_B;i++){
            C[i] = tmp[i];
        }
        return;
    }
    int ma1=x_A/2;
    int ma2=x_A-ma1;
    int na1=y_A/2;
    int na2=y_A-na1;
    int mb1=na1;
    int mb2=na2;
    int nb1=y_B/2;
    int nb2=y_B-nb1;
    double ** sub_A = block_partition(A,ma1,ma2,na1,na2);
    double ** sub_B = block_partition(B,mb1,mb2,nb1,nb2);


    double *T1 = new double[ma1*nb1];
    double *T2 = new double[ma1*nb1];
    double *T3 = new double[ma1*nb2];
    double *T4 = new double[ma1*nb2];
    double *T5 = new double[ma2*nb1];
    double *T6 = new double[ma2*nb1];
    double *T7 = new double[ma2*nb2];
    double *T8 = new double[ma2*nb2];
    cilk_spawn  block_MM(T1,sub_A[0],sub_B[0],ma1,na1,mb1,nb1,gran);
    cilk_spawn  block_MM(T2,sub_A[1],sub_B[2],ma1,na2,mb2,nb1,gran);
    cilk_spawn  block_MM(T3,sub_A[0],sub_B[1],ma1,na1,mb1,nb2,gran);
    cilk_spawn  block_MM(T4,sub_A[1],sub_B[3],ma1,na2,mb2,nb2,gran);
    cilk_spawn  block_MM(T5,sub_A[2],sub_B[0],ma2,na1,mb1,nb1,gran);
    cilk_spawn  block_MM(T6,sub_A[3],sub_B[2],ma2,na2,mb2,nb1,gran);
    cilk_spawn  block_MM(T7,sub_A[2],sub_B[1],ma2,na1,mb1,nb2,gran);
    block_MM(T8,sub_A[3],sub_B[3],ma2,na2,mb2,nb2,gran);
    
    cilk_sync;
    
    double * C11 = new double[ma1*nb1];
    double * C12 = new double[ma1*nb2];
    double * C21 = new double[ma2*nb1];
    double * C22 = new double[ma2*nb2];
    cilk_spawn  add_MM(C11,T1, T2, ma1*nb1);
    cilk_spawn  add_MM(C12,T3, T4, ma1*nb2);
    cilk_spawn  add_MM(C21,T5, T6, ma2*nb1);
    add_MM(C22,T7, T8, ma2*nb2);
    
    cilk_sync;
    
    double * ans = put_together(C11, C12, C21, C22, ma1, ma2, nb1, nb2);
    
    for (int i=0;i<x_A*y_B;i++){
        C[i] = ans[i];
    }
    return;
    
}

void add_MM( double *C,double * A, double * B, int n){

    for (int i=0;i<n;i++){ // can be cilk_for;
        C[i]=std::min(A[i],B[i]);
    }

}

double ** block_partition (double * A, int m1, int m2, int n1, int n2){ //can be further parallel
    int n=n1+n2;
    //int m=m1+m2;
    double * A11=new double[m1*n1];
    for (int i=0;i<m1;i++){
        for (int j=0;j<n1;j++){
            A11[i*n1+j]=A[i*n+j];
        }
    }
    double * A12=new double[m1*n2];
    for (int i=0;i<m1;i++){
        for (int j=0;j<n2;j++){
            A12[i*n2+j]=A[i*n+j+n1];
        }
    }
    double * A21=new double[m2*n1];
    for (int i=0;i<m2;i++){
        for (int j=0;j<n1;j++){
            A21[i*n1+j]=A[(i+m1)*n+j];
        }
    }
    double * A22=new double[m2*n2];
    for (int i=0;i<m2;i++){
        for (int j=0;j<n2;j++){
            A22[i*n2+j]=A[(i+m1)*n+j+n1];
        }
    }
    //double * partition[] = {A11,A12,A21,A22};
    double ** ans = new double*[4];
    ans[0] = A11;
    ans[1] = A12;
    ans[2] = A21;
    ans[3] = A22;
    return ans;
}

double * put_together (double * A11, double * A12, double * A21, double * A22, int m1, int m2, int n1, int n2){ // can be further parallel;
    int n=n1+n2;
    int m=m1+m2;
    double * A=new double[m*n];
    for (int i=0;i<m1;i++){
        for (int j=0;j<n1;j++){
            A[i*n+j]=A11[i*n1+j];
        }
    }
    for (int i=0;i<m1;i++){
        for (int j=0;j<n2;j++){
            A[i*n+j+n1]=A12[i*n2+j];
        }
    }
    for (int i=0;i<m2;i++){
        for (int j=0;j<n1;j++){
            A[(i+m1)*n+j]=A21[i*n1+j];
        }
    }
    for (int i=0;i<m2;i++){
        for (int j=0;j<n2;j++){
            A[(i+m1)*n+j+n1]=A22[i*n2+j];
        }
    }
    return A;
}

//helper method of special_matrix_multiplication with returning a new array
double * modified_MM( double * A, double * B, int row_A, int column_A, int row_B, int column_B)
{
    assert( column_A == row_B );
    double * C = new double[row_A*column_B];
      for ( size_t i=0; i<row_A; i++ ){
        for (size_t j=0; j<column_B; j++){
          double * tmp = new double[column_A];
          for ( size_t k=0; k<column_A;  k++){
            tmp[k] = *(A+i*column_A+k) + *(B+k*column_B+j);
          }
          C[i*column_B+j] = *std::min_element(tmp,tmp+column_A);
        }
      }        
    return C;
}

//helper method of special_matrix_multiply with modifying the input matrix
void special_matrix_multiply_C(double ** matrix, int row, int column) 
{
  assert(row == column);
  double INF = std::numeric_limits<double>::infinity(); 
  double **tempMatrix = new double *[row];
  for(size_t i=0; i<row; i++) {
    tempMatrix[i] = new double[column];
  }

  for(size_t i=0; i<row; i++) {
    for(size_t j=0; j<column; j++) {
      *(tempMatrix[i] +j)=INF;
      for(size_t k=0; k<row; k++) {
  *(tempMatrix[i]+j) = std::min( *(tempMatrix[i]+j), (*(matrix[i]+k) + *(matrix[k]+j)) );
      }
    }
  }
  
  for(size_t i=0; i<row; i++) {
    for(size_t j=0; j<column; j++) {
      *(matrix[i]+j) = *(tempMatrix[i]+j);// D <- tempMatrix
    }
  }
 
  for(size_t i=0; i<row; i++) {
    delete[] tempMatrix[i];
  }
  delete[] tempMatrix;
}//end of special_matrix_multiply

//helper method of special_matrix_multiply
void special_matrix_multiply_cilk(double ** matrix, int row, int column) 
{
  assert(row == column);
  double INF = std::numeric_limits<double>::infinity(); 
  double **tempMatrix = new double *[row];
  for(size_t i=0; i<row; i++) {
    tempMatrix[i] = new double[column];
  }

  cilk_for(size_t i=0; i<row; i++) {
    for(size_t j=0; j<column; j++) {
      *(tempMatrix[i] +j)=INF;
      for(size_t k=0; k<row; k++) {
  *(tempMatrix[i]+j) = std::min( *(tempMatrix[i]+j), (*(matrix[i]+k) + *(matrix[k]+j)) );
      }
    }
  }
  
  for(size_t i=0; i<row; i++) {
    for(size_t j=0; j<column; j++) {
      *(matrix[i]+j) = *(tempMatrix[i]+j);// D <- tempMatrix
    }
  }
 
  for(size_t i=0; i<row; i++) {
    delete[] tempMatrix[i];
  }
  delete[] tempMatrix;
}//end of special_matrix_multiply

//function help to debug
void show_matrix_matrix(double ** matrix, int row, int column) 
{
  for(size_t i=0; i<row; i++) {
    for(size_t j=0; j<column; j++) {
      std::cout<<*(matrix[i] +j)<<" ";
    }
    std::cout<<std::endl;
  }
  std::cout<<std::endl;
}

int  set_worker(const char* N){
  int num = atoi(N);
  std::cout<<"N workers : "<< num <<std::endl;
   if (0!=__cilkrts_set_param("nworkers",N)){
      std::cout<<"worker setting failed!\n";
      return 1;
    }
}

void compareMatrix(double * A, double * B, int m, int n){
    for (int i = 0; i < m*n; ++i) {
        if (A[i] != B[i]) {
            std::cout << "Distance differ at location i=" << i << " A[i] = "
                      << A[i] << " B[i] = " << B[i] << "\n";
                      return;
        }
    }
  std::cout << "Result check passed!\n";
}

void show_matrix(double * matrix, int m, int n){
    for (int i=0;i<m;i++){
        for (int j=0; j<n; j++){
         std::cout<<*(matrix+i*n+j)<<" ";    
        }
        std::cout<<"\n";
    }
}
/*  Test apsp_MM function */

int main(int argc, char* argv[])
{
    int m = 4;
    int n = m;
    double inf = std::numeric_limits<double>::infinity();

    double a[16] = {0,inf,1,inf,inf,0,5,2,1,5,0,2,inf,2,2,0};
    double *matrix_in = a;
    double *matrix_out = new double[m*n];
    double ans[16] = {0,5,1,3,5,0,4,2,1,4,0,2,3,2,2,0};
 
    std::cout<<"\n original matrix:\n";
    show_matrix(matrix_in, m, n);
    std::cout<<" correct answer:\n";
    show_matrix(ans, m, n);
    
    apsp_floyd_C(matrix_out, matrix_in, m, n);
    std::cout<<"\n asps floyd C:\n";
    show_matrix(matrix_out, m, n);
    compareMatrix(ans, matrix_out,m,n);

    apsp_floyd_cilk(matrix_out, matrix_in, m, n);
    std::cout<<"\n asps floyd cilk:\n";
    show_matrix(matrix_out, m, n);
    compareMatrix(ans, matrix_out,m,n);
    
    apsp_MM_C(matrix_out,matrix_in,m,n);
    std::cout<<"\n asps matrix C:\n";
    show_matrix(matrix_out, m, n);
    compareMatrix(ans, matrix_out,m,n);

    apsp_MM_cilk(matrix_out,matrix_in,m,n);
    std::cout<<"\n asps matrix cilk:\n";
    show_matrix(matrix_out, m, n);
    compareMatrix(ans, matrix_out,m,n);

  
    apsp_block_MM(matrix_out,matrix_in,m,n,1);
    std::cout<<"\n block_MM matrix:\n";
    show_matrix(matrix_out,m,n);
    compareMatrix(ans, matrix_out,m,n);
  
    delete[] matrix_out;
}




											     

