function [ijBtmRgt, height, width] = ...
        sizeEditRegion (sizeSrc, sizeTgt, ijTopLft)
%
% SIZEEDITREGION - Compute height and width of source image in
%                  edited target domain 
%   
% SYNTAX
%
%   [IJBTMRGT] = SIZEEDITREGION( SIZESRC, SIZETGT, IJSRCTOPLFT )
%   [IJBTMRGT, HEIGHT, WIDTH] = SIZEEDITREGION( ... )
%
% INPUT
%
%   SIZESRC     Size of source image            [Ms, Ns, ...]
%   SIZETGT     Size of target image            [Mt, Nt, ...]
%   IJSRCTOPLFT Top-left pixel coordinates of   [i_top, j_left]
%               source in edited target image
%
% OUTPUT
%
%   IJSRCBTMRGT Bottom-right pixel coordinates  [i_bottom, j_right]
%               of source in edited target image
%   HEIGHT      Height of edited-domain source  [scalar]
%   WIDTH       Width of edited-domain source   [scalar]
%
% DESCRIPTION
%
%   [IJBTMRGT] = SIZEEDITREGION(SIZESRC,SIZETGT,IJSRCTOPLFT) returns a
%   2-vector with the (i,j) coordinates of the bottom-right corner of the
%   source domain in the edited image.
%
%   [IJBTMRGT,HEIGHT,WIDTH] = SIZEEDITREGION(SIZESRC,SIZETGT,IJSRCTOPLFT)
%   also returns the height and width of the source domain in the edited
%   image.
%
%   SIZEEDITREGION truncates the edited region size if it would fall
%   outside the target image domain.
%
% DEPENDENCIES
%
%   <none>
%
%
% See also      poissonEditingJacobi, visInputImages
%
    
    
    iBtm = min( sizeTgt(1), ijTopLft(1) + sizeSrc(1) - 1 );
    jRgt = min( sizeTgt(2), ijTopLft(2) + sizeSrc(2) - 1 );
    
    height = iBtm - ijTopLft(1) + 1;
    width  = jRgt - ijTopLft(2) + 1;
    
    ijBtmRgt = [iBtm, jRgt];
    
    
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
