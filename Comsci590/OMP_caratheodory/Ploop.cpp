#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_opadd.h>
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <functional>
#include <cassert>
#include <algorithm>
#include <limits>
#include <pthread.h>
#include <stdlib.h>  /*srand, rand*/
#include <time.h>

using namespace cv;
using namespace std;

void MatToPointer( cv::Mat_<double>& A, double * B, int col_B, int row_B);
void PointerToMat( cv::Mat_<double>& A, double * B, int col_begin, int col_num, int col_B);
void OMP(const cv::Mat_<double>& x, const cv::Mat_<double>& D, int L, cv::Mat_<double>& coeff);
void printFunct(double * temp, int row, int column);

int  set_worker(const char* N){
  int num = atoi(N);
  std::cout<<"set workers # as : "<< num <<std::endl;
   if ( 0 != __cilkrts_set_param("nworkers",N) ){
      std::cout<<"worker setting failed!\n";
      return 1;
    }
    int numWorkers = __cilkrts_get_nworkers();
    std::cout<<"actual wokers are: "<<numWorkers<<std::endl;
}

void ParaLoop(
        double * X, 
        double * H, 
        double * D, 
        double * S,
        int sparsity, 
        int atom_size, 
        int num_data,
        int num_dict,
        int num_frame)

{   
    //printFunct(X, 4, 1);
    //printFunct(H, 8, 1);
    //printFunct(D, 4, 6);
    //printFunct(S, 12, 1);
    Mat_<double> M_coef = Mat::zeros(num_frame*num_dict, num_data, CV_64FC1);
    Mat_<double> temp_HD = Mat::zeros(atom_size, num_frame*num_dict, CV_64FC1);
    Mat_<double> temp_coef = Mat::zeros(num_frame*num_dict, 1, CV_64FC1);
    Mat_<double> Dict = Mat::zeros(atom_size, num_dict, CV_64FC1);
    PointerToMat(Dict, D, 0, num_dict, num_dict);
 //   "this is Dict \n";
 //   cout<<Dict<<endl;
//     for(int i = 0; i < num_data; i++)
    cilk_for(int i = 0; i < num_data; i++)
    {
        Mat_<double> temp_x = Mat::zeros(atom_size, 1, CV_64FC1);
        Mat_<double> temp_H = Mat::zeros(num_frame*atom_size, 1, CV_64FC1);
        PointerToMat(temp_x,X,i,1,num_data);
        //cout<<"this is temp_x \n";
        //cout<<temp_x<<endl;
        PointerToMat(temp_H,H,i,1,num_data);
        //cout<<"this is temp_H \n";
        //cout<<temp_H<<endl;
        for(int j = 0; j < num_frame; j++){
            for(int k = 0; k < atom_size; k++){
                Mat temp_pro = temp_H.at<double>(j*atom_size+k,0)*Dict.row(k); 
                cv::Mat tmp = temp_HD(cv::Rect(j*num_dict,k,num_dict,1));
                temp_pro.copyTo(tmp);
            }
        }
        //cout<<"this is temp_HD\n";
        //cout<<temp_HD<<"\n";
        //cout<<"this is temp_x \n";
        //cout<<temp_x<<"\n";
        //Mat temp_y = temp_x;
        //Mat temp_hd_ = temp_HD;
        OMP(temp_x,temp_HD,sparsity,temp_coef);
        temp_coef.copyTo(M_coef.col(i));
        //cout<<"this is M_coef \n";
        //cout<<M_coef<<endl;
    }
    MatToPointer(M_coef,S,num_data,num_frame*num_dict);
}   

void MatToPointer(cv::Mat_<double>& A, double * B, int col_B, int row_B)
{
    for(int i = 0; i < row_B; i++){
        for(int j = 0; j < col_B; j++){
            B[i*col_B+j] = A(i,j);
        }
    }
        
}

void PointerToMat(cv::Mat_<double>& A, double * B, int col_begin, int col_num, int col_B)
{
    int M = A.rows;
    int N = A.cols;
    for(int i = 0; i < M; i++){
        for(int j = 0; j < col_num; j++){
            A(i,j) = B[i*col_B+j+col_begin];
        }
    }
}

/* Orthogonal matching pursuit
* x: input signal, N * 1 
* D: dictionary, N * M 
* L: number of non_zero elements in output
* coeff: coefficent of each atoms in dictionary, M * 1
*/
void OMP(const cv::Mat_<double>& x, const cv::Mat_<double>& D, int L, cv::Mat_<double>& coeff){
    int dim = x.rows;
    int atom_num = D.cols;
    coeff = Mat::zeros(atom_num, 1, CV_64FC1);
    Mat_<double> residual = x.clone();
    Mat_<double> selected_index(L, 1);
    Mat_<double> a;
    int count =0;
    for (int i = 0; i < L; i++,count++){
        //cout << "here ok 1" << endl;
        Mat_<double> dot_p = D.t() * residual; 
        Point max_index;
        minMaxLoc(abs(dot_p), NULL, NULL, NULL, &max_index);
        int max_row = max_index.y;
        selected_index(i) = max_row;
        Mat_<double> temp(dim, i + 1);
        for (int j = 0; j < i + 1; j++){
            D.col(selected_index(j)).copyTo(temp.col(j));
        }
        //Mat_<double> invert_temp; /*Solve Ax=b by inverting the matrix A*/
        //invert(temp, invert_temp, CV_SVD);        
        //a = invert_temp * x;     
        solve(temp, x, a, DECOMP_QR); /*should be faster than matrix inverting*/   
        residual = x - temp * a;
        double res = norm(residual,NORM_L2,noArray());
        if( res < 0.001 ) {
            break;
        }
        //cout<< "res: "<<res<<"\n";
    }

    for (int i = 0; i <= count; i++){
        coeff(selected_index(i)) = a(i);
    }
} 

void printFunct(double * temp, int row, int column) {
     for(int i=0; i<row; i++) {
         for(int j=0; j<column; j++) {
             std::cout<<*(temp+column*i+j)<<" ";
         }
         std::cout<<"\n";
     }
     std::cout<<"\n";
}

int main() {
        /*test OMP   
       Mat D = (Mat_<double>(4, 6) << 7,7,3,7,5,2,8,2,1,4,4,5,8,8,1,10,8,5,4,1,9,1,8,7);
       //Mat X = (Mat_<double>(4, 2) << 5.8,6,3.2,3,8,8,5.2,4.5);
       Mat X = (Mat_<double>(4, 1) << 5.8,3.2,8,5.2);
       Mat S0 = (Mat_<double>(6, 1) << 0,0.4,0,0,0.6,0);
       Mat_<double> S = Mat::zeros(6, 1, CV_64FC1);
       */
       /*test OMP*/
       srand(time(NULL));
       double D[] = {7,7,3,7,5,2,8,2,1,4,4,5,8,8,1,10,8,5,4,1,9,1,8,7};
       //double H[] = {1,1,1,1,1,1,1,1};
       //double X[] = {5.8,6,3.2,3,8,8,5.2,4.5}; 4*2
       //double H[]={1,0,0,1,0,1,1,0};
       double H[676] = {0};
        for(int i=0; i<676; i++) {
        H[i]=1;
       }
       //double X[] = {6,5.6,9.2,4.5};
       double X[676]={0};
       for(int i=0; i<676; i++) {
        X[i]=(rand()%10)*1.0;
       }
       //double S[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
       double S[1014] = {0};

    int sparsity  = 80;
    int atom_size = 4;
    int num_data = 169;
    int num_dict = 6;
    int num_frame = 1;
    //double S[12] = {0,0,0.4,0.6,0,0,0,0,0.5,0.5,0,0};//6*2

        ParaLoop(X, H, D, S, sparsity,atom_size,num_data,num_dict,num_frame);
       //OMP(X,D,sparsity,S);
        //cout<<"thi sis S after ParaLoop \n";
       //printFunct(S, 12, 1);
       //cout << S0 << endl;

//     for(int i=0; i<6; i++) {
//         for(int j=0; j<2; j++) {
//             std::cout<<*(S+2*i+j)<<" ";
//         }
//         std::cout<<"\n";
//     }
    return 1;
}
