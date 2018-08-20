/* apsp_cilk_mex.cpp    -*- C++ -*-
 *
 * ************************************************************
 *
 * MEX-interface function wrapper for externally compiled function
 *
 *
 * MATLAB SYNTAX
 *
 *      S = apsd_MM( int32(X) );
 *      S = apsd_floyd(     );
 * NOTE
 *
 *      The input must be a square matrix of doubles.
 *
 * ************************************************************ */



#include "mex.h"
#include "matrix.h"
//#include <cstring>           // only used for the 'memcpy' operation



/***************************************************
 *  external function definition
 *
 */
extern
void apsp_MM_C(double*, double *, int, int);
void apsp_MM_cilk(double*, double *, int, int);

void apsp_floyd_C(double *, double*, int, int);
void apsp_floyd_cilk(double *, double*, int, int);

void apsp_block_MM(double *, double *, int, int, int);
int set_worker(const char*);

/* macros for input and output MEX arguments */
  #define MATRIX_IN   prhs[0]
  #define METHOD_IN   prhs[1]
  #define GRAN_IN     prhs[2]
  #define N_WORKER    prhs[3]
  #define MATRIX_OUT  plhs[0]

/***************************************************
 * MEX function
 *
 */
void mexFunction( int nlhs,       mxArray* plhs[],
                  int nrhs, const mxArray* prhs[] ) {
  /* Input output validation */
  if(nrhs < 2)
    mexErrMsgTxt("Must have 2~4 input arguments (matrix and method, granularity, worker)."); 
  if(nlhs > 4)
    mexErrMsgTxt("Too many input arguments.");
    
  /* input vector */
  //const double* X = (double*) mxGetData( X_IN );
  double *input_matrix = mxGetPr( MATRIX_IN );  

  /* input vector length */
  const int m   = mxGetM( MATRIX_IN );
  const int n   = mxGetN( MATRIX_IN );

  /*which method chosen*/
  double method;
  if(mxIsComplex(METHOD_IN) || !mxIsDouble(METHOD_IN) ||
     mxGetNumberOfElements(METHOD_IN) != 1)
    mexErrMsgTxt("second argument method must be a double scalar.");
  else
    method = mxGetScalar(METHOD_IN);

  /*3rd or 4th input argument, granularity and workers*/
  double granularity = mxGetScalar(GRAN_IN);
  double N_worker = mxGetScalar(N_WORKER);
  
  char* ptr_worker = (char*)(&N_worker);

  /* create output array */
  //MATRIX_OUT = mxCreateDoubleMatrix(m, n, mxREAL);
  MATRIX_OUT = mxDuplicateArray(MATRIX_IN);
  double * output_matrix = mxGetPr( MATRIX_OUT );

  /* ***NOTE*** This is only done because the sample_qsort function
                sorts the input *in place*. In MATLAB, functions are
                supposed to leave input unchanged. A function to be
                used via MEX should always produce the output in a new
                array (as with the matrix multiplication example) */
  //std::memcpy( S, X, sizeof(double) * m * n );
  
  /* run the sample_qsort external function */
  double * ans;

  set_worker( ptr_worker );
  if ( method == 1)
    {
      apsp_MM_C( output_matrix,input_matrix, m, n );
    }else if ( method == 2)
      {
        apsp_MM_cilk( output_matrix, input_matrix, m, n);
      }else if (method == 3) 
        {
          apsp_floyd_C(output_matrix, input_matrix, m, n);
        }else if (method ==4)
          {
            apsp_floyd_cilk(output_matrix, input_matrix, m, n);
          }else if (method ==5)
            {
              apsp_block_MM(output_matrix, input_matrix, m, n, granularity);
            }else {
                mexErrMsgTxt("Unknown method (must be 1 to 5).");
              }
  
  
  //   show_matrix(ans,m,n);
  //    for (size_t i=0;i<m*n;i++){
  //      output_matrix[i] = ans[i];
  //    }

  return;
  
}  // end mexFunction
