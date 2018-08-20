
Project Members: X* Li, Kangnan Li


# Overview

In temporal compressive imaging, coded aperture has been applied to spatially compress multiple images into one, meanwhile using compress sensing algorithms all source images can be recovered from the single measurement. This technique has great advantage in many applications such as increasing camera frame rate, overcoming digital video resolution and temporal/spectral coherence reconstruction.


# files included


program files
Ploop.cpp  omp_cilk_mex.cpp   XL_KSVD_simu.m  Makefile

compile
type in: make 

to check the correctness of the c++ code, run ./Ploop directly. 

to run the whole program:
LD_PRELOAD="/usr/lib/x86_64-linux-gnu/libstdc++.so.6" matlab -nodisplay -r "run('/home/kl186/OMP/XL_KSVD_simu.m');"

(we saved the outpur data into data.mat, which could be plotted with matlab to get the picture in the report)

to change the image size, change:
 S in XL_KSVD_simu.m 
to change the workers, change
workers in XL_KSVD_simu.m 