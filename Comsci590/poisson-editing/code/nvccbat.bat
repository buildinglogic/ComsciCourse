echo off
set PATH=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\;%PATH%
set PATH=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\bin;%PATH%
call vcvars32.bat
nvcc %1 %2 %3 %4 %5 %6 %7 %8 %9 
