rng('default')

%parameters for patches
B = [8,8,4]; % Patch size
d = [4,4,4]; % Patch skip (1=fully overlapping, B = nonoverlapping)

sigma = 0.001;
pxfrac = 0.5;

AOE = {(-128:127)+512, (-128:127)+512};

% get true data
load Uncoded_speckles.mat
img0 = processed_cube(AOE{1},AOE{2}, :);
[M1,M2,M3] = size(img0);
M = [M1,M2,M3];

% setup patch stuff for measurements and reconstruction
patchMeas = Patchifier(M, B(1:2), d);
patchRec = Patchifier(M, B, d);

%add noise
img = img0 + sigma*randn(M);

%make mask
sensingMask = rand(M) > pxfrac;

% compress measurements
img = sum(sensingMask.*img,3);

X = patchMeas.getPatches(img);
X0 = patchRec.getPatches(img0);
[Q,N] = size(X);

% get mask for each patch
H_3D = sparse(logical(patchRec.getPatches(sensingMask)));
H = false(Q,M3*Q,N);
for i=1:N
    Hi = [];
    for j=1:M3, Hi = [ Hi, diag(H_3D((1:Q)+(j-1)*Q,i)) ]; end
    H(:,:,i) = Hi;
end

%%% do reconstruction here
%%% cheat and load from file
load bpfa.mat
% load tbpfa.mat

S = zeros(K,N);
for i=1:N
    S(:,i) = OMP(D, X(:,i), L)
end

%put image together from learned dictionary and weights
Xrecon = state.D*(state.Z.*state.S)';
imgRecon = patchRec.makeImage(Xrecon);

%compute error
PSNR = psnr(imgRecon, img0)
MSE = mse(imgRecon(:), img0(:))
RMSE = sqrt(MSE)

%compute autocorrelation error
ac = @(x) ifftshift(ifft2(abs(fft2(fftshift(x))).^2));
for i=1:M3
    aci = ac(imgRecon(:,:,i))/max(max(ac(imgRecon(:,:,i))));
    ac0i = ac(img0(:,:,i))/max(max(ac(img0(:,:,i))));

    PSNRac(i) = psnr(aci, ac0i);
    MSEac(i) = mse(aci, ac0i);
    RMSEac(i) = sqrt(MSEac(i));
end
PSNRac, MSEac, RMSEac

%show some pictures
figure(1)
imagesc([  (img0(:,:,1)), (img0(:,:,2)), (img0(:,:,3)), (img0(:,:,4));
           (imgRecon(:,:,1)), (imgRecon(:,:,2)), (imgRecon(:,:,3)), (imgRecon(:,:,4));
        ])

pause(1)

figure(2)
DispDic(reshape(state.D,B(1),B(2)*B(3),size(state.D,2)));

