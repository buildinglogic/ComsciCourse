function [Iedit, tsoln] = poissonEditingJacobi_MG1(Itgt, Isrc, Msrc, ijTopLft, ...
                                                nMaxIter, tauDiffL2, varargin)
%
% POISSONEDITINGJACOBI - Poisson image editing using Jacobi iteration
%   
% SYNTAX
%
%   IEDIT = POISSONEDITINGJACOBI( ITGT, ISRC, MSRC, IJTOPLFT, NMAXITER, IJTOPLFT )
%   IEDIT = POISSONEDITINGJACOBI( ..., 'stepecho', STEPECHO )
%   IEDIT = POISSONEDITINGJACOBI( ..., 'displaysystem', FLAGDISP )
%   [IEDIT, TSOLN] = POISSONEDITINJACOBI( ... )
%
% INPUT
%
%   ITGT        Target image                    [Mt-by-Nt-by-C]
%   ISRC        Source image                    [Ms-by-Ns-by-C]
%   MSRC        Source image mask               [Ms-by-Ns] (logical)
%   IJTOPLFT    Top-left (i,j) coordinates      [i_top, j_left]
%               of edited source image in
%               target image domain
%   NMAXITER    Maximum number of iterations    [scalar]
%   TAUDIFFL2   Termination threshold on 2-norm [scalar]
%               of difference between successive
%               iterates
%   STEPECHO    Number of iteration steps for   [scalar]
%               solution progress echo
%               (no progress echoing if 0)
%               {default: 500}
%   FLAGDISP    Display editing system          [boolean]
%               (guiding gradient fields and
%               right-hand side)?
%               {default: false}
%
% OUTPUT
%
%   IEDIT       Edited image                    [Mt-by-Nt-by-C]
%   TSOLN       Elapsed time (sec) for Jacobi   [scalar]
%               iteration solution computation
%
% DESCRIPTION
%
%   IEDIT = POISSONEDITINGJACOBI(ITGT,ISRC,MSRC,IJTOPLFT,NAMXITER,IJTOPFLT)
%   edits the masked source image onto the target image, at the specified
%   location, by solving the Poisson image editing system [1] with mixed
%   gradients.  The Poisson system is solved using the Jacobi iterative
%   method [2].
%
%   IEDIT = POISSONEDITINGJACOBI(...,'stepecho',STEPECHO) prints a progress
%   update every STEPECHO iterations.
%
%   IEDIT = POISSONEDITINGJACOBI(...,'displaysystem',true) displays figures
%   with the matrices that make up the Poisson image editing system
%   (source/target image gradient fields and resulting right-hand side),
%   prior to the Jacobi iteration computations.
%
%   [IEDIT,TSOLN] = POISSONEDITINGJACOBI(...) also returns the elapsed time
%   (in seconds) for the Jacobi iteration solution computations.  (This
%   does not include system set-up computations.)
%
% REFERENCES
%
%   [1] P. Pérez, M. Gangnet, and A. Blake, "Poisson Image Editing," in ACM
%   Transactions on Graphics - Proceedings of ACM SIGGRAPH 2003, vol. 22,
%   no. 3, pp. 313-318, 2003.  [DOI: 10.1145/882262.882269]
%
%   [2] http://www.cs.berkeley.edu/~demmel/cs267/lecture24/lecture24.html
%
% DEPENDENCIES
%
%   <none>
%
%
% See also      imfilter
%
    
    
    %% DEFAULTS & PARAMETERS
    
    % optional input default values
    stepEcho    = 500;
    flagDisplay = false;
    
    % gradient parameters
    hDeriv    = [-1, 1];                % derivative kernel
    hLaplace  = [0 1 0; 1 0 1; 0 1 0];  % Laplace kernel (excluding self)
    hLaplace = gpuArray(hLaplace);
    bndFilter = 'replicate';            % boundary type for derivative and
                                        % Laplace filters (see imfilter)
    
    % image description strings (for Poisson editing system display)
    strTgt = 'target image';
    strSrc = 'source image';
    strRhs = '|RHS| (guiding gradient field divergence)';
    
    
    %% OPTIONAL ARGUMENTS PARSING
    
    [stepEcho, flagDisplay] = parseOptArgs( stepEcho, flagDisplay, ...
                                            varargin{:} );
    
    
    %% INITIALIZATION / SYSTEM SET-UP
    
    Itgt = gpuArray(Itgt);
    Isrc = gpuArray(Isrc);
    % compute x/y image gradients
    GxTgt = imfilter( Itgt, hDeriv(:).', bndFilter ); % row filtering
    GxSrc = imfilter( Isrc, hDeriv(:).', bndFilter );
    GyTgt = imfilter( Itgt, hDeriv(:)  , bndFilter ); % column filtering
    GySrc = imfilter( Isrc, hDeriv(:)  , bndFilter );
    
    % calculate the size and bottom-right corner of source in the edited image
    [ijBtmRgt, hgtSrcEdit, wdtSrcEdit] = ...
        sizeEditRegion( size(Isrc), size(Itgt), ijTopLft );
    
    % create source mask in target domain
    Medit = false( size(Itgt) );  
    Medit( ijTopLft(1):ijBtmRgt(1), ijTopLft(2):ijBtmRgt(2), : ) = ...
        Msrc(1:hgtSrcEdit, 1:wdtSrcEdit, :);
    
    % form right-hand side (divergence of mixed gradient field)
    GxMixed        = GxTgt;
    GyMixed        = GyTgt;
    GxMixed(Medit) = GxSrc(Msrc);
    GyMixed(Medit) = GySrc(Msrc);
    B = imfilter( GxMixed, hDeriv(:).', bndFilter ) + ...
        imfilter( GyMixed, hDeriv(:)  , bndFilter );
    
    
    %% VISUALIZATION: POISSON EDITING SYSTEM
    
    if flagDisplay
        visGradientFields( GxTgt, GyTgt, strTgt );
        visGradientFields( GxSrc, GySrc, strSrc );
        visRightHandSide( B, strRhs );
    end
    
    
    %% JACOBI ITERATIVE METHOD / SYSTEM SOLUTION
    

    % initialize solution (source superposition on target)
    IeditInit        = Itgt;
    IeditInit(Medit) = Isrc(Msrc);
    
    % Jacobi iteration solver
    [Iedit, tsoln] = ...
        poissonEditingJacobi_solve( IeditInit, Medit, B, ...
                                    nMaxIter, tauDiffL2, ...
                                    hLaplace, bndFilter, stepEcho );
%     Iedit = gather(Iedit);
end



%% ===== LOCAL FUNCTION: JACOBI ITERATION METHOD SOLVER

function [Iedit, tsoln] = ...
        poissonEditingJacobi_solve (Iedit, Medit, B, ...
                                    nMaxIter, tauDiffL2, ...
                                    hLaplace, bndFilter, stepEcho)
    
    % initialize successive iterate difference to numerically large value
    diffIterL2 = 1e+100;
    
    % iteration initiation echo
    if stepEcho > 0
        fprintf( '   > Starting Jacobi method iterations\n' )
    end
    
    % start timing
    tic
   
    % Jacobi iteration steps
    for k = 1 : nMaxIter
        
        % previous-step solution
        IeditPrev      = Iedit;
        diffIterL2Prev = diffIterL2;
        
        % compute next iterate: Inew(i,j) = (L{Iprev(i,j)} - B(i,j)) / 4
        ILaplaceNbr  = imfilter( Iedit, hLaplace, bndFilter );
%         figure; imagesc(ILaplaceNbr); axis equal;
        Iedit(Medit) = (ILaplaceNbr(Medit) - B(Medit)) / 4;
%         if mod(k,500) == 0
%             figure; imagesc(Iedit); axis equal;
%         end
        % calculate 2-norm of difference between solution steps
        diffIterL2 = sqrt( sum( (Iedit(:) - IeditPrev(:)).^2 ) );
        
        % termination condition (convergence; solution stagnation)
        if ((diffIterL2Prev - diffIterL2) / diffIterL2Prev) < tauDiffL2
            break
        end
        
        % progress echo (iteration number)
        if (stepEcho > 0) && (mod( k, stepEcho ) == 0)
            fprintf( '     -- iteration #%d\n', k )
        end
        
    end  % for (i; Jacobi iteration)
    
    % iteration computations time
    tsoln = toc;
    
    % termination echo
    if stepEcho > 0
        fprintf( '     Finished at iteration #%d\n', k )
    end
 
end



%% ===== LOCAL FUNCTION: VISUALIZE GRADIENT FIELDS OF COLOR IMAGE

function visGradientFields (Gx, Gy, strImg)
    
    figure
    
    nClr = size( Gx, 3 );
    for c = 1 : nClr
        
        subplot( 2, nClr, c )
        imagesc( abs( Gx(:,:,c) ) )
        axis image
        set( gca, 'XTick', [], 'Ytick', [] )
        if c == 1
            ylabel( '$|\nabla_x I|$', 'Interpreter', 'latex' )
        end
        
        subplot( 2, nClr, c+nClr )
        imagesc( abs( Gy(:,:,c) ) )
        axis image
        set( gca, 'XTick', [], 'YTick', [] )
        if c == 1
            ylabel( '$|\nabla_y I|$', 'Interpreter', 'latex' )
        end
        xlabel( sprintf( '$c_{%d}$', c ), 'Interpreter', 'latex' )
        
    end  % for (c)
    
    colormap gray
    suptitle( ['gradient fields of ' strImg] )
    
end



%% ===== LOCAL FUNCTION: VISUALIZE RIGHT-HAND SIZE

function visRightHandSide (B, strImg)
    
    figure
    
    nClr = size( B, 3 );
    for c = 1 : nClr
        
        subplot( 1, nClr, c )
        imagesc( abs( B(:,:,1) ) )
        axis image
        set( gca, 'XTick', [], 'YTick', [] )
        xlabel( sprintf( '$c_{%d}$', c ), 'Interpreter', 'latex' )
        
    end  % for (c)
    
    colormap gray
    suptitle( strImg )
    
end



%% ===== LOCAL FUNCTION: OPTIONAL ARGUMENT PARSING

function [stepEcho, flagDisplay] = parseOptArgs (stepEcho, flagDisplay, ...
                                                 varargin)
    
    % create inputParser object for parsing of name-value pair input
    ip = inputParser;
    
    % specify optional arguments and default values
    addParameter( ip, 'stepecho'     , stepEcho    );
    addParameter( ip, 'displaysystem', flagDisplay );
    
    % parse input name-value pair arguments
    parse( ip, varargin{:} );
    
    % set output
    stepEcho    = ip.Results.stepecho;
    flagDisplay = ip.Results.displaysystem;
    
end



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
