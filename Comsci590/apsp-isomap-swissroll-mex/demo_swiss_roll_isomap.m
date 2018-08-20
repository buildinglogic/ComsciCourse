%
% SCRIPT: demo_swiss_roll_isomap.m
%
%
% Demo script for manifold embedding and projection onto low-dimensional
% Euclidean space, using points on a 3D "Swiss roll" manifold and
% unrolling them into a 2D "sheet" using ISOMAP [1].
%
% The ISOMAP method entails calculation of all pair-wise geodesic
% distances, as per an appropriate neighborhood graph.  That is, it entails
% the all-pairs-shortest-paths solution given a sparse, weighted and
% non-symmetric adjacency matrix (directed distance-graph).
%
%
% Dependencies
% ============
%
% swissroll
% weightedNeighborGraph
%
%
% References
% ==========
%
% J. B. Tenenbaum, V. de Silva, and J. C. Langford, "A Global Geometric
% Framework for Nonlinear Dimensionality Reduction," Science, vol. 290,
% no. 5500, pp. 2319-2323, Dec. 2000.
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
clc


%% PARAMETERS

% number of samples on the Swiss roll
n = [];

% ball radius for neighborhood graph formation
epsilonNbrGraph = pi;

% output dimensionality
d = 2;

% visualization parameters
mrkMark = '.';
mrkSize = 50;


%% (BEGIN)

fprintf( '\n***** BEGIN (%s) *****\n\n', mfilename )


%% USER PROMPTS

if isempty( n )
    n = input( '> Number of samples on Swiss roll manifold: ' );
    fprintf( '\n' )
end


%% DATA GENERATION

fprintf( '...generating samples on a 3D Swiss roll manifold...\n' )
fprintf( '   - n = %d\n', n )

[X, Xparam, iSort] = swissroll3( n );


%% DISTANCE-WEIGHTED NEIGHBORHOOD GRAPH

fprintf( '...forming weighted adjacency matrix for neighborhood graph...\n' )
fprintf( '   - neighborhood ball radius: %f\n', epsilonNbrGraph )

W = weightedNeighborGraph( X, epsilonNbrGraph );


%% ALL PAIRS SHORTEST DISTANCES (GEODESIC DISTANCES)

fprintf( '...computing geodesic distances (all-pairs shortest distances)...\n' )


%%%%%%%%%%%%%%% choose method here !!!!  %%%%%%%%%%%%%%%%%%%%%%%%%%%%
transW = W';
tic

%fprintf( ' \nMATLAB....running original floyd (provided by instructor)  .....\n' )
%D = apsdFloydWarshall(W);

%fprintf(' MATLAB....running matrix multiplication APSP C .....\n' )
%D = calculateMatrix_mex( transW, 1 );

%fprintf(' MATLAB....running matrix multiplication APSP cilk .....\n' )
%D = calculateMatrix_mex( transW, 2 );

%fprintf(' MATLAB....running floyd APSP C .....\n' )
%D = calculateMatrix_mex( transW, 3 );

%fprintf(' MATLAB....running floyd APSP cilk .....\n' )
%D = calculateMatrix_mex( transW, 4 );

fprintf(' MATLAB....running APSP block-wise MM with cilk.....\n' )
%%%%%%% the last argument is # of workers %%%%%%%%%
D = calculateMatrix_mex( transW, 5, 1, 256 );

toc
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%% DIMENSIONALITY REDUCTION

fprintf( '...ISOMAP dimensionality reduction...\n' )
fprintf( '   - output dimensionality: %d', d )

% form the inner-product distance matrix (to get the EVD of)
M = D.^2;
M = -0.5 * (M - sum(M,1)/n - sum(M,2)/n + sum(M(:))/(n^2));

% ISOMAP dimensionality reduction
[Y, lambda] = eig( M, 'vector' );

% remove unwanted dimensions and scale the rest
Y = Y(:,1:d) .* sqrt( lambda(1:d).' );


%% VISUALIZATIONS

% ---------- 3D Swiss roll

figure;
scatter3( X(:,1), X(:,2), X(:,3), mrkSize, Xparam(:,1), mrkMark );
axis equal
box on
rotate3d
xlabel( 'x' )
ylabel( 'y' )
zlabel( 'z' )
title( 'Swiss roll manifold samples in 3D space' )
print(gcf,'F1','-dpng','-r300')
% ---------- neighborhood graph adjacency matrix

figure;
% point-cloud
subplot( 1, 2, 1 )
imagesc( W )
axis image
colormap( hot )
colorbar
title( {'distance-weighted adjacency matrix of neighborhoods graph', ...
        '(Swiss roll sample points in random order)'} )

% sorted
subplot( 1, 2, 2 )
imagesc( W(iSort,iSort) )
axis image
colormap( hot )
colorbar
title( {'distance-weighted adjacency matrix of neighborhoods graph', ...
        '(Swiss roll sample points sorted according to their trace)'} )

print(gcf,'F2','-dpng','-r300')
% ---------- APSD matrix

figure;
% point cloud
subplot( 1, 2, 1 )
imagesc( D )
axis image
colormap( hot )
colorbar
title( {'all pairs shortest distances matrix', ...
        '(Swiss roll sample points in random order)'} )

% sorted
subplot( 1, 2, 2 )
imagesc( D(iSort,iSort) )
axis image
colormap( hot )
colorbar
title( {'all pairs shortest distances matrix', ...
        '(Swiss roll sample points sorted according to their trace)'} )
print(gcf,'F3','-dpng','-r300')

% ---------- 2D "unrolled" Swiss roll

figure;
scatter( Y(:,1), Y(:,2), mrkSize, Xparam(:,1), mrkMark );
xlabel( '\lambda_N' )
ylabel( '\lambda_{N-1}' )
title( {'Swiss roll samples in ISOMAP-generated 2D space', ...
        '("unrolled" roll)'} )
print(gcf,'F4','-dpng','-r300')
%% (END)

fprintf( '\n***** END (%s) *****\n\n', mfilename )



%%------------------------------------------------------------
%
% AUTHOR
%
%   Alexandros-Stavros Iliopoulos       ailiop@cs.duke.edu
%
% REVISIONS
%
%   0.1 (Spring 2017)
%
% ------------------------------------------------------------
