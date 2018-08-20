function D = apsdFloydWarshall (W)
%
% APSDFLOYDWARSHALL - All pairs shortest distances (Floyd-Warshall
%                     algorithm) 
%   
% SYNTAX
%
%   D = APSDFLOYDWARSHALL( W )
%
% INPUT
%
%   W           Distance-weighted adjancency matrix     [N-by-N]
%
% OUTPUT
%
%   D           All pairs shortest distances            [N-by-N]
%
% DESCRIPTION
%
%   D = APSDFLOYDWARSHALL(W) computes the all-pairs shortest distances
%   matrix, given a distance-weighted adjacency matrix.  Absence of edges
%   is markedby Inf values in the W matrix.
%
% DEPENDENCIES
%
%   <none>
%
%
% See also      
%
    
    
    D = W;
    for k = 1 : size(W,1)
        D = min( D, D(:,k) + D(k,:) );
    end
    
    
end



%%------------------------------------------------------------
%
% AUTHORS
%
%   Nikos Pitsianis                     nikos@cs.duke.edu
%
% REVISIONS
%
%   0.1 (Spring 2017)
%
% ------------------------------------------------------------
