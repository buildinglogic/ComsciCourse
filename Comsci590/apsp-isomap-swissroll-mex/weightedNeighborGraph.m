function W = weightedNeighborGraph (X, epsilon)
%
% WEIGHTEDNEIGHBORGRAPH - Distance-weighted neighborhood graph
%                         (Euclidean ball neighborhoods) 
%   
% SYNTAX
%
%   W = WEIGHTEDNEIGHBORGRAPH( X, EPSILON )
%
% INPUT
%
%   X           Point cloud                     [N-by-D]
%   EPSILON     Euclidean ball radius           [scalar]
%
% OUTPUT
%
%   W           Distance-weighted adjacency     [N-by-N]
%               matrix (dense packing)
%
% DESCRIPTION
%
%   W = WEIGHTEDNEIGHBORGRAPH(X,EPSILON) computes the weighted
%   EPSILON-radius Euclidean-ball neighborhood graph among all points in X.
%
% DEPENDENCIES
%
%   [Statistics and Machine Learning Toolbox]
%
%
% See also      knnsearch, rangesearch
%
    
    
    % number of points
    n = size( X, 1 );
    
    % find indices and distances of all epsilon-neighbors for each point in X
    [idxTgt, D] = rangesearch( X, X, epsilon, 'Distance', 'euclidean' );
    
    % form cell-array of source-node indices (for each neighbor in idxTgt)
    idxSrc = num2cell( 1 : n );
    idxSrc = cellfun( @(itgt,isrc) isrc * ones( size(itgt) ), ...
                      idxTgt(:), idxSrc(:), 'UniformOutput', false );
    
    % concatenate range search cell arrays to long vectors to facilitate
    % global indexing into the distacnce-weighted adjacency matrix
    idxSrc = horzcat( idxSrc{:} );
    idxTgt = horzcat( idxTgt{:} );
    D      = horzcat( D{:} );
    
    % form distance-weighted neighborhood graph
    W       = inf( [n, n], 'like', X );
    idxW    = sub2ind( size(W), idxTgt, idxSrc );
    W(idxW) = D;
    
    
end



%%------------------------------------------------------------
%
% AUTHORS
%
%   Alexandros-Stavros Iliopoulos       ailiop@cs.duke.edu
%
% REVISIONS
%
%   0.1 (Spring 2017) - Alexandros
%
% ------------------------------------------------------------
