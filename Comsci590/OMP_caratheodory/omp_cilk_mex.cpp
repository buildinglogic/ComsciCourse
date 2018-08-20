/* omp_cilk_mex.cpp    -*- C++ -*-
 *
 * ************************************************************
 *
 * MEX-interface function wrapper for externally compiled function
 *
 *
 * MATLAB SYNTAX
 *
 *      output_S = omp_cilk_mex(X, H, D, S, L, atom_size, N, num_dict, M3);  
 *      
 * NOTE
 *
 *
 * ************************************************************ */


#include <iostream>
#include "mex.h"
#include "matrix.h"
//#include <cstring>           // only used for the 'memcpy' operation



/***************************************************
 *  external function definition
 *
 */
extern
void ParaLoop(double*, double*, double*, double*, int, int, int, int, int);
int set_worker(const char*);

/* macros for input and output MEX arguments */
  #define X_IN   prhs[0]
  #define H_IN   prhs[1]
  #define D_IN   prhs[2]
  #define S_IN   prhs[3]
  #define SPARCITY_IN prhs[4]
  #define ATOM_SIZE_IN prhs[5]
  #define NUM_DATA_IN  prhs[6]
  #define NUM_DICT_IN  prhs[7]
  #define NUM_FRAME_IN prhs[8]
  #define N_WORKER    prhs[9]
  #define MATRIX_OUT  plhs[0]

/***************************************************
 * MEX function
 *
 */
void mexFunction( int nlhs,       mxArray* plhs[],
                  int nrhs, const mxArray* prhs[] ) {
  /* Input output validation */
  if(nrhs != 10)
    mexErrMsgTxt("Must have 10 input arguments."); 
  if(nlhs != 1)
    mexErrMsgTxt("Should be only 1 output arguments.");
    
  /* input vectors */
  double *X = mxGetPr( X_IN );
  double *H = mxGetPr( H_IN );
  double *D = mxGetPr( D_IN );
  double *S = mxGetPr( S_IN );
  /* input integers */
  double sparcity = mxGetScalar(SPARCITY_IN);
  double atom_size = mxGetScalar(ATOM_SIZE_IN);
  double num_data = mxGetScalar(NUM_DATA_IN);
  double num_dict = mxGetScalar(NUM_DICT_IN);
  double num_frame = mxGetScalar(NUM_FRAME_IN);
  
  /*set workers*/
  char buffer[10];
  size_t N_worker;
  N_worker = mxGetScalar(N_WORKER);
  sprintf(buffer, "%d", (int)N_worker);

  /*verify the workers #*/
  //int actual_worker = __cilkrts_get_nworkers();
  //std::cout<<"workers: "<<actual_worker<<std::endl;

  /* create output array */
  //MATRIX_OUT = mxDuplicateArray(S_IN);
  //double * output_matrix = mxGetPr( MATRIX_OUT );
    /* Find the dimensions of input matrices S */
  //size_t S_M = mxGetM(S_IN);
  //size_t S_N = mxGetN(S_IN);
  plhs[0] = mxCreateDoubleMatrix(2048, (size_t)num_data, mxREAL);
  double * output_matrix = mxGetPr( plhs[0] );

  /* ***NOTE*** This is only done because the sample_qsort function
                sorts the input *in place*. In MATLAB, functions are
                supposed to leave input unchanged. A function to be
                used via MEX should always produce the output in a new
                array (as with the matrix multiplication example) */
  //std::memcpy( S, X, sizeof(double) * m * n );
  
  /* run the sample_qsort external function */
  //std::cout<<"before ParaLoop\n";
  set_worker( buffer  );
  ParaLoop( X, H, D, output_matrix, sparcity, atom_size, num_data, num_dict, num_frame );
  //std::cout<<"after ParaLoop\n";

  return;
  
}  // end mexFunction
