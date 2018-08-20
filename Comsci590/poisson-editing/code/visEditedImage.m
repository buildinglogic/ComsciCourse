function visEditedImage (h, Iedit)
%
% VISEDITEDIMAGE - Display Poisson-edited image
%   
% SYNTAX
%
%    = VISEDITEDIMAGE(  )
%
% INPUT
%
%   
%
% OUTPUT
%
%   
%
% DESCRIPTION
%
%    = VISEDITEDIMAGE()
%
% DEPENDENCIES
%
%   
%
%
% See also      
%
    
    
    %% INITIALIZATION
    
    figure(h);
    subplot( 2, 2, 4 )
    imshow( Iedit )
    title( 'edited image' )
    
    
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
