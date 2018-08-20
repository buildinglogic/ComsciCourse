close all
clear all
clc

rng('default')
L = 80; % sparsity
S = 256; % reconstruction size

% setup params for the spams OMP
% params.L = 80;
% params.eps = 0;
% params.lambda = 0;
% params.numThreads = -1;

data1 = load('Dictionary'); % load dictionary
D0 = data1.Dictionary; 
data3 = load('Speckle'); % load speckle data
GT = data3.obj_cube;

% parameters for patches
B = [16,16,4]; % Patch size
d = [4,4,4]; % Patch skip (1=fully overlapping, B = nonoverlapping)
pxfrac = 0.5;
% decide problem size
img0 = GT(1:S,1:S,:);
[M1,M2,M3] = size(img0);
M = [M1,M2,M3];

% setup patch stuff for measurements and reconstruction
patchRec = Patchifier(M, B, d);
patchMeas = Patchifier(M(1:2), B(1:2), d(1:2));

% generate mask
sensingMask = rand(M) > pxfrac;
% generate compress measurements
img = sum(sensingMask.*img0,3);

X = patchMeas.getPatches(img); % brake compress measureent into small patches
[Q,N] = size(X);

% get corresponding mask for each patch
H_3D = patchRec.getPatches(sensingMask);
K = size(D0,2);
S = zeros(K*M3,N);
D = blkdiag(D0,D0,D0,D0);

X_ = X';
H_ = H_3D';
D_ = D0';
S_ = S';
workers = 128;

tic
%{
for i=1:N
    Hi = [];
    for j=1:M3 
        Hi = [ Hi, diag(H_3D((1:Q)+(j-1)*Q,i)) ]; 
    end
    S(:,i) = OMP(Hi*D, X(:,i), L);   
%%%% downloaded, only run once, need to be solved
%     S(:,i) = OMP_Cholesky(X(:,i), Hi*D, (Hi*D)'*(Hi*D), (Hi*D)'*X(:,i), L, 1e-12);
%     S(:,i) = OMP_QR(X(:,i), Hi*D, (Hi*D)'*(Hi*D), (Hi*D)'*X(:,i), L, 1e-6);
%     S(:,i) = OMP_MIL(X(:,i), Hi*D, (Hi*D)'*(Hi*D), (Hi*D)'*X(:,i), L, 1e-6);
%%%% complied, only run once, similar problem maybe
%     S(:,i) = mexOMP(X(:,i), Hi*D, params);       
end
%}
%profile on
SS = omp_cilk_mex(X_,H_,D_,S_,80,256,N,512,4, workers);
%profile off
%profsave(profile('info'),'size')
toc

% put reconstructed patches back together to become a complete image
Xrecon = D*SS;
imgRecon = patchRec.makeImage(Xrecon);

for i = 1:4
    figure;  
    subplot(1,2,1); imagesc(img0(:,:,i)); axis equal;
    subplot(1,2,2); imagesc(imgRecon(:,:,i)); axis equal;
end


save('data.mat','imgRecon','img0')