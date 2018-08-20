function Idata = readInputData (pathData)
%
% READINPUTDATA - Read input image data (Poisson editing) from MAT-file
%   
% SYNTAX
%
%   IDATA = READINPUTDATA( PATHDATA )
%
% INPUT
%
%   PATHDATA    Path to MAT-file with target image      [string]
%               and source image/mask
%
% OUTPUT
%
%   IDATA.tgt   Target image            [Mt-by-Nt-by-3]
%   IDATA.src   Source image            [Ms-by-Ns-by-3]
%   IDATA.msk   Source image mask       [Ms-by-Ns] (logical)
%
% DESCRIPTION
%
%   IDATA = READINPUTDATA(PATHDATA) reads the image/mask data from the
%   MAT-file in the input path, and converts image data from uint8 to
%   double.
%
% DEPENDENCIES
%
%   <none>
%
%
% See also      driver_poisson_editing, poissonEditing, load, matfile
%
    
    
    ioData = matfile( pathData );
    
    Idata = struct( 'tgt'   , ioData.tgt, ...
                    'src'   , ioData.src, ...
                    'msk'   , ioData.msk, ...
                    'edited', []         );
    
    
end



%%------------------------------------------------------------
%
% AUTHORS
%
%   Alexandros-Stavros Iliopoulos       ailiop@cs.duke.edu
%
% VERSION
%
%   0.1 - January 06, 2017
%
% CHANGELOG
%
%   0.1 (Jan 06, 2017) - Alexandros
%       * initial implementation
%
% ------------------------------------------------------------
