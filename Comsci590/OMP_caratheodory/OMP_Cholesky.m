function s = OMP_Cholesky(x, dict, D, dictTx, natom, tolerance)
% OMPDCHOL.m -- Orthogonal Matching Pursuit with Cholesky decomposition
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
dictsize = size(dict,2);
L = zeros(natom,natom);
gamma = zeros(natom,1);
% find initial projections
projections = dictTx;
solveropts.LT = true;
solveropts2.UT = true;

k = 1;
while normr2 > normtol2 && k <= natom
  % find index of maximum magnitude projection
  [alpha,maxindex] = max(projections.^2); % cost N
  newgam = maxindex(1);
  gamma(k) = newgam;
  % update Cholesky decomposition, projections, and residual energy  
  if k==1
    L(1,1) = 1;
    % find solution
    stemp = projections(newgam);
    % update projections
    projections = dictTx - D(:,newgam)*stemp; % cost N
    % update residual energy
    normr2 = normx2 - stemp*stemp;
  else
    gammak = gamma(1:k);
    v = linsolve(L(1:k-1,1:k-1),D(gamma(1:k-1),newgam),solveropts); % cost k^2
    L(k,k) = sqrt(1-v'*v); % cost k
    L(k,1:k-1) = v';
    % find new solution
    Lk = L(1:k,1:k); 
    w = linsolve(Lk,dictTx(gammak),solveropts); % cost k^2
    stemp = linsolve(Lk',w,solveropts2); % cost k^2
    % update projections
    projections = dictTx - D(:,gammak)*stemp; % cost Nk
    % update residual energy
    normr2 = normx2 - w'*w; % cost k
  end
  k = k + 1;
  disp(k);
end
s = zeros(dictsize,1);
s(gammak) = stemp;

end


