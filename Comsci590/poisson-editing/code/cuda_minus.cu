/* ****************************************
*
* CUDA Kernel: matrix minus 
*
*/


/* ****************************************
 *
 * sub2ind - Column-major indexing of 2D arrays
 *
 */
template <typename T>
__device__ __forceinline__ T sub2ind( T i, T j, T height ) {

  return (i + height*j);

}  // end function 'sub2ind'


/* ****************************************
 *
 *  core kernel
 *
 */

__global__ void apply_filter(double * Iedit, 
	const double * ILaplace,
	const double * B,
	const int * Mask,
	const int m, 
	const int n,
    const int p){

/* thread indices */
        const int j = blockIdx.y*blockDim.y+threadIdx.y;
        const int i = blockIdx.x*blockDim.x+threadIdx.x;
        
/* matrix calculation */
	if ((i >= m) || (j >= n*p) || (Mask[sub2ind(i,j,m)]==0) ){
                return;
        }
        
        Iedit[sub2ind(i,j,m)] = (ILaplace[sub2ind(i,j,m)]-B[sub2ind(i,j,m)])/4.0;
        return ;
}

