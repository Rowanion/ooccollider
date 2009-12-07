CC=mpic++
LD=$(CC)
HOMEDIR=/home-pc2/user/ava
CPPFLAGS = -Wall -Wno-deprecated -O3 -fPIC -ffast-math -funroll-loops -msse2 -I../include -I$(HOMEDIR)/Diplom/usr/include -I$(HOMEDIR)/Diplom/usr/include/ooctools -I/home-pc2/user/ava/Diplom/include
# `sdl-config --cflags`
LFLAGS = -fPIC -L$(HOMEDIR)/Diplom/usr/lib64 -L$(HOMEDIR)/Diplom/usr/lib -L$(HOMEDIR)/Diplom/lib  -L/usr/lib64 -L/usr/X11R6/lib -lGL -lGLU -lGLEW -lCg -lCgGL -lglut -lboost_system-gcc41-mt-1_38 -lboost_filesystem-gcc41-mt -lpthread -looctools 

HEADERFILES = declarations.h FileHeader.h FileIO.h IModelLoader.h ModelWriter.h ObjModelLoader.h OctreeHandler.h RawModelLoader.h RawModelWriter.h 

OBJS = FileHeader.o FileIO.o IModelLoader.o ModelWriter.o ObjModelLoader.o OctreeHandler.o RawModelLoader.o RawModelWriter.o

SRCS = FileHeader.cpp FileIO.cpp IModelLoader.cpp ModelWriter.cpp ObjModelLoader.cpp OctreeHandler.cpp RawModelLoader.cpp RawModelWriter.cpp

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

