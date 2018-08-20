function s = OMP_MIL(x, dict, D, dictTx, natom, tolerance)
% OMPDMIL.m -- Orthogonal Matching Pursuit with matrix inversion lemma
%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%
% INPUTS
% x         - input signal
% dict      - dictionary
% D         - dictionary Gramian
% dictTx    - dict'*x
% natom     - stopping crit. 1: max number of iterations
% tolerance - stopping crit. 2: minimum norm residual
%
%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%
% OUTPUTS
% s  - solution to x = dict*s
%
%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%
% Bob L. Sturm <bst@create.aau.dk> 20111106 in Monterey California :)
% Department of Architecture, Design and Media Technology
% Aalborg University Copenhagen
% Lautrupvang 15, 2750 Ballerup, Denmark
%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%=%

x = x(:);
% initialization
residual = x;
normx2 = x'*x;
normtol2 = tolerance*normx2;
normr2 = normx2;
atomlen = size(dict,1);
dictsize = size(dict,2);
projections = dictTx;
dualbasis = zeros(atomlen,natom);
stemp = zeros(natom,1);
gamma = zeros(natom,1);

k = 1;
while normr2 > normtol2 && k <= natom
  % find index of maximum magnitude projection
  [alpha,maxindex] = max(projections.^2); % cost N
  newgam = maxindex(1);
  gamma(k) = newgam;
  
  % update QR factorization, projections, and residual energy  
  if k==1
    % update dualbasis
    dualbasis(:,1) = dict(:,newgam);
    % find solution
    stemp(1) = projections(newgam);
    % update projections
    projections = dictTx - D(:,newgam)*stemp(1); % cost N
    % update residual energy
    normr2 = normx2 - stemp(1)*stemp(1);
  else
    % update dualbasis
    gammakm1 = gamma(1:k-1);
    tempbasis = dualbasis(:,1:k-1);
    vh = tempbasis'*dict(:,newgam); % cost Mk
    projphi = dict(:,gammakm1)*vh;  % cost Mk
    vv = dict(:,newgam) - projphi; % cost M
    lambda = 1/(1-projphi'*projphi);  % cost M
    lambdavh = lambda*vh; % cost k
    lambdavv = lambda*vv; % cost M
    lambdavvvhH = vv*lambdavh'; % cost Mk
    dualbasis(:,1:k-1) = tempbasis - lambdavvvhH; % cost Mk
    dualbasis(:,k) = lambdavv;
    % find solution
    delta = lambda*dictTx(newgam) - dictTx(gammakm1)'*lambdavh; % cost k
    stemp(1:k) = stemp(1:k) - delta*[vh; -1]; % cost k
    normr2 = normr2 + delta*delta*sum(vv.^2) - 2*delta*projections(newgam); % cost k
    projections = dictTx - D(:,gamma(1:k))*stemp(1:k); % cost Nk
  end
  k = k + 1;
end
k = k-1;
% find solution
s = zeros(dictsize,1);
s(gamma(1:k)) = stemp(1:k);

