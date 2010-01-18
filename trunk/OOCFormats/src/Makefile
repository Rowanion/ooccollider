CC=mpic++
LD=$(CC)
#HOMEDIR=/home-pc2/user/ava
HOMEDIR=/scratch/tsuess/ava
CPPFLAGS = -Wall -Wno-deprecated -O3 -fPIC -ffast-math -funroll-loops -msse2 -I../include -I$(HOMEDIR)/Diplom/usr/include -I../../OOCTools/include/ -I$(HOMEDIR)/Diplom/include
# `sdl-config --cflags`
LFLAGS = -fPIC -L$(HOMEDIR)/Diplom/usr/lib64 -L$(HOMEDIR)/Diplom/usr/lib -L$(HOMEDIR)/Diplom/lib  -L/usr/lib64 -L/usr/X11R6/lib -lmpi -lGL -lGLU -lGLEW -lCg -lCgGL -lglut -lboost_system-gcc41-mt-1_38 -lboost_filesystem-gcc41-mt -lpthread -looctools 

HEADERFILES = declarations.h FileHeader.h FileIO.h IModelLoader.h LooseOctree.h LooseProcessingOctree.h LooseRenderOctree.h ModelWriter.h ObjModelLoader.h OctreeHandler.h RawModelHandler.h RawModelLoader.h RawModelWriter.h TriangleStream.h 

OBJS = FileHeader.o FileIO.o IModelLoader.o LooseOctree.o LooseProcessingOctree.o LooseRenderOctree.o ObjModelLoader.o OctreeHandler.o RawModelHandler.o TriangleStream.o

SRCS = FileHeader.cpp FileIO.cpp IModelLoader.cpp LooseOctree.cpp LooseProcessingOctree.cpp LooseRenderOctree.cpp ObjModelLoader.cpp OctreeHandler.cpp RawModelHandler.cpp TriangleStream.cpp

all : liboocformats.so 
	chmod g+rw *.o

clean :
	rm -rf liboocformats.so *.o

liboocformats.so : ${OBJS}
	$(LD) -shared -o $@ ${OBJS} ${LFLAGS}

depend :
	makedepend ${SRCS}

install :
	cp liboocformats.so $(HOMEDIR)/Diplom/usr/lib
	cp ../include/*.h $(HOMEDIR)/Diplom/usr/include/oocformats

