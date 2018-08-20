function visInputImages (varargin)
%
% VISINPUTIMAGES - Display source/target images and source mask for
%                  Poisson editing task 
%   
% SYNTAX
%
%   VISINPUTIMAGES( ISTRUCT )
%   VISINPUTIMAGES( ISTRUCT, IJTOPLFTEDIT )
%   VISINPUTIMAGES( HFIG, ... )
%
% INPUT
%
%   ISTRUCT.tgt         Target image            [Mt-by-Nt(-by-3)]
%   ISTRUCT.src         Source image            [Ms-by-Ns(-by-3)]
%   ISTRUCT.msk         Source image mask       [Ms-by-Ns]
%   IJTOPLFTEDIT        Top-left pixel          [i_top, j_left]
%                       coordinates of source
%                       in edited target image
%   HFIG                Figure handle           [figure]
%
% OUTPUT
%
%   
%
% DESCRIPTION
%
%   VISINPUTIMAGES(ISTRUCT) generates a figure with 3 subplots
%   containing the input data for Poisson image editing task:
%       * (1,1): source image
%       * (1,2): target image
%       * (2,1): source image mask
%
%   VISINPUTIMAGES(ISTRUCT,IJTOPLFTEDIT) also displays a rectangle showing
%   where the source image will be edited onto the target image.
%
%   VISINPUTIMAGES(HFIG,...) plots the images on the input figure.
%
% DEPENDENCIES
%
%   sizeEditRegion
%
%
% See also      driver_poisson_editing, poissonEditing, visEditedImage
%
    
    
    % parse input arguments
    [hFig, Istruct, ijTopLftEdit] = parseInput( varargin{:} );
    
    % set current figure
    figure( hFig );
    
    % source image
    subplot( 2, 2, 1 )
    imshow( Istruct.src )
    title( 'source image' )
    
    % source image mask
    subplot( 2, 2, 3 )
    imshow( Istruct.msk(:,:,1) )
    title( 'source mask' )
    
    % target image
    subplot( 2, 2, 2 )
    imshow( Istruct.tgt )
    title( 'target image' )
    
    % source bounding box on target
    if ~isempty( ijTopLftEdit )
        [~, hgtEdit, wdtEdit] = sizeEditRegion( size(Istruct.src), ...
                                                size(Istruct.tgt), ...
                                                ijTopLftEdit );
        hold on
        rectangle( 'Position', [ijTopLftEdit([2,1]), wdtEdit, hgtEdit], ...
                   'EdgeColor', 'r' );
        hold off
    end
    
end



%% LOCAL FUNCTION: INPUT ARGUMENT PARSING

function [hFig, Istruct, ijTopLftEdit] = parseInput (varargin)
    
    % figure handle (if input)
    if isgraphics( varargin{1} )
        hFig        = varargin{1};
        varargin(1) = [];
    else
        hFig = figure;
    end
    
    % images struct
    Istruct     = varargin{1};
    varargin(1) = [];
    
    % top-left source coordinates in target domain (if input)
    if ~isempty( varargin )
        ijTopLftEdit = varargin{1};
    else
        ijTopLftEdit = [];
    end
    
end



%%------------------------------------------------------------
%
% AUTHORS
%
%   Xiaobai Sun                         xiaobai@cs.duke.edu
%   Alexandros-Stavros Iliopoulos       ailiop@cs.duke.edu
%
% VERSION
%
%   0.2
%
% CHANGELOG
%
%   0.2 - Alexandros
%       * optional figure handle specification
%       * editing domain rectangle on target image
%       * removed optional image storage code
%
%   0.1 - Xiaobai
%       * initial implementation
%
% ------------------------------------------------------------
