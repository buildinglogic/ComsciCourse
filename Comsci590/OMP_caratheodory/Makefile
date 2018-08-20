#
# Sample Cilk makefile
#
# Usage:		[compile with:]
#	make			[g++]
#	make CC=icpc		[icpc]
#	make CC=icc		[icc]
#	make CC=g++		[g++]
#	make CC=gcc		[gcc]
#	make CC=clang		[clang]
#
#
# ***NOTE: compilation for clang (for OS X) has not been tested!  If
# ***      you try make it work, please post your solution in the
# ***      Sakai forum.
#
#
# --------------------------------------------------
# CompSci 590.06: Parallel Programming
# Department of Computer Science
# Duke University
# --------------------------------------------------
#



########## DEFAULT COMPILER

CC = g++


########## HEADER/LIBRARY PATHS

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# ***NOTE*** The parameters below are set up for the caratheodory filesystem.
#            Edit as appropriate.
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

PATH_CILK     = /usr/pkg/intel/compilers_and_libraries/include

##CORE_PATH     =/usr/include/opencv2

##PATH_OPENCV	  = /home/kl186/opencv/include/opencv

PATH_CPP_BITS = /usr/include/x86_64-linux-gnu/c++/5

PATH_MATLAB   = /usr/pkg/matlab-2017a

PATH_INTEL    = /usr/pkg/intel/compilers_and_libraries/linux
TGT_INTEL     = intel64

ARCH          = glnxa64
MEXEXT        = .mexa64

SHELL         = /bin/bash
CMD_SOURCE    = source


########## CILK COMPILATION SET-UP

# ---------- icpc

ifeq ($(CC),icpc)
CC     = icpc
INC    = -I$(PATH_CILK) -I$(PATH_CPP_BITS) 
CFLAGS = -O3 -fPIC
LFLAGS = -L/home/kl186/opencv/lib -lopencv_features2d
endif

# ---------- g++

ifeq ($(CC),g++)
CC = g++
INC    = -I$(PATH_CILK) -I$(PATH_CPP_BITS) 
CFLAGS = -O3 -fcilkplus -fPIC -lcilkrts
endif

# ---------- clang

ifeq ($(CC),clang)
CC     = clang
INC    = -I$(PATH_CILK) -I$(PATH_CPP_BITS) 
CFLAGS = -O3 -fcilkplus -fPIC
endif


########## MEX COMPILATION SET-UP

# ---------- icpc

ifeq ($(CC),icpc)
MEX         = icpc
MEXINC      = -I$(PATH_MATLAB)/extern/include
MEXRPATH    = -Wl,rpath-link,$(PATH_MATLAB)/bin/$(ARCH)
MEXLIB      = -L$(PATH_MATLAB)/bin/$(ARCH)
MEXLIB     += -lmx -lmex -lmat -lm
MEXFLAGS    = -O3 -fPIC -fno-omit-frame-pointer -pthread
MEXDEBUG    = -DNDEBUG
MEXLDFLAGS  = -pthread -shared -Wl,--no-undefined
endif

# ---------- g++

ifeq ($(CC),g++)
MEX         = g++
MEXINC      = -I$(PATH_MATLAB)/extern/include
MEXRPATH    = -Wl,rpath-link,$(PATH_MATLAB)/bin/$(ARCH)
MEXLIB      = -L$(PATH_MATLAB)/bin/$(ARCH)
MEXLIB     += -lmx -lmex -lmat -lm -lcilkrts
MEXFLAGS    = -O3 -fPIC -fno-omit-frame-pointer -pthread
MEXDEBUG    = -DNDEBUG
MEXLDFLAGS  = -pthread -shared -Wl,--no-undefined
endif

# ---------- clang

ifeq ($(CC),clang)
MEX         = clang
MEXINC      = -I$(PATH_MATLAB)/extern/include
MEXRPATH    = -Wl,rpath-link,$(PATH_MATLAB)/bin/$(ARCH)
MEXLIB      = -L$(PATH_MATLAB)/bin/$(ARCH)
MEXLIB     += -lmx -lmex -lmat -lm -lc++ -lcilkrts
MEXFLAGS    = -O3 -fPIC -fno-common -fexceptions -arch $(ARCH) -fcilkplus
MEXDEBUG    = -DNDEBUG
MEXLDFLAGS  = -arch $(ARCH) -bundle -Wl,-exported_symbols_list,$(PATH_MATLAB)/extern/lib/$(ARCH)/mexFunction.map
endif



########## COMPILATION RULES


##.PHONY: testmex test intelsetup

all: Ploop omp_cilk_mex$(MEXEXT)


# ----- compilation

omp_cilk_mex$(MEXEXT): omp_cilk_mex.o Ploop.o
	$(MEX) $(MEXINC) -o $@ $+ $(MEXLIB) $(MEXLDFLAGS) `pkg-config opencv --libs` -I/usr/include -lstdc++ 

omp_cilk_mex.o: omp_cilk_mex.cpp
	$(MEX) -c $(MEXINC) -o $@ $< $(MEXRPATH) $(MEXLIB) $(MEXFLAGS) $(MEXDEBUG)

Ploop: Ploop.o
	$(CC) $(INC) -o $@ $+ $(CFLAGS) -I/usr/include `pkg-config opencv --libs`

Ploop.o: Ploop.cpp
	$(CC) -c $(INC) -o $@ $< $(CFLAGS) -I/usr/include 


# ---------- tests

#test: calculateMatrix
#	./$<

#testmex: calculateMatrix_mex.$(MEXEXT)
#	$(MATLAB) -r "test_calculateMatrix_mex; exit"


# ---------- Intel compiler tools set-up

intelsetup:
	$(CMD_SOURCE) $(PATH_INTEL)/bin/compilervars.sh $(TGT_INTEL)


# ---------- clean-up

clean:
	rm -f Ploop Ploop.o omp_cilk_mex.mexa64 omp_cilk_mex.o *~
