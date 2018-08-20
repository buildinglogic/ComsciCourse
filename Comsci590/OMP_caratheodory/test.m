close all
clear all
clc

%% test mex function of Ploop

D = [7,7,3,7,5,2;8,2,1,4,4,5;8,8,1,10,8,5;4,1,9,1,8,7];
X = [6, 5.6, 9.2, 4.5]';
H = [1,0,0,1,0,1,1,0]';
S = zeros(12,1);

D_ = D';
X_ = X';
H_ = H';
S_ = S';
L = 4;
Se = omp_cilk_mex(X_,H_,D_,S_,L,size(X,1),size(X,2),size(D,2),size(H,1)/size(X,1));
disp(['Estimated coef is :'])
Se