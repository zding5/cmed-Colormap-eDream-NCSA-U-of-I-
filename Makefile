# Makefile for cmed - please do not edit if the file is named "Makefile",
# since configure would overwrite it on the next configure.
# Edit "Makefile.in", and run configure instead!!!
# If you must change configure.in, run autoconf, and then configure etc.

# the following variables are under AC control
# KIRA_INC, KIRA_LIB
# FLTK_INC, FLTK_LIB
# PV_FLAGS

SHELL = /bin/sh

# TARGET
TARGET   = cmed

FLTK_INC = -I/bd2/shome/spin/zding5/fltk-1.3.3 -I/usr/include/freetype2 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_THREAD_SAFE -D_REENTRANT
FLTK_LIB = -L/bd2/shome/spin/zding5/fltk-1.3.3/lib -lfltk_images -lpng -lz -ljpeg -lfltk_gl -lGLU -lGL -lfltk -lXcursor -lXfixes -lXext -lXft -lfontconfig -lXinerama -lpthread -ldl -lm -lX11


# FLAGS and BINARIES
CC          = gcc
CXX	    = g++
LINK        = ${CXX} ${OTYPE}
MAKEGUI	    = true

GL_LIB	    = -lGLU -lGL
X_LIB	    =   -lSM -lICE -lXext -lX11 
M_LIB	    = -lm
LIBS        = $(FLTK_LIB) $(GL_LIB) $(X_LIB) $(M_LIB)

DEFS	    = $(PV_FLAGS)
INCS	    = $(FLTK_INC)
OPT	    = -g -O2
#OPT			= -g -O0
# CFLAGS	    = $(OPT) $(DEFS) $(INCS) -g -O2  -I/bd2/shome/spin/zding5/fltk-1.3.3 -I/usr/include/freetype2 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_THREAD_SAFE -D_REENTRANT
# CXXFLAGS    = $(OPT) $(DEFS) $(INCS) -g -O2 -I/bd2/shome/spin/zding5/fltk-1.3.3 -I/usr/include/freetype2 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_THREAD_SAFE -D_REENTRANT

CFLAGS	    = $(OPT) $(DEFS) $(INCS) $(OPT)  -I/bd2/shome/spin/zding5/fltk-1.3.3 -I/usr/include/freetype2 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_THREAD_SAFE -D_REENTRANT
CXXFLAGS    = $(OPT) $(DEFS) $(INCS) $(OPT) -I/bd2/shome/spin/zding5/fltk-1.3.3 -I/usr/include/freetype2 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_THREAD_SAFE -D_REENTRANT


APP_CSRCS   = hsb.c
APP_CXXSRCS = cmed.C cmedpanel.C CMedit.C

APP_OBJS    = cmed.o CMedit.o cmedpanel.o hsb.o

$(TARGET):  $(APP_OBJS)
	$(CXX) -o $@ $(OPT) $(OTYPE)  $(APP_OBJS) $(LIBS)
	$(MAKEGUI)

clean:
	rm -f $(TARGET) $(APP_OBJS)

dep: depend
depend:	_always
	rm -f Makedepend
	$(CC) -M $(CFLAGS) $(APP_CSRCS) > Makedepend
	$(CXX) -M $(CXXFLAGS) $(APP_CXXSRCS) >> Makedepend

_always:

include Makedepend
