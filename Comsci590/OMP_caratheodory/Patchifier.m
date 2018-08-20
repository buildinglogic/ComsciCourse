classdef Patchifier
properties
    T = 0
    N = []
    n = []
    d = []
    
    nPatch = 0
    start = []
    
    ind = {}
    weight = []
end

methods
    function o = Patchifier(imORsize, n, d)
        %imORsize is an image w/desired size or a vector w/desired image
        %dimensions
        %n is the patch size
        %d is the spacing between patches
        
        o.T = length(n);
        if isvector(imORsize), o.N = imORsize;
        else for t=1:o.T, o.N(t) = size(im,t); end
        end
        o.n = n;
        o.d = d;
        
        for t=1:o.T, start{t} = uint16(1 : o.d(t) : o.N(t) - o.n(t) + 1); end
        [start{:}] = ndgrid(start{:});
        for t=1:o.T, o.start(:,t) = start{t}(:); end
        
        o.nPatch = size(o.start,1);
        
        o.weight = zeros(o.N);
        nm1 = o.n - 1;
        for k = 1:o.nPatch
            for t=1:o.T, o.ind{k,t} = o.start(k,t):o.start(k,t) + nm1(t); end
            o.weight(o.ind{k,:}) = o.weight(o.ind{k,:}) + 1;
        end
    end
    
    function p = getPatches(o, im)
        p = zeros(prod(o.n), o.nPatch);
        for k = 1:o.nPatch
            pk = im(o.ind{k,:});
            p(:, k) = pk(:);
        end
    end
    
    function im = makeImage(o, p)
        im = zeros(o.N);
        for k = 1:o.nPatch
            im(o.ind{k,:}) = im(o.ind{k,:}) + reshape(p(:, k), o.n);
        end
        im = im./o.weight;
        im(isnan(im)) = 0;
    end
end

end
