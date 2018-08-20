function s = OMP_Cholesky2(D,X,L)

I = [];
L = [1];
residual = X;
gamma = 0;
alpha = D'*X;
for ii = 1:L
    projection=D'*residual;
    [maxVal,pos]=max(abs(projection));
    if ii == 1
        w = 
    end
end
end