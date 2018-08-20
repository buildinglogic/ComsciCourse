% demo_poisson_editing.m
%
%
% Driver script for demonstration of Poisson image editing for seamless
% cloning of a source image onto a target.
%
%
% Demo cases
% ==========
%
% case 1: airplane (source) over terrain (target)
% case 2: handwritten whiteboard text (source) onto brick wall (target)
%
%
% Data source
% ===========
%
% http://cs.brown.edu/courses/csci1950-g/results/proj2/pdoran/
%
%
% References
% ==========
%
% P. Pérez, M. Gangnet, and A. Blake, "Poisson Image Editing," in ACM
% Transactions on Graphics - Proceedings of ACM SIGGRAPH 2003, vol. 22,
% no. 3, pp. 313-318, 2003.  [DOI: 10.1145/882262.882269]
%
%
% Dependencies
% ============
%
% poissonEditingJacobi
% readInputData
% visInputImages
% visEditedImage
% sizeEditRegion
% suptitle
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

% paths to data and corresponding string descriptors
dirData  = '../data';
pathData = {[dirData filesep 'airplane_terrain.mat'], ...
            [dirData filesep 'writing_wall.mat']};
Method_MG = ['2']; % 0 - original; 1 - MG1; 2 - MG3

% brief description for each editing case (corresponding to datasets)
strCase  = {'airplane (source) over terrain (target)', ...
            'written text (source) on brick wall (target)'};

% source coordinates in target domain (top-left corner [i,j]) for each case
ijSrcTopLft = {[ 60, 100], ...
               [ 20,  20]};

% execution parameters
nMaxIter    = 5000;        % max number of Jacobi solver iterations
tauDiffL2   = 1e-6;        % threshold for insignificant iterate difference

% display flags
flagDisplay      = true; % plot figures/images? (off for shell-only execution)
flagVisSystem    = true; % display editing system data?
stepEchoProgress = 500;  % iteration steps for progress echo


%% (BEGIN)

fprintf( '\n***** BEGIN (%s) *****\n\n', mfilename )


%% INITIALIZATION

% number of demo cases
nCases = length(pathData);

% initialize arrays of empty image-data structs & computation times
Idata(nCases) = struct( 'tgt', [], 'src', [], 'msk', [], 'edited', [] );
tsoln         = zeros( nCases, 1 );

% initialize graphics objects for input/edited image figures
if flagDisplay
    hFig = gobjects( nCases, 1 );
end


%% DEMO CASES (POISSON IMAGE EDITING)

% iterate over all cases
for c = 1 : nCases
    
    % matlab profiler on
    profile on
    
    % prompt next case
    if c > 1
        fprintf( '(press any key for next demo case)\n' )
        pause
    end
    
    % demo case echo
    fprintf( 'CASE %d: %s\n\n', c, strCase{c} )
    
    % load data
    fprintf( '...loading data...\n' )
    fprintf( '   - path: ''%s''\n', pathData{c} )
    Idata(c) = readInputData( pathData{c} );
    
    % display input data
    if flagDisplay
        hFig(c) = figure;
        visInputImages( hFig(c), Idata(c), ijSrcTopLft{c} );
        suptitle( sprintf( 'case %d images', c ) )
        drawnow
    end
    
    % ~~~ POISSON IMAGE EDITING ~~~
    
    fprintf( '...calling Poisson image editing function...\n' )
    fprintf( '   - source top-left coordinates (i,j) in target: (%d,%d)\n', ...
             ijSrcTopLft{c} )
    fprintf( '   - maximum number of iterations: %d\n', nMaxIter )
    fprintf( '   - relative solution difference threshold: %.2g\n', tauDiffL2 )
    
    % ~~~ Different acceleration method
    switch Method_MG
        case '0'
            fprintf('\n Start original method ... ...\n');
            [Idata(c).edited, tsoln(c)] = ...
                poissonEditingJacobi( Idata(c).tgt, Idata(c).src, Idata(c).msk, ...
                              ijSrcTopLft{c}, nMaxIter, tauDiffL2, ...
                              'stepecho', stepEchoProgress, ...
                              'displaysystem', flagDisplay && flagVisSystem );
        case '1'
            fprintf('\n Start MG1 method ... ...\n');
            [Idata(c).edited, tsoln(c)] = ...
                poissonEditingJacobi_MG1( Idata(c).tgt, Idata(c).src, Idata(c).msk, ...
                              ijSrcTopLft{c}, nMaxIter, tauDiffL2, ...
                              'stepecho', stepEchoProgress, ...
                              'displaysystem', flagDisplay && flagVisSystem );
        case '2'
            fprintf('\n Start MG3 method ... ...\n');
            [Idata(c).edited, tsoln(c)] = ...
                poissonEditingJacobi_MG3( Idata(c).tgt, Idata(c).src, Idata(c).msk, ...
                              ijSrcTopLft{c}, nMaxIter, tauDiffL2, ...
                              'stepecho', stepEchoProgress, ...
                              'displaysystem', flagDisplay && flagVisSystem );
    end
    
    % echo computation time
    fprintf( ' * elapsed time: %.2fs\n', tsoln(c) )
    
    % display edited image in input-images figure
    if flagDisplay
        visEditedImage( hFig(c), Idata(c).edited );
        drawnow
    end
    
    % blank line echo
    fprintf( '\n' )
    
    % matlab profiler off and save
    profile off
    profsave(profile('info'),['Case',num2str(c),'method',Method_MG])
    
end  % for (c; demo cases)


%% (END)

fprintf( '\n***** END (%s) *****\n\n', mfilename )



%%------------------------------------------------------------
%
% AUTHORS
%
%   Alexandros-Stavros Iliopoulos       ailiop@cs.duke.edu
%   Xiaobai Sun                         xiaobai@cs.duke.edu
%   Tiancheng Liu
%
% REVISIONS
%
%   0.3 (Spring 2017) - Alexandros
%   0.2 (Spring 2016) - Xiaobai
%   0.1 (Spring 2015) - Tiancheng
%
% ------------------------------------------------------------
