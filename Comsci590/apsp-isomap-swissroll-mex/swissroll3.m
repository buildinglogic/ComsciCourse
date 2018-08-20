function [X, Xparam, idxSort] = swissroll3 (numSamples, flagSort)
%
% SWISSROLL3 - Generate points on a 3D-embedded Swiss roll surface
%   
% SYNTAX
%
%   [X, XPARAM] = SWISSROLL3( NUM )
%   [X, XPARAM, ISORT] = SWISSROLL3( NUM )
%
% INPUT
%
%   NUM         Number of Swiss roll samples            [scalar]
%
% OUTPUT
%
%   X           Swiss roll point-cloud                  [N-by-3]
%   XPARAM      Intrinsic longitudinal and lateral      [N-by-2]
%               coordinates on the unrolled swiss roll
%               (i.e. on the manifold parameter space)
%   ISORT       Sorted sample indices, based on their   [N-by-1]
%               longitudinal ordering
%
% DESCRIPTION
%
%   [X,XPARAM] = SWISSROLL3(NUM) generates NUM points on a
%   uniformly-sampled Swiss roll surface, and returns their 3D coordinates
%   in X and the corresponding manifold parameters in XPARAM.
%
%   [X,XPARAM,ISORT] = SWISSROLL3(NUM) also returns the sample indices
%   in sorted order, as per their longitudinal trace parameter.
%
% DEPENDENCIES
%
%   <none>
%
%
% See also      demo_nodaldom_swissroll
%
    
    
    %% PARAMETERS
    
    % trace curve parameters: t(x) = phi * (alpha + beta*x)
    trPhi   = 2*pi;
    trAlpha = 0.5;
    trBeta  = 1.5;
    
    % Swiss roll width parameters: w(y) = alpha + beta*y
    wdAlpha = 0;
    wdBeta  = 3*pi;
    
    
    %% SWISS ROLL SAMPLES
    
    % generate samples on the tracing curve
    tr = trPhi * (trAlpha + trBeta * rand( numSamples, 1 ) );
    
    % sorted sample indices (based on longitudinal trace)
    [~, idxSort] = sort( tr );
    
    % generate random width offsets for all samples
    wd = wdAlpha + wdBeta * rand( numSamples, 1 );
    
    % Swiss roll mapping: (x,y) --> (x cosx, y, x sinx)
    X = [tr .* cos(tr), wd, tr .* sin(tr)];
    
    % Swiss roll parameter space output?
    if nargout > 1
        Xparam = [tr, wd];
    end
    
    
end



%%------------------------------------------------------------
%
% AUTHORS
%
%   Alexandros-Stavros Iliopoulos       ailiop@cs.duke.edu
%
% VERSION
%
%   0.1 - July 25, 2015
%
% CHANGELOG
%
%   0.1 (Jul 25, 2015) - Alexandros
%       * initial implementation
%
% ------------------------------------------------------------
