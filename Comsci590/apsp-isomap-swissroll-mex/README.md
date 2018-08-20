Demo for dimensionality reduction of 3D points on a Swiss roll manifold using
the ISOMAP algorithm, which entails computation of the all-pairs shortest
distance matrix.


A demo of dimensionality reduction with ISOMAP [1] is provided in MATLAB. ISOMAP consists of 3 steps: (i) construction of neighborhood graph of a set of points; (ii) computation of all-pairs shortest path distances (APSD); and (iii) lower-dimensional spectral embedding. Develop parallel implementations for the APSD step that utilize multicore architectures. The demo uses a simple MATLAB implementation of the Floyd-Warshall algorithm.