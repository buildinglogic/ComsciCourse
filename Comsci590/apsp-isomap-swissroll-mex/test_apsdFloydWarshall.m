%
% SCRIPT: test_apsdFloyWarshall.m
%
%
% Simple testing script for the Floyd-Warshall APSD function, using a
% ring graph.
%
%
% Dependencies
% ============
%
% apsdFloydWarshall
%
%
% --------------------------------------------------
% CompSci 590.06: Parallel Programming
% Department of Computer Science
% Duke University
% --------------------------------------------------
%



%% CLEAN UP

clear variables
close all


%% PARAMETERS

% number of nodes
n = 20;

% max weight
wMax = 10;

% numerical equality threshold
tau = 1e-6;


%% (BEGIN)

fprintf( '\n***** BEGIN (%s) *****\n\n', mfilename )


%% LINEAR GRAPH GENERATION

fprintf( '...generating linear graph with random weights in [0,%g]...\n', ...
         wMax )
fprintf( '   - number of nodes: %d\n', n )

weights        = wMax * rand( n-1, 1 ); % random weights
A              = inf( n, n );           % initialize INF distances
A(1  :n+1:n*n) = 0;                     % 0s on the diagonal
A(2  :n+1:n*n) = weights;               % weights on the sub-diagonal
A(n+1:n+1:n*n) = weights;               % weights on the super-diagonal


%% FORM KNOWN APSD MATRIX

fprintf( '...computing true APSD matrix...\n' )

wPrefixSum = [0; cumsum( weights )];
Dgold      = zeros( n, n );
for i = 1 : n
    for j = 1 : n
        Dgold(i,j) = abs( wPrefixSum(j) - wPrefixSum(i) );
    end
end


%% COMPUTE APSD MATRIX WITH FLOYD-WARSHALL FUNCTION

fprintf( '...computing APSD matrix with ''apsdFloydWarshall'' function...\n' )

D = apsdFloydWarshall( A );

mae = max( abs( D(:) - Dgold(:) ) );
if mae < tau
    fprintf( '   > PASS' )
else
    fprintf( '   > FAIL' )
end
fprintf( ' (MAE = %g)\n', mae )


%% (END)

fprintf( '\n***** END (%s) *****\n\n', mfilename )
