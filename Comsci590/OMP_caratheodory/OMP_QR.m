function s = OMP_QR(x, dict, D, dictTx, natom, tolerance)
% OMPDQR.m -- Orthogonal Matching Pursuit with QR decomposition
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
R = zeros(natom,natom);
Q = zeros(length(x),natom);
gamma = zeros(natom,1);
projections = dictTx';
% find initial projections
solveropts.LT = true;
solveropts2.UT = true;
crossoverit = (sqrt(dictsize^2 + 4*dictsize*size(dict,1)) - dictsize)/2;

k = 1;
while normr2 > normtol2 && k <= natom
  % find index of maximum magnitude projection
  [alpha,maxindex] = max(projections.^2); % cost N
  newgam = maxindex(1);
  gamma(k) = newgam;
  % update QR factorization, projections, and residual energy  
  if k==1
    R(1,1) = 1;
    Q(:,1) = dict(:,newgam);
    stemp = projections(newgam);
    % update projections
    projections = projections - D(gamma(1:k),:)*stemp; % cost Nk
    % update residual energy
    normr2 = normx2 - stemp*stemp;
  else
    gammak = gamma(1:k);
    w = Q(:,1:k-1)'*dict(:,gamma(k)); % cost kM
    R(k,k) = sqrt(1-sum(w.^2)); % cost k
    R(1:k-1,k) = w;
    Q(:,k) = (dict(:,newgam)-Q(:,1:k-1)*w)/R(k,k); % cost kM
    if k > crossoverit
      qkTx = Q(:,k)'*x; % cost M
      projections = projections - (qkTx*Q(:,k))'*dict; % cost Nm
      normr2 = normr2 - qkTx*qkTx;
    else
      tempR = R(1:k,1:k);
      y = linsolve(tempR',dictTx(gammak),solveropts); % cost k^2
      stemp = linsolve(tempR,y,solveropts2); % cost k^2
      % update projections
      projections = dictTx - D(:,gammak)*stemp; % cost Nk
      % update residual energy
      normr2 = normx2 - sum(y.^2); % cost k
    end 
  end
  k = k + 1;
end
% find solution
k = k-1;
s = zeros(dictsize,1);
if k > crossoverit
  tempR = R(1:k,1:k);
  w = linsolve(tempR',dictTx(gammak),solveropts);  
  s(gamma(1:k)) = linsolve(tempR,w,solveropts2);
else
  s(gammak) = stemp;
end
